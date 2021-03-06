#ifndef IASASSEMBLER_H_INCLUDED
#define IASASSEMBLER_H_INCLUDED

#include "IASComputer.h"

#include <fstream>

class IASAssembler {
  public:
    IASAssembler();

    Memory assembleFile(const char *fileName);

  private:
    void parse(std::ifstream &inFile);
    void parseLine(const std::string &line);

    Memory m_assembledCode;
    uint16_t m_instructionCount = 0;
};

#endif // IASASSEMBLER_H_INCLUDED
