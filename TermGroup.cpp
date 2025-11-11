//
// Created by janna on 05/11/2025.
//

#include "TermGroup.h"

//constructor
TermGroup::TermGroup(int vars, bool maxtermMode) : numVariables(vars), isMaxterm(maxtermMode) {}



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
vector<Term> TermGroup::generatePrimeImplicants(const vector<string>& termBinaries, const vector<int>& termNumbers, const vector<string>& dontCareBinaries, const vector<int>& dontCareNumbers) {
    vector<string> totalBinaries = termBinaries;
    vector<int> totalNumbers = termNumbers;
    for (int i = 0; i < (int)dontCareBinaries.size(); ++i) {
        totalBinaries.push_back(dontCareBinaries[i]);
        totalNumbers.push_back(dontCareNumbers[i]);
    }

    // Create initial terms with correct covered minterm numbers
    vector<Term> allTerms;
    for (int i = 0; i < (int)totalBinaries.size(); ++i) {
        allTerms.push_back(Term(totalBinaries[i], vector<int>{totalNumbers[i]}));
    }

    bool canCombineFurther = true;
    vector<Term> current = allTerms;

    while (canCombineFurther) {
        canCombineFurther = false;
        vector<Term> nextLevel;
        vector<vector<Term>> groups(numVariables + 1);

        // Group terms by count of ones
        for (int i = 0; i < (int)current.size(); ++i) {
            groups[countOnes(current[i].binary)].push_back(current[i]);
        }

        // Combine adjacent groups terms differing by one bit
        for (int i = 0; i < numVariables; ++i) {
            for (int j = 0; j < (int)groups[i].size(); ++j) {
                for (int k = 0; k < (int)groups[i + 1].size(); ++k) {
                    if (canCombine(groups[i][j].binary, groups[i + 1][k].binary)) {
                        string combinedBin = combine(groups[i][j].binary, groups[i + 1][k].binary);
                        vector<int> merged = groups[i][j].coveredMinterms;
                        vector<int>& rhsMinterms = groups[i + 1][k].coveredMinterms;
                        for (int m = 0; m < (int)rhsMinterms.size(); ++m) {
                            merged.push_back(rhsMinterms[m]);
                        }
                        nextLevel.push_back(Term(combinedBin, merged));
                        groups[i][j].combined = true;
                        groups[i + 1][k].combined = true;
                        canCombineFurther = true;
                    }
                }
            }
        }

        for (int i = 0; i < (int)current.size(); ++i) {
            if (!current[i].combined)
                primeImplicants.push_back(current[i]);
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
