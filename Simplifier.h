#ifndef SIMPLIFIER_H
#define SIMPLIFIER_H

#include <string>
#include <vector>

class Simplifier {
public:
    Simplifier(const std::vector<int>& mins, const std::vector<int>& dontCares, int vars);
    std::string simplify();

private:
    std::vector<int> mins;
    std::vector<int> dontCares;
    int vars;

    int countOnes(int n) const;
    bool differByOneBit(int a, int b, int& diffBit) const;
    std::string toBinary(int n) const;
    std::string getTermPattern(int a, int b, int diffBit) const;
    std::string termToExpression(const std::string& pattern) const;
};

#endif