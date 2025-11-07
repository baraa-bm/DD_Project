//
// Created by janna on 05/11/2025.
//

#include "TermGroup.h"

//constructor
TermGroup::TermGroup(int vars, bool maxtermMode) : numVariables(vars), isMaxterm(maxtermMode) {}


//converts an integer to binary string
string TermGroup::toBinary(int value, int bits) {
string bin(bits, '0');
for (int i = bits - 1; i >= 0; i--) {
bin[i] = (value % 2) + '0';
value /= 2;
}
return bin;
}

//count numer of 1s in binary string
int TermGroup::countOnes(const string& binary) {
int count = 0;
for (char c : binary)
if (c == '1')
count ++;
return count;
}


//check if two terms differ by exactly one bit
bool TermGroup::canCombine(const string& a, const string& b) {
int diff = 0;
for (int i = 0; i < a.size(); i++) {
if (a[i] != b[i]) {
diff++;
if (diff > 1) return false;
}
}
return diff == 1;
}

//combine two terms that differ by one bit
string TermGroup::combine(const string& a, const string& b) {
string result = a;
for (int i = 0; i < a.size(); i++) {
if (a[i] != b[i])
result[i] = '-';
}
return result;
}

//convert maxterms to minterms
vector<int> TermGroup::convertMaxtermsToMinterms(const vector<int>& maxterms) {
int total = pow(2, numVariables);
vector<int> minterms;
for (int i = 0; i < total; ++i) {
if (find(maxterms.begin(), maxterms.end(), i) == maxterms.end())
minterms.push_back(i);
}
return minterms;

}


//generating prime implicants
vector<Term> TermGroup::generatePrimeImplicants(const vector<int>& terms, const vector<int>& dontCares) {
vector<int> minterms;

//if the input is maxterms, convert them to minterms first
if (isMaxterm)
minterms = convertMaxtermsToMinterms(terms);
else
minterms = terms;

// merge minterms and dont cares for combination
vector<int> total = minterms;
total.insert(total.end(), dontCares.begin(), dontCares.end());

//creating initial term list
vector<Term> allTerms;
for (int m : total)
allTerms.push_back(Term(toBinary(m, numVariables), {m}));

bool canCombineFurther = true;
vector<Term> current = allTerms;

// repeat grouping/combining until no more combinations possible
while (canCombineFurther) {
canCombineFurther = false;
vector<Term> nextLevel;
vector<vector<Term>> groups(numVariables + 1);

//group terms by number of 1s
for (auto& t : current)
groups[countOnes(t.binary)].push_back(t);

//compare each group with the next one
for (int i = 0; i < numVariables; ++i) {
    for (auto& a : groups[i]) {
        for (auto& b : groups[i + 1]) {
            if (canCombine(a.binary, b.binary)) {
            string combinedBin = combine(a.binary, b.binary);
            vector<int> merged = a.coveredMinterms;
            merged.insert(merged.end(), b.coveredMinterms.begin(), b.coveredMinterms.end());
            nextLevel.push_back(Term(combinedBin, merged));
            a.combined = b.combined = true;
            canCombineFurther = true;
                    }
                }
            }
        }
//umcombined are prime implicants
    for (auto& t : current) {
        if (!t.combined)
            primeImplicants.push_back(t);
    }

    current = nextLevel;
}

//remove duplicates
sort(primeImplicants.begin(), primeImplicants.end(),
[](const Term& a, const Term& b) { return a.binary < b.binary; });
primeImplicants.erase(unique(primeImplicants.begin(), primeImplicants.end(),
[](const Term& a, const Term& b) { return a.binary == b.binary; }), primeImplicants.end());

return primeImplicants;

}

//printing prime implicants

void TermGroup::printPrimeImplicants() const {
    cout << "Prime Implicants:" << endl;
    for (const auto& pi : primeImplicants) {
        cout << pi.binary << " covers { ";
        for (size_t i = 0; i < pi.coveredMinterms.size(); ++i) {
            cout << pi.coveredMinterms[i];
            if (i + 1 < pi.coveredMinterms.size()) cout << ", ";
        }
        cout << " }" << endl;
    }
}
