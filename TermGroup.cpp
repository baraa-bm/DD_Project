//
// Created by janna on 05/11/2025.
//

#include "TermGroup.h"

//constructor
TermGroup::TermGroup(int vars, bool maxtermMode) : numVariables(vars) {}

//count number of 1s in binary string
int TermGroup::countOnes(const string& binary) {
    int count = 0;
    for (char c : binary)
    if (c == '1')
    count ++;
    return count;
}


//check if two terms differ by exactly one bit, returns true if the two strings can be combined
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

//combining the two terms that differ by one bit, where the differing bit is replaced by '-'
string TermGroup::combine(const string& a, const string& b) {
    string result = a;
    for (int i = 0; i < a.size(); i++) {
        if (a[i] != b[i])
        result[i] = '-';
    }
    return result;
}

//generating prime implicants
//combines minterms and dont cares into more general terms
//groups terms by num of 1s and compares adjacent groups
//marks combined terms and keeps track of those that cant be combined
//returns the final list of unique PIs
vector<Term> TermGroup::generatePrimeImplicants(const vector<string>& termBinaries, const vector<int>& termNumbers, const vector<string>& dontCareBinaries, const vector<int>& dontCareNumbers) {
    vector<string> totalBinaries = termBinaries;
    vector<int> totalNumbers = termNumbers;
    for (int i = 0; i < (int)dontCareBinaries.size(); ++i) {
        totalBinaries.push_back(dontCareBinaries[i]);
        totalNumbers.push_back(dontCareNumbers[i]);
    }

    vector<Term> current;
    for (int i = 0; i < (int)totalBinaries.size(); ++i)
        current.push_back(Term(totalBinaries[i], {totalNumbers[i]}));

    bool canCombineFurther = true;

    while (canCombineFurther) {
        canCombineFurther = false;
        vector<Term> nextLevel;
        vector<vector<Term>> groups(numVariables + 1);

        for (const auto& term : current)
            groups[countOnes(term.binary)].push_back(term);

        for (int i = 0; i < numVariables; ++i) {
            for (const auto& a : groups[i]) {
                for (const auto& b : groups[i + 1]) {
                    if (canCombine(a.binary, b.binary)) {
                        string combinedBin = combine(a.binary, b.binary);
                        vector<int> merged = a.coveredMinterms;
                        merged.insert(merged.end(), b.coveredMinterms.begin(), b.coveredMinterms.end());
                        nextLevel.push_back(Term(combinedBin, merged));
                        const_cast<Term&>(a).combined = true;
                        const_cast<Term&>(b).combined = true;
                        canCombineFurther = true;
                    }
                }
            }
        }

        //keep uncombined terms for later
        for (const auto& term : current)
            if (!term.combined) primeImplicants.push_back(term);

        current = nextLevel;
    }

    //add any leftover terms as prime implicants
    for (const auto& term : current)
        primeImplicants.push_back(term);

    //remove duplicates (same bianry pattern)
    sort(primeImplicants.begin(), primeImplicants.end(),
         [](const Term& a, const Term& b) { return a.binary < b.binary; });
    primeImplicants.erase(unique(primeImplicants.begin(), primeImplicants.end(),
         [](const Term& a, const Term& b) { return a.binary == b.binary; }), primeImplicants.end());

    //remove dominated implicants (fully covered by more general ones)
    auto covers = [](const std::string& general, const std::string& specific) {
        if (general.size() != specific.size()) return false;
        bool different = false;
        for (size_t i = 0; i < general.size(); ++i) {
            if (general[i] == '-') { continue; }
            if (general[i] != specific[i]) return false;
            if (general[i] != '-') different = true; // just to avoid identical strings counting as cover
        }
        return general != specific; // ensure strict containment, not equality
    };

    vector<Term> filtered;
    for (size_t i = 0; i < primeImplicants.size(); ++i) {
        bool dominated = false;
        for (size_t j = 0; j < primeImplicants.size() && !dominated; ++j) {
            if (i == j) continue;
            if (covers(primeImplicants[j].binary, primeImplicants[i].binary)) {
                dominated = true; // i is fully covered by a more general j
            }
        }
        if (!dominated) filtered.push_back(primeImplicants[i]);
    }
    primeImplicants.swap(filtered);

    //removing implicants that cover only dont cares
    {
        std::vector<Term> finalFiltered;
        for (const auto& t : primeImplicants) {
            bool keep = false;
            // keep if the implicant covers at least one true minterm (not just don't cares)
            for (int m : t.coveredMinterms) {
                // If m is a true minterm
                if (std::find(termNumbers.begin(), termNumbers.end(), m) != termNumbers.end()) {
                    keep = true;
                    break;
                }
            }
            if (keep)
                finalFiltered.push_back(t);
        }
        primeImplicants.swap(finalFiltered);
    }

    return primeImplicants;
}


//printing prime implicants (uncombined) and the minterms they cover

void TermGroup::printPrimeImplicants() const {
    cout << "Prime Implicants:" << endl;

    // Filter: print only uncombined (true prime) implicants
    for (const auto& pi : primeImplicants) {
        if (pi.combined) continue; // skip those merged into others

        cout << pi.binary << " covers { ";
        for (size_t i = 0; i < pi.coveredMinterms.size(); ++i) {
            cout << pi.coveredMinterms[i];
            if (i + 1 < pi.coveredMinterms.size()) cout << ", ";
        }
        cout << " }" << endl;
    }
}

