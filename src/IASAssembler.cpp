#include "IASAssembler.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <sstream>
#include <vector>

namespace {
const std::unordered_map<std::string, Word> strToOpcode{
    {"add", 0}, {"sub", 1}, {"sta", 2}, {"lda", 3},
    {"inp", 4}, {"out", 5}, {"jip", 6}, {"end", 7}};

// Splits a string at spaces
auto split(const std::string &str)
{
    std::istringstream stream(str);
    std::vector<std::string> tokens{std::istream_iterator<std::string>(stream),
                                    std::istream_iterator<std::string>()};
    return tokens;
}

// Converts string to lower case
void toLowerCase(std::string &str)
{
    std::transform(str.begin(), str.end(), str.begin(), ::tolower);
}

} // namespace

IASAssembler::IASAssembler()
{
    m_assembledCode.fill(0);
}

// Input a file, outputs a program
Memory IASAssembler::assembleFile(const char *fileName)
{
    std::ifstream inFile(fileName);

    if (inFile.good()) {
        parse(inFile);
    }
    else {
        std::cout << "File " << fileName
                  << " does not exist.\n";
    }
    m_assembledCode[m_instructionCount] = INSTRUCTION_END << MEMORY_BITS;
    return m_assembledCode;
}

// Loop through lines and put into memory
void IASAssembler::parse(std::ifstream &inFile)
{
    std::string line;
    while (std::getline(inFile, line)) {
        parseLine(line);
    }
}

// Parse an single line; extracts both memory and address part of instruction
void IASAssembler::parseLine(const std::string &line)
{
    auto tokens = split(line);
    toLowerCase(tokens.at(0));
    auto opcode = strToOpcode.at(tokens.at(0));

    Word instruction = opcode << MEMORY_BITS;
    if (tokens.size() == 2) {
        Word address = (Word)std::stoi(tokens[1]);
        instruction |= address;
    }
    m_assembledCode[m_instructionCount++] = instruction;
}
