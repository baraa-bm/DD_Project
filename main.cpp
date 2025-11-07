#include "Simplifier.h"
#include <iostream>
#include <vector>

int main() {
    // Example: F(A,B,C) = 1 for minterms 1,3,5,7
    std::vector<int> mins = {1, 3, 5, 7};
    int vars = 3;

    Simplifier simp(mins, vars);
    std::string simplified = simp.simplify();

    std::cout << "Simplified Boolean Expression: " << simplified << "\n";
    return 0;
}
