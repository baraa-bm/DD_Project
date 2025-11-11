#include "Simplifier.h"
#include <iostream>
#include <algorithm>
#include <bitset>
#include <map>
#include <set>

Simplifier::Simplifier(const std::vector<int>& mins, int vars)
    : mins(mins), vars(vars) {}

int Simplifier::countOnes(int n) const {
    int count = 0;
    while (n) {
        count += n & 1;
        n >>= 1;
    }
    return count;
}

bool Simplifier::differByOneBit(int a, int b, int& diffPos) const {
    int diff = a ^ b;
    if (diff && !(diff & (diff - 1))) {
        diffPos = 0;
        while ((diff >>= 1)) diffPos++;
        return true;
    }
    return false;
}

std::string Simplifier::toBinary(int n) const {
    std::string s = std::bitset<16>(n).to_string();
    return s.substr(16 - vars);
}

std::string Simplifier::getTermPattern(int a, int b) const {
    std::string A = toBinary(a);
    std::string B = toBinary(b);
    std::string res;
    for (int i = 0; i < vars; ++i) {
        if (A[i] == B[i])
            res += A[i];
        else
            res += '-';
    }
    return res;
}

std::string Simplifier::termToExpression(const std::string& pattern) const {
    std::string expr;
    for (int i = 0; i < vars; ++i) {
        if (pattern[i] == '-') continue;
        char var = 'A' + i;
        if (pattern[i] == '0')
            expr += var, expr += '\'';
        else
            expr += var;
    }
    return expr.empty() ? "1" : expr;
}

std::string Simplifier::simplify() {
    std::map<int, std::set<int>> groups;
    for (int m : mins) {
        groups[countOnes(m)].insert(m);
    }

    std::set<std::string> primePatterns;
    std::set<int> covered;

    bool done = false;
    std::map<int, std::set<int>> current = groups;

    while (!done) {
        done = true;
        std::map<int, std::set<int>> next;
        std::set<int> used;

        for (auto it = current.begin(); it != current.end(); ++it) {
            auto nextIt = std::next(it);
            if (nextIt == current.end()) break;

            for (int a : it->second) {
                for (int b : nextIt->second) {
                    int diffPos;
                    if (differByOneBit(a, b, diffPos)) {
                        done = false;
                        used.insert(a);
                        used.insert(b);
                        next[it->first].insert(a & b);
                        primePatterns.insert(getTermPattern(a, b));
                    }
                }
            }
        }

        // Add unused terms (prime implicants)
        for (auto& [ones, terms] : current) {
            for (int t : terms) {
                if (!used.count(t)) {
                    primePatterns.insert(toBinary(t));
                }
            }
        }

        current = next;
    }

    // Convert patterns to expressions
    std::set<std::string> expressions;
    for (const auto& p : primePatterns)
        expressions.insert(termToExpression(p));

    std::string result;
    for (auto it = expressions.begin(); it != expressions.end(); ++it) {
        if (it != expressions.begin()) result += " + ";
        result += *it;
    }

    return result.empty() ? "0" : result;
}