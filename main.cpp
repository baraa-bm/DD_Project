#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <set>
#include <string>
#include <bitset>
#include <filesystem>
#include "TermGroup.h"
#include "Simplifier.h"

using namespace std;

struct Data {
    int nVariables;
    vector<int> minterms;
    vector<int> dontCares;
    vector<string> mBinary;
    vector<string> dBinary;
};
int main()
{
    string tests;
Data input;
    ifstream file;
    //here we are making sure the file is opened before continuing
    while (true) {
        cout << "Enter the file name you want to test: ";
        cin >> tests;

        file.open(tests);
        if (file.is_open())
            break;
        cout << "File is not open. Please try again."<<endl;
    }


        string l1, l2, l3;
        getline(file, l1);
        getline(file, l2);
        getline(file, l3);

        input.nVariables = stoi(l1);
        int maxVal = (1 << input.nVariables) - 1;
    // the maximum input we can have is 2^n -1
        char first = l2[0];
    //inputting the first character of the second line to determine if they max or minterms
        bool MaxTerm = (first == 'M');

        string token;
        stringstream ss(l2);
// here we are using parsing to disregard the comma and only input the terms in the second line
        while (getline(ss, token, ',')) {
            if (token[0] == first) {
                int num = stoi(token.substr(1));
                input.minterms.push_back(num);
            }
        }

        // same parsing but for the third line
        if (!l3.empty() && l3[0] == 'd') {
            stringstream ss2(l3);
            while (getline(ss2, token, ',')) {
                if (token[0] == 'd') {
                    int num = stoi(token.substr(1));
                    input.dontCares.push_back(num);
                }
            }
        }
// we will convert any maxterms to minterms so its easier to process later
    if (MaxTerm) {
        vector<int> newMins;
        for (int i = 0; i <= maxVal; i++) {
            bool found = false;

            for (int j = 0; j < input.minterms.size(); j++) {
                if (input.minterms[j] == i) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                newMins.push_back(i);
            }
        }
        input.minterms = newMins;
    }
// we need to convert any terms to binary strings for later
    for (int i = 0; i < input.minterms.size(); i++) {
        int m = input.minterms[i];
        string bits = bitset<20>(m).to_string();
        input.mBinary.push_back(bits.substr(20 - input.nVariables));
    }
// we only have upto 20 variables

    vector<string> dBinary;
    for (int i = 0; i < input.dontCares.size(); i++) {
        int d = input.dontCares[i];
        string bits = bitset<20>(d).to_string();
        dBinary.push_back(bits.substr(20 - input.nVariables));
    }

    TermGroup group(input.nVariables, MaxTerm);

    //generating prime implicants
    vector<Term> PIs = group.generatePrimeImplicants(input.mBinary,input.minterms, dBinary,input.dontCares);

    //printing results
    cout << "Results" << endl;
    group.printPrimeImplicants();
    file.close();

    Simplifier simplifier(input.minterms,input.dontCares ,input.nVariables);

    string finalExpression = simplifier.simplify();

    cout << "Simplified Boolean Expression: " << finalExpression << endl;


    return 0;

}
