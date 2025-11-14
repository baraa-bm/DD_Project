# DD_Project

## How to Build the Application

Make sure you are inside the project folder, then compile using:

```
g++ main.cpp Simplifier.cpp TermGroup.cpp VerilogGenerator.cpp -o main.exe
```

This command generates the executable:

```
main.exe
```

---

## How to Run the Application

After building, run the program with:

```
./main.exe
```

The application will then ask for the file name containing your minterms:

```
Enter the file name you want to test:
```

Provide the name of the file (you can choose any test file across all the 10 we have available)


---

## Output

The program will display:

- Prime Implicants and Essential Prime Implicants
- Possible minimal covers 
- The simplified Boolean expression  
- **Bonus:** The generated Verilog module using Verilog primitives will be written in a file called "output.v"
