#ifndef SIMPLIFIER_H
#define SIMPLIFIER_H

#include <vector>
#include <string>

class Simplifier {
private:
    std::vector<int> mins; // minterms
    int vars;              // number of variables

    // Helper functions
    int countOnes(int n) const;
    bool differByOneBit(int a, int b, int& diffPos) const;
    std::string toBinary(int n) const;
    std::string getTermPattern(int a, int b) const;
    std::string termToExpression(const std::string& pattern) const;

public:
    Simplifier(const std::vector<int>& mins, int vars);
    std::string simplify();
};

#endif
