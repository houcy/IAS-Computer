#include "IASFrontEnd.h"

#include <sstream>
#include <iomanip>
#include <bitset>
#include <thread>
#include <chrono>
#include <iostream>

namespace
{
    auto getDecAndBinString(Word value)
    {
        std::ostringstream stream;
        stream << std::setw(4) << (int)value << " " << std::bitset<8>(value);
        return stream.str();
    }
}

IASFrontEnd::IASFrontEnd(const Memory& memory)
:   m_window        ({1280, 720}, "8-bit IAS Computer")
,   m_iasComputer   (memory)
{
    m_window.setFramerateLimit(30);
    m_mainFont.loadFromFile("res/Anonymous.ttf");

    initRegisterDisplay();
    initInstructionDisplay();
}

//Runs the computer, either using CLI or GUI
void IASFrontEnd::run(bool useGui)
{
    if (useGui) {
        sf::Clock c;
        while (m_window.isOpen()) {
            m_window.clear();

            //Cycle every n seconds, updates display in process
            if (c.getElapsedTime() > sf::seconds(0.75)) {
                cycleComputer();
                c.restart();
            }
            render();

            m_window.display();
            tryCloseWindow();
            if (m_iasComputer.getOpcodeFromInstr() == INSTRUCTION_END) break;
        }
    }
    else {
        m_window.close();
        m_iasComputer.run();
    }
    m_window.close();
}



//Check if user has closed window
void IASFrontEnd::tryCloseWindow()
{
    sf::Event e;
    while (m_window.pollEvent(e)) {
        if (e.type == sf::Event::Closed) {
            m_window.close();
        }
    }
}

//Updates the values of the register displays to those stored in the IAS computer
void IASFrontEnd::updateRegisterDisplay()
{
    auto* start = m_iasComputer.getFirstRegister(); //Get pointer to first register
    for (int i = 0; i < NUM_REGISTERS; i++) {
        std::ostringstream stream;
        Word value = *(start + i);  //Offset pointer by "i", to find correct value
        m_registerValueDisplay[i].setString(getDecAndBinString(value));
    }
}

void IASFrontEnd::updateInstructionDisplay()
{
    auto& instrDisp = m_instructionDisplays[0].second;
    auto& addrDisp  = m_instructionDisplays[1].second;
    auto& descDisp  = m_instructionDisplays[2].second;

    Word opcode         = m_iasComputer.getOpcodeFromInstr();
    Word address        = m_iasComputer.getMemAddrFromInstr();
    descDisp.setString(opcodeString.at(opcode));

    instrDisp.setString(getDecAndBinString(opcode));
    addrDisp.setString(getDecAndBinString(address));


}


//Does 1 fetch-execute cycle
void IASFrontEnd::cycleComputer()
{
    m_iasComputer.fetch();
    updateRegisterDisplay();
    updateInstructionDisplay();
    m_iasComputer.execute();
}

//Renders the displays
void IASFrontEnd::render()
{
    m_registerSect      .draw(m_window);
    m_instructionSect   .draw(m_window);

    for (int i = 0; i < NUM_REGISTERS; i++) {
        m_window.draw(m_registerDisplay[i]);
        m_window.draw(m_registerValueDisplay[i]);
    }

    for (int i = 0; i < 3; i++) {
        m_window.draw(m_instructionDisplays[i].first);
        m_window.draw(m_instructionDisplays[i].second);
    }
}

void IASFrontEnd::initRegisterDisplay()
{
    int charSize = 15;
    sf::Font& f = m_mainFont;

    m_registerDisplay.emplace_back("   Accumulator Register: ", f);
    m_registerDisplay.emplace_back(" Memory Buffer Register: ", f);
    m_registerDisplay.emplace_back("Memory Address Register: ", f);
    m_registerDisplay.emplace_back("   Instruction Register: ", f);
    m_registerDisplay.emplace_back("        Program Counter: ", f);
    m_registerDisplay.emplace_back("    IO Address Register: ", f);
    m_registerDisplay.emplace_back("     IO Buffer Register: ", f);

    for (int i = 0; i < NUM_REGISTERS; i++) {
        m_registerValueDisplay.emplace_back("Not Used Yet", f);
    }

    //Move the display texts into the correct location
    for (int y = 0; y < NUM_REGISTERS; y++) {
        float yPosition = REG_GUI_Y + 20 + y * TEXT_HEIGHT;
        m_registerDisplay[y].setCharacterSize(charSize);
        m_registerDisplay[y].move(REG_GUI_X, yPosition);

        m_registerValueDisplay[y].setCharacterSize(charSize);
        m_registerValueDisplay[y].move(REG_GUI_X + 260, yPosition);
    }

    m_registerSect.init("Registers", {REG_GUI_X, REG_GUI_Y}, {460, 230}, m_mainFont);
}

void IASFrontEnd::initInstructionDisplay()
{
    int charSize = 15;
    sf::Font& f = m_mainFont;

    //init the text objects
    sf::Text opcode         ("     Opcode: ", f);
    sf::Text address        ("    Address: ", f);
    sf::Text description    ("Description: ", f);
    sf::Text opcodeVal      ("", f);
    sf::Text addressVal     ("", f);
    sf::Text descVal        ("", f);

    //Add them to the vector
    m_instructionDisplays.insert(m_instructionDisplays.end(), {
        std::make_pair(opcode,      opcodeVal),
        std::make_pair(address,     addressVal),
        std::make_pair(description, descVal),
    });

    //Move to correct location
    for (int i = 0; i < 3; i++) {
        auto& disp = m_instructionDisplays[i];
        disp.first  .setCharacterSize(charSize);
        disp.second .setCharacterSize(charSize);

        disp.first  .move(INS_GUI_X,        INS_GUI_Y + 20 + i * TEXT_HEIGHT);
        disp.second .move(INS_GUI_X + 125,  INS_GUI_Y + 20 + i * TEXT_HEIGHT);
    }

    m_instructionSect.init("Opcode and Address", {INS_GUI_X, INS_GUI_Y}, {1000, 130}, m_mainFont);
}


void IASFrontEnd::Section::init(const std::string& title,  const sf::Vector2f& position, const sf::Vector2f& size, const sf::Font& font)
{
    m_titleText.setOutlineThickness(1);
    m_titleText.setFillColor({200, 200, 200});
    m_titleText.setCharacterSize(17);

    m_background.setSize(size);
    m_background.setPosition(position);

    m_background.setOutlineThickness(2);
    m_background.setOutlineColor({150, 150, 150});
    m_background.setFillColor({100, 100, 100});

    m_titleText.setFont     (font);
    m_titleText.move        (position);
    m_titleText.setString   (title);
}

void IASFrontEnd::Section::draw(sf::RenderWindow& window)
{
    window.draw(m_background);
    window.draw(m_titleText);

}


