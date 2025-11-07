# DD_Project

## How the Program Works
1. **User Input:**  
   The user specifies the number of variables and minterms. Each minterm represents a binary combination where the function output is 1.
2. **Simplification Steps:**  
   - Minterms are grouped according to the number of 1s in their binary representation.  
   - Adjacent groups are compared to find terms that differ by one bit.  
   - Combined terms form **prime implicants**.  
   - Redundant implicants are removed to form the minimal Boolean expression.
3. **Output:**  
   The program displays the simplified Boolean expression and shows intermediate grouping and combination steps for clarity.

---

### Roles Within the Code
- **Simplifier Class:** Responsible for core logic and data processing.  
- **Main Function:** Connects the user interface with the Simplifier’s functionality.  
- **Header File:** Ensures modularity, reusability, and a clean structure for future extensions.

---

## Example of Execution

**Input:**
```
Enter number of variables: 3
Enter number of minterms: 4
Enter the minterms (space-separated): 1 3 5 7
```

**Output:**
```
Simplifier Details:
Variables: 3
Minterms: 1 3 5 7
Prime Implicants (patterns): 0-1 1-1 11-

 Simplified Boolean Expression: B + AC
```

**Explanation:**  
The minterms represent all the input combinations for which the function is true.  
The algorithm groups, compares, and minimizes them to produce a shorter equivalent Boolean expression.

---

## 🧾 Conclusion
This project successfully demonstrates the implementation of Boolean expression simplification using the Quine–McCluskey algorithm. It showcases structured C++ programming through file separation, modular class design, and algorithmic logic handling. The program provides a strong foundation for advanced digital logic systems and can be extended for educational or industrial use.


