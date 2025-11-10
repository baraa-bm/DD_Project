//
// Created by janna on 05/11/2025.
//

#ifndef DD_PROJECT_TERMGROUP_H
#define DD_PROJECT_TERMGROUP_H

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cmath>

using namespace std;

//represents a single term
class Term {
public:
    string binary; //binary pattern
    vector<int> coveredMinterms; //which minterms this term covers
    bool combined; //if it has been merged

    Term (string bin, vector<int> mins) : binary(move(bin)), coveredMinterms(move(mins)), combined(false) {}
};

//class that handles both minterms and maxterms
class TermGroup {
private:
int numVariables; //number of variables
bool isMaxterm; //true if we're simplifying from maxterms
vector<Term> primeImplicants; //final list of prime implicants

string toBinary (int value, int bits); //convert int to binary string
int countOnes(const string& binary); //count 1s in binary string
bool canCombine(const string& a, const string& b); //check if differ by one bit
string combine(const string& a, const string& b); //replace differing bit with -
vector<int> convertMaxtermsToMinterms(const vector<int>& maxterms); //conversion helper

public:

//constructor, choose num of variables and whether its maxterm or minterm input
TermGroup(int vars, bool maxtermMode = false);

//generate all prime implicants given minterms/maxterms and dont cares
vector<Term> generatePrimeImplicants(const vector<int>& terms, const vector<int>& dontCares);

//print prime implicants
void printPrimeImplicants() const;

};


#endif //DD_PROJECT_TERMGROUP_H