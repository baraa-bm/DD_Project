#ifndef VERILOGGENERATOR_H
#define VERILOGGENERATOR_H

#include <string>
#include <vector>
using namespace std;

class VerilogGenerator {
public:
    void generateVerilog(const vector<string>& implicants, int varCount);
};

#endif
