#include "Simplifier.h"
#include <iostream>
#include <algorithm>
#include <bitset>
#include <map>
#include <set>
#include <vector>
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

std::string Simplifier::simplify() {
    if (mins.empty()) return "0";

    // Combine minterms and don't cares for prime implicant generation
    vector<int> allTerms = mins;
    allTerms.insert(allTerms.end(), dontCares.begin(), dontCares.end());

    // Remove duplicates and sort
 set<int> uniqueTerms(allTerms.begin(), allTerms.end());
   vector<int> sortedTerms(uniqueTerms.begin(), uniqueTerms.end());

    // Step 1: Group by number of ones
    // CHANGED: Using set<string> for patterns instead of vector<pair<int,string>>
    map<int, std::set<std::string>> groups;
    for (int term : sortedTerms) {
        int ones = countOnes(term);
        groups[ones].insert(toBinary(term));
    }

    // Step 2: Find all prime implicants using PATTERNS, not term numbers
    set<std::string> primeImplicants;
    map<int, std::set<std::string>> currentGroups = groups;

    while (true) {
        map<int, std::set<std::string>> nextGroups;
        set<std::string> combined; // Track which patterns were combined
        bool foundCombination = false;

        // CHANGED: Proper pattern combination logic
        for (int ones = 0; ones < vars; ones++) {
            if (currentGroups[ones].empty() || currentGroups[ones + 1].empty())
                continue;

            for (const std::string& pattern1 : currentGroups[ones]) {
                for (const std::string& pattern2 : currentGroups[ones + 1]) {
                    // Check if patterns can combine
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
                        // Create combined pattern
                        std::string newPattern = pattern1;
                        newPattern[diffPos] = '-';

                        nextGroups[ones].insert(newPattern);
                        combined.insert(pattern1);
                        combined.insert(pattern2);
                        foundCombination = true;
                    }
                }
            }
        }

        // Add uncombined patterns to prime implicants
        for (const auto& [ones, patterns] : currentGroups) {
            for (const std::string& pattern : patterns) {
                if (combined.find(pattern) == combined.end()) {
                    primeImplicants.insert(pattern);
                }
            }
        }

        if (!foundCombination) break;

        currentGroups = nextGroups;
    }

    // Step 3: Build coverage table (ONLY for minterms, not don't cares)
    map<int, std::set<std::string>> coverage;
    map<std::string, std::set<int>> piCoverage;

    for (const std::string& pi : primeImplicants) {
        for (int m : mins) {
            bool covers = true;
            std::string mintermBin = toBinary(m);
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

    // Step 4: Find essential prime implicants
    // CHANGED: Fixed to avoid re-adding essentials
    set<std::string> essentialPrimes;
    set<int> coveredMinterms;

    for (const auto& [minterm, coveringPIs] : coverage) {
        if (coveringPIs.size() == 1) {
            std::string essentialPI = *coveringPIs.begin();
            if (essentialPrimes.insert(essentialPI).second) {
                for (int m : piCoverage[essentialPI]) {
                    coveredMinterms.insert(m);
                }
            }
        }
    }

    // Step 5: Cover remaining minterms
    set<int> remainingMinterms;
    for (int m : mins) {
        if (coveredMinterms.find(m) == coveredMinterms.end()) {
            remainingMinterms.insert(m);
        }
    }

    set<std::string> selectedCover = essentialPrimes;

    while (!remainingMinterms.empty()) {
        std::string bestPI;
        int maxCover = 0;

        for (const auto& [pi, covered] : piCoverage) {
            if (selectedCover.count(pi)) continue;

            int coverCount = 0;
            for (int m : remainingMinterms) {
                if (covered.count(m)) coverCount++;
            }

            if (coverCount > maxCover) {
                maxCover = coverCount;
                bestPI = pi;
            }
        }

        if (bestPI.empty()) break;

        selectedCover.insert(bestPI);
        for (int m : piCoverage[bestPI]) {
            remainingMinterms.erase(m);
        }
    }

    // ---- Printing Section ----
    cout << "\nPrime Implicants:\n";
    for (const std::string& pi : primeImplicants) {
        cout << "  " << pi << "  →  " << termToExpression(pi);
        cout << "  covers: {";
        bool first = true;
        for (int m : piCoverage[pi]) {
            if (!first) cout << ", ";
            cout << m;
            first = false;
        }
        cout << "}\n";
    }

    cout << "\nEssential Prime Implicants:\n";
    if (essentialPrimes.empty()) {
        cout << "  (None)\n";
    } else {
        for (const std::string& epi : essentialPrimes) {
            cout << "  " << epi << "  →  " << termToExpression(epi) << "\n";
        }
    }

    cout << "\nFinal Cover:\n";
    for (const std::string& pi : selectedCover) {
        cout << "  " << pi << "  →  " << termToExpression(pi) << "\n";
    }

    if (!remainingMinterms.empty()) {
        cout << "\nWARNING: Uncovered minterms: ";
        for (int m : remainingMinterms) {
            cout << m << " ";
        }
        cout << "\n";
    }

    // ---- Build Final Simplified Expression ----
    if (selectedCover.empty()) return "0";

    vector<std::string> terms;
    for (const std::string& pi : selectedCover) {
        terms.push_back(termToExpression(pi));
    }

    sort(terms.begin(), terms.end());

    string result;
    for (size_t i = 0; i < terms.size(); i++) {
        if (i > 0) result += " + ";
        result += terms[i];
    }

    return result;
}

