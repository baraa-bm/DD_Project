#include "Simplifier.h"
#include <iostream>
#include <algorithm>
#include <bitset>
#include <map>
#include <set>
#include <vector>

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
    std::string pattern = toBinary(a);
    pattern[diffBit] = '-';
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
    std::vector<int> allTerms = mins;
    allTerms.insert(allTerms.end(), dontCares.begin(), dontCares.end());
    
    // Remove duplicates and sort
    std::set<int> uniqueTerms(allTerms.begin(), allTerms.end());
    std::vector<int> sortedTerms(uniqueTerms.begin(), uniqueTerms.end());
    
    // Step 1: Group by number of ones
    std::vector<std::vector<int>> groups(vars + 1);
    for (int term : sortedTerms) {
        int ones = countOnes(term);
        if (ones <= vars) {
            groups[ones].push_back(term);
        }
    }

    // Step 2: Find all prime implicants
    std::set<std::string> primeImplicants;
    std::vector<std::vector<std::pair<int, std::string>>> currentGroups(vars + 1);
    
    // Initialize with all terms (minterms + don't cares)
    for (int i = 0; i <= vars; i++) {
        for (int term : groups[i]) {
            currentGroups[i].push_back({term, toBinary(term)});
        }
    }

    bool changed = true;
    std::set<std::string> allCombined;
    
    while (changed) {
        changed = false;
        std::vector<std::vector<std::pair<int, std::string>>> nextGroups(vars + 1);
        std::set<std::string> combinedThisRound;

        for (int i = 0; i < vars; i++) {
            for (const auto& term1 : currentGroups[i]) {
                for (const auto& term2 : currentGroups[i + 1]) {
                    int diffBit;
                    if (differByOneBit(term1.first, term2.first, diffBit)) {
                        std::string newPattern = getTermPattern(term1.first, term2.first, diffBit);
                        combinedThisRound.insert(term1.second);
                        combinedThisRound.insert(term2.second);
                        
                        // Add to appropriate group based on number of ones
                        int ones = countOnes(term1.first);
                        if (ones <= vars) {
                            nextGroups[ones].push_back({term1.first, newPattern});
                        }
                        changed = true;
                    }
                }
            }
        }

        // Add uncombined terms as prime implicants
        for (int i = 0; i <= vars; i++) {
            for (const auto& term : currentGroups[i]) {
                if (!combinedThisRound.count(term.second)) {
                    primeImplicants.insert(term.second);
                }
            }
        }

        allCombined.insert(combinedThisRound.begin(), combinedThisRound.end());
        currentGroups = nextGroups;
    }

    // Add any remaining terms
    for (int i = 0; i <= vars; i++) {
        for (const auto& term : currentGroups[i]) {
            primeImplicants.insert(term.second);
        }
    }

    // Step 3: Build coverage table (ONLY for minterms, not don't cares)
    std::map<int, std::set<std::string>> coverage;
    std::map<std::string, std::set<int>> piCoverage;
    
    for (const std::string& pi : primeImplicants) {
        for (int m : mins) { // Only check minterms, not don't cares
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
    std::set<std::string> essentialPrimes;
    std::set<int> coveredMinterms;
    
    for (const auto& [minterm, coveringPIs] : coverage) {
        if (coveringPIs.size() == 1) {
            std::string essentialPI = *coveringPIs.begin();
            essentialPrimes.insert(essentialPI);
            for (int coveredM : piCoverage[essentialPI]) {
                coveredMinterms.insert(coveredM);
            }
        }
    }

    // Step 5: Cover remaining minterms
    std::set<int> remainingMinterms;
    for (int m : mins) {
        if (!coveredMinterms.count(m)) {
            remainingMinterms.insert(m);
        }
    }

    while (!remainingMinterms.empty()) {
        // Find PI that covers most remaining minterms
        std::string bestPI;
        int maxCover = 0;
        
        for (const auto& [pi, covered] : piCoverage) {
            if (essentialPrimes.count(pi)) continue;
            
            int coverCount = 0;
            for (int m : remainingMinterms) {
                if (covered.count(m)) coverCount++;
            }
            
            if (coverCount > maxCover) {
                maxCover = coverCount;
                bestPI = pi;
            }
        }
        
        if (bestPI.empty() && !remainingMinterms.empty()) {
            // Fallback: pick first PI that covers any remaining minterm
            for (const auto& [pi, covered] : piCoverage) {
                if (essentialPrimes.count(pi)) continue;
                for (int m : remainingMinterms) {
                    if (covered.count(m)) {
                        bestPI = pi;
                        break;
                    }
                }
                if (!bestPI.empty()) break;
            }
        }
        
        if (bestPI.empty()) break;
        
        essentialPrimes.insert(bestPI);
        for (int m : piCoverage[bestPI]) {
            coveredMinterms.insert(m);
            remainingMinterms.erase(m);
        }
    }

    // Convert to expression
    if (essentialPrimes.empty()) return "0";
    
    std::vector<std::string> terms;
    for (const std::string& pi : essentialPrimes) {
        terms.push_back(termToExpression(pi));
    }
    
    // Sort terms for consistent output
    std::sort(terms.begin(), terms.end());
    
    std::string result;
    for (size_t i = 0; i < terms.size(); i++) {
        if (i > 0) result += " + ";
        result += terms[i];
    }
    
    return result;
}