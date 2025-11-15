#include "Simplifier.h"
#include <iostream>
#include <algorithm>
#include <bitset>
#include <map>
#include <set>
#include <vector>
#include <functional>
using namespace std;

Simplifier::Simplifier(const std::vector<int>& mins, const std::vector<int>& dontCares, int vars)
    : mins(mins), dontCares(dontCares), vars(vars) {}

int Simplifier::countOnes(int n) const {
    int count = 0;
    while (n) {
        count += n & 1;
        n >>= 1;
    }
    return count;
}

bool Simplifier::differByOneBit(int a, int b, int& diffBit) const {
    int diff = a ^ b;
    if (diff && (diff & (diff - 1)) == 0) {
        diffBit = 0;
        while (diff > 1) {
            diff >>= 1;
            diffBit++;
        }
        return true;
    }
    return false;
}

std::string Simplifier::toBinary(int n) const {
    if (vars == 0) return "";
    std::string result;
    for (int i = vars - 1; i >= 0; i--) {
        result += ((n >> i) & 1) ? '1' : '0';
    }
    return result;
}

std::string Simplifier::getTermPattern(int a, int b, int diffBit) const {
    std::string patternA = toBinary(a);
    std::string patternB = toBinary(b);
    std::string pattern = patternA;

    for (int i = 0; i < vars; i++) {
        if (patternA[i] != patternB[i])
            pattern[i] = '-';
    }
    return pattern;
}


std::string Simplifier::termToExpression(const std::string& pattern) const {
    std::string expr;
    for (int i = 0; i < vars; i++) {
        if (pattern[i] == '-') continue;
        char var = 'A' + i;
        if (pattern[i] == '0') {
            expr += var;
            expr += '\'';
        } else {
            expr += var;
        }
    }
    return expr.empty() ? "1" : expr;
}



string Simplifier::simplify() {
    if (mins.empty()) return "0";

    // Combine minterms and don't cares for prime implicant generation
    vector<int> allTerms = mins;
    allTerms.insert(allTerms.end(), dontCares.begin(), dontCares.end());

    // Remove duplicates and sort
    set<int> uniqueTerms(allTerms.begin(), allTerms.end());
    vector<int> sortedTerms(uniqueTerms.begin(), uniqueTerms.end());




    // grouping by number of ones
 
    map<int, set<string>> groups;
    for (int term : sortedTerms) {
        int ones = countOnes(term);
        groups[ones].insert(toBinary(term));
    }

    //finding all prime implicants using PATTERNS, not term numbers
    set<string> primeImplicants;
    map<int, set<string>> currentGroups = groups;

    while (true) {
        map<int, set<string>> nextGroups;


        set<string> combined; 
        bool foundCombination = false;

        // the pattern combination logic
        for (int ones = 0; ones < vars; ones++) {
            if (currentGroups[ones].empty() || currentGroups[ones + 1].empty())
                continue;

            for (const string& pattern1 : currentGroups[ones]) {
                for (const string& pattern2 : currentGroups[ones + 1]) {
                    // checking if patterns can combine
                    int diffCount = 0;
                    int diffPos = -1;
                    bool canCombine = true;

                    for (int i = 0; i < vars; i++) {
                        if (pattern1[i] == '-' && pattern2[i] == '-') {
                            continue; // both are don't cares
                        }
                        if (pattern1[i] == '-' || pattern2[i] == '-') {
                            canCombine = false; // can't combine if only one has don't care
                            break;
                        }
                        if (pattern1[i] != pattern2[i]) {
                            diffCount++;
                            diffPos = i;
                            if (diffCount > 1) {
                                canCombine = false;
                                break;
                            }
                        }
                    }

                    if (canCombine && diffCount == 1) {
                        // creating a combined pattern
                        string newPattern = pattern1;
                        newPattern[diffPos] = '-';

                        nextGroups[ones].insert(newPattern);
                        combined.insert(pattern1);
                        combined.insert(pattern2);
                        foundCombination = true;
                    }
                }
            }
        }

        //adding a uncombined patterns to prime implicants

        for (const auto& [ones, patterns] : currentGroups) {
            for (const string& pattern : patterns) {
                if (combined.find(pattern) == combined.end()) {
                    primeImplicants.insert(pattern);
                }
            }
        }

        if (!foundCombination) break;

        currentGroups = nextGroups;
    }

    //building coverage table (ONLY for minterms, not don't cares)


    map<int, set<string>> coverage;
    map<string, set<int>> piCoverage;

    for (const string& pi : primeImplicants) {
        for (int m : mins) {
            bool covers = true;
            string mintermBin = toBinary(m);
            for (int i = 0; i < vars; i++) {
                if (pi[i] == '-') continue;
                if (pi[i] != mintermBin[i]) {
                    covers = false;
                    break;
                }
            }
            if (covers) {
                coverage[m].insert(pi);
                piCoverage[pi].insert(m);
            }
        }
    }

    //Finding essential prime implicants
    // CHANGED: Fixed to avoid re-adding essentials
    set<string> essentialPrimes;
    set<int> coveredMinterms;

    for (const auto& [minterm, coveringPIs] : coverage) {
        if (coveringPIs.size() == 1) {
            string essentialPI = *coveringPIs.begin();
            if (essentialPrimes.insert(essentialPI).second) {
                for (int m : piCoverage[essentialPI]) {
                    coveredMinterms.insert(m);
                }
            }
        }
    }

    //finding all minimal covers
    set<int> remainingMinterms;
    for (int m : mins) {
        if (coveredMinterms.find(m) == coveredMinterms.end()) {
            remainingMinterms.insert(m);
        }
    }

    // Get non-essential PIs that can cover remaining minterms
    vector<string> nonEssentialPIs;
    for (const auto& [pi, covered] : piCoverage) {
        if (essentialPrimes.count(pi)) continue;
        bool coversRemaining = false;
        for (int m : remainingMinterms) {
            if (covered.count(m)) {
                coversRemaining = true;
                break;
            }
        }
        if (coversRemaining) {
            nonEssentialPIs.push_back(pi);
        }
    }

    // finding all minimal covers using backtracking

    vector<set<string>> allMinimalCovers;

    int minCoverSize = INT_MAX;

    function <void(size_t, set<string>, set<int>)> findCovers;
    findCovers = [&](size_t idx, set<string> currentCover, set<int> uncovered) {
        // if current cover is already larger than best found, skip
        if (currentCover.size() > minCoverSize) return;

        // if all minterms covered
        if (uncovered.empty()) {
            if (currentCover.size() < minCoverSize) {
                minCoverSize = currentCover.size();
                allMinimalCovers.clear();
                allMinimalCovers.push_back(currentCover);
            } else if (currentCover.size() == minCoverSize) {
                allMinimalCovers.push_back(currentCover);
            }
            return;
        }

        // if we've considered all PIs
        if (idx >= nonEssentialPIs.size()) return;

        // try including current PI
        const string& pi = nonEssentialPIs[idx];
        set<int> newUncovered = uncovered;
        for (int m : piCoverage.at(pi)) {
            newUncovered.erase(m);
        }
        set<string> newCover = currentCover;
        newCover.insert(pi);
        findCovers(idx + 1, newCover, newUncovered);

        // try excluding current PI
        findCovers(idx + 1, currentCover, uncovered);
    };

    if (remainingMinterms.empty()) {
        // no additional PIs needed
        allMinimalCovers.push_back(set<string>());
    } else {
        findCovers(0, set<string>(), remainingMinterms);
    }

    // build all final covers (essential + each minimal cover)
    vector<set<string>> allFinalCovers;
    for (const auto& minCover : allMinimalCovers) {

        set<string> finalCover = essentialPrimes;
        finalCover.insert(minCover.begin(), minCover.end());
        allFinalCovers.push_back(finalCover);
    }

    // use first cover for compatibility

    set<string> selectedCover = allFinalCovers.empty() ? essentialPrimes : allFinalCovers[0];
    set<int> finalRemaining = remainingMinterms;
    for (const auto& pi : selectedCover) {
        if (piCoverage.count(pi)) {
            for (int m : piCoverage.at(pi)) {
                finalRemaining.erase(m);
            }
        }
    }

    // printing the output
    cout << "\nEssential Prime Implicants:\n";
    if (essentialPrimes.empty()) {
        cout << "  (None)\n";
    } else {
        for (const string& epi : essentialPrimes) {
            cout << "  " << epi << "  " << termToExpression(epi) << endl;
        }
    }

    cout << "All Possible Minimal Covers:\n";
    for (size_t i = 0; i < allFinalCovers.size(); i++) {
        cout << "Cover " << (i + 1) << ":\n";
        for (const string& pi : allFinalCovers[i]) {
            cout << "  " << pi << "  " << termToExpression(pi) << endl;
        }
    }


    // building simplified expressions
    if (allFinalCovers.empty()) return "0";

    vector<string> allExpressions;
    for (const auto& cover : allFinalCovers) {
        vector<string> terms;
        for (const string& pi : cover) {
            terms.push_back(termToExpression(pi));
        }
        sort(terms.begin(), terms.end());

        string expr;
        for (size_t i = 0; i < terms.size(); i++) {
            if (i > 0) expr += " + ";
            expr += terms[i];
        }
        allExpressions.push_back(expr);
    }

    // printing all possible expressions
    cout << "All Possible Minimal Boolean Expressions:"<<endl;
    for (size_t i = 0; i < allExpressions.size(); i++) {
        cout << "Expression " << (i + 1) << ": " << allExpressions[i] << endl;
    }

    return allExpressions[0]; // Return first one for compatibility
}

