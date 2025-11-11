#include "VerilogGenerator.h"
#include <fstream>
#include <iostream>
using namespace std;

void VerilogGenerator::generateVerilog(const vector<string>& implicants, int varCount) {
    ofstream file("output.v");
    if (!file.is_open()) {
        cerr << "Error: Could not create Verilog file.\n";
        return;
    }

    file << "module func(output F";
    for (int i = 0; i < varCount; ++i)
        file << ", input x" << i;
    file << ");\n\n";

    // Define negation wires
    for (int i = 0; i < varCount; ++i)
        file << "  wire nx" << i << ";\n";
    file << "\n";

    // NOT gates
    for (int i = 0; i < varCount; ++i)
        file << "  not(nx" << i << ", x" << i << ");\n";
    file << "\n";

    // AND gates for each implicant
    for (size_t i = 0; i < implicants.size(); ++i) {
        file << "  wire w" << i << ";\n";
        file << "  and(w" << i;
        for (int j = 0; j < varCount; ++j) {
            if (implicants[i][j] == '0')
                file << ", nx" << j;
            else if (implicants[i][j] == '1')
                file << ", x" << j;
        }
        file << ");\n";
    }

    // OR gate for output
    file << "\n  or(F";
    for (size_t i = 0; i < implicants.size(); ++i)
        file << ", w" << i;
    file << ");\n";

    file << "endmodule\n";
    file.close();

    cout << "Verilog file generated successfully: output.v" << endl;
}
