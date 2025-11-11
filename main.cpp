#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <bitset>
#include "TermGroup.h"
#include "Simplifier.h"
#include "VerilogGenerator.h"

using namespace std;

struct Data {
    int nVariables;
    vector<int> minterms;
    vector<int> dontCares;
};

int main() {
    string filename;
    Data input;
    ifstream file;

    // --- Step 1: Read input file ---
    while (true) {
        cout << "Enter the file name you want to test: ";
        cin >> filename;
        file.open(filename);
        if (file.is_open()) break;
        cout << "File is not open. Please try again." << endl;
    }

    string l1, l2, l3;
    getline(file, l1);
    getline(file, l2);
    getline(file, l3);

    input.nVariables = stoi(l1);
    int maxVal = (1 << input.nVariables) - 1;

    char first = l2[0];
    bool MaxTerm = (first == 'M');

    string token;
    stringstream ss(l2);
    while (getline(ss, token, ',')) {
        if (token[0] == first) {
            input.minterms.push_back(stoi(token.substr(1)));
        }
    }

    if (!l3.empty() && l3[0] == 'd') {
        stringstream ss2(l3);
        while (getline(ss2, token, ',')) {
            if (token[0] == 'd') input.dontCares.push_back(stoi(token.substr(1)));
        }
    }

    // --- Step 2: Convert maxterms to minterms if needed ---
    if (MaxTerm) {
        vector<int> newMins;
        for (int i = 0; i <= maxVal; i++) {
            if (find(input.minterms.begin(), input.minterms.end(), i) == input.minterms.end())
                newMins.push_back(i);
        }
        input.minterms = newMins;
    }

    // --- Step 3: Generate prime implicants ---
    TermGroup group(input.nVariables, MaxTerm);
    vector<Term> PIs = group.generatePrimeImplicants(input.minterms, input.dontCares);

    cout << "All Prime Implicants:" << endl;
    group.printPrimeImplicants();

    // --- Step 4: Minimize using Simplifier ---
    Simplifier simplifier(input.minterms, input.dontCares, input.nVariables);
    string minimizedExpr = simplifier.simplify();
    cout << "Minimized Boolean Expression: " << minimizedExpr << endl;

    // --- Step 5: Convert minimized expression to binary patterns for Verilog ---
    // Note: Assuming Simplifier can return binary patterns if needed
    // For simplicity, here we just use prime implicants for Verilog generator
    vector<string> finalImplicants;
    for (auto &pi : PIs) {
        finalImplicants.push_back(pi.binary);
    }

    // --- Step 6: Generate Verilog Module ---
    VerilogGenerator verilogGen;
    verilogGen.generateVerilog(finalImplicants, input.nVariables);

    return 0;
}
