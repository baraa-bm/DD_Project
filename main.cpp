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

// we added this function for the input validation to check that the variables enterd are numbers
bool isNumber(const string& s) {
    if (s.length() == 0) return false;
    for (int i = 0; i < s.length(); i++) {
        if (s[i] < '0' || s[i] > '9') return false;
    }
    return true;
}

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

    if (!isNumber(l1)) {
        cout << "First line must only contain a number." << endl;
        return 1;
    }

    input.nVariables = stoi(l1);

    if (input.nVariables < 1 || input.nVariables > 20) {
        cout << "Error: Number of variables must be between 1 and 20." << endl;
        return 1;
    }

    int maxVal = (1 << input.nVariables) - 1;;
    // the maximum input we can have is 2^n -1
    if (l2.length() == 0) {
        cout << " Second line is empty." << endl;
        return 1;
    }

    if (!(l2[0] == 'm' || l2[0] == 'M')) {
        cout << "Second line must start with m or M." << endl;
        return 1;
    }

    bool MaxTerm = (l2[0] == 'M');
    char first = l2[0];
    //inputting the first character of the second line to determine if they max or minterms

// here we are using parsing to disregard the comma and only input the terms in the second line
       string token = "";
    for (int i = 0; i <= l2.length(); i++) {
        if (i == l2.length() || l2[i] == ',') {
            if (token.length() > 0 && token[0] == first) {

                string numPart = token.substr(1);

                if (!isNumber(numPart)) {
                    cout << "Invalid term: " << token << endl;
                    return 1;
                }

                int num = stoi(numPart);

                if (num < 0 || num > maxVal) {
                    cout << "Error: Term " << num << " out of range." << endl;
                    return 1;
                }

                // duplicate check using loops
                bool dup = false;
                for (int j = 0; j < input.minterms.size(); j++) {
                    if (input.minterms[j] == num) {
                        dup = true;
                        break;
                    }
                }
                if (dup) {
                    cout << "Error: Duplicate term: " << num << endl;
                    return 1;
                }

                input.minterms.push_back(num);
            }
            token = "";
        } else {
            token += l2[i];
        }
    }

    // Parsing line 3 (don't cares) like line 2
    if (!l3.empty()) {
        if (l3[0] == 'd' || l3[0] == 'D') {
            char first_d = l3[0];
            string token_d = "";

            for (int i = 0; i <= l3.length(); i++) {
                if (i == l3.length() || l3[i] == ',') {
                    if (token_d.length() > 0 && token_d[0] == first_d) {
                        string numPart = token_d.substr(1);
                        if (!isNumber(numPart)) {
                            cout << "Invalid dont-care term: " << token_d << endl;
                            return 1;
                        }
                        int num = stoi(numPart);
                        if (num < 0 || num > maxVal) {
                            cout << "Error: Dont-care term " << num << " out of range." << endl;
                            return 1;
                        }
                        // Duplicate check in don't cares
                        bool dup_d = false;
                        for (int j = 0; j < input.dontCares.size(); j++) {
                            if (input.dontCares[j] == num) {
                                dup_d = true;
                                break;
                            }
                        }
                        if (dup_d) {
                            cout << "Error: Duplicate dont-care term: " << num << endl;
                            return 1;
                        }
                        input.dontCares.push_back(num);
                    }
                    token_d = "";
                } else {
                    token_d += l3[i];
                }
            }
        } else {
            cout << "Third line must start with d or D if provided." << endl;
            return 1;
        }
    }




    // here we are checking that the term inputted in min/maxterms is not inputted in the dont cares as well
    for (int i = 0; i < input.dontCares.size(); i++) {
        for (int j = 0; j < input.minterms.size(); j++) {
            if (input.dontCares[i] == input.minterms[j]) {
                cout << "Error: Term " << input.dontCares[i]
                     << " appears in both minterms and dont-cares." << endl;
                return 1;
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
