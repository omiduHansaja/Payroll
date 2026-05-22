//G21328038 - Omidu Hansaja
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>

using namespace std;

// Constants defined for the calculations
const int maxEmployees = 100;
const double taxRate = 0.20;
const double taxAllowance = 12570.0;
const int months = 12;

// Constants defining file names 
const string employeeFile = "employees.txt";
const string janPayFile = "Jan25.txt";
const string febPayFile = "Feb25.txt";
const string marPayFile = "Mar25.txt";
const string aprPayFile = "Apr25.txt";
const string errorFileName = "errors.txt";

// Structure to hold employee data
struct employee {
    string id;
    string name;
    double rate;
};

// Structure to hold each employee's payroll information
struct payroll {
    string id;
    double monthlyPayment[4] = {0, 0, 0, 0}; // Monthly payments for four months
    double totalPayment = 0;
};

// Function to display errors to "errors.txt" file
void displayError(const string& payFile, const string& errorMessage) {
    ofstream errorFile(errorFileName, ios::app);
    if (errorFile) {
        errorFile << "File: " << payFile << " - " << errorMessage << endl; // Log error message 
    } else {
        cout << "ERROR: Unable to open " << errorFileName << endl; // Error when opening the file
    }
}

// Function to load employee data from a payfile
int loadEmployeeData(employee employees[]) {
    ifstream employeeDataFile(employeeFile);
    
    if (!employeeDataFile) {
        cout << "ERROR: Unable to open file " << employeeFile << endl;
        return 0;
    }
    
    // Initialize employees count to 0 at the start       
    int employeeCount = 0;
    
    // To read data of employees from the file
    while (employeeCount < maxEmployees && employeeDataFile >> employees[employeeCount].id 
           >> employees[employeeCount].name >> employees[employeeCount].rate) {
        employeeCount++;
    }
    
    employeeDataFile.close();
    return employeeCount;
}

// Function to create output file name based on the input file
string createOutputFileName(const string& payFileName) {
    string outputFileName = "";
    for (char c : payFileName) {
        if (c == '.') {
            break;
        }
        outputFileName += c;
    }
    outputFileName += "_output.txt";
    return outputFileName;
}

// Function to convert strings to lowercase 
string toLowerCase(string str) {
    for (char &c : str) {
        if (c >= 'A' && c <= 'Z') {
            c = c + 32; // Convert uppercase to lowercase using ASCII values
        }
    }
    return str;
}

// Function to process payroll data from a payfile
void processPayroll(const string& payFileName, employee employees[], int employeeCount, payroll payrolls[], int currentMonth) {
    cout << "Processing payroll for file: " << payFileName << endl;
    ifstream payFile(payFileName); // Open payroll file
    
    if (!payFile) {
        cout << "ERROR: File named " << payFileName << " not found" << endl;
        displayError(payFileName, "File not found"); // Log error
        return;
    }

    ofstream outputFile(createOutputFileName(payFileName)); // To create the output file
    if (!outputFile) {
        cout << "ERROR: Unable to create output file for " << payFileName << endl; // To display the error when creating the output file
        return;
    }

    // Display headers of the columns for payroll report
    cout << "Generating payroll report for the file " << payFileName << endl;
    cout << " " << endl;
    cout << left << setw(12) << "Employee ID" << setw(15) << "Employee Name" 
         << right << setw(14) << "Hours" << setw(14) << "Base Payment" 
         << setw(15) << "Tax Amount" << setw(14) << "Final Payment" << endl;

    string id; // To hold employee ID's
    double hours; // To hold hours worked

    // Process through each employee payroll data
    while (payFile >> id) {
        if (!(payFile >> hours)) {
            displayError(payFileName, " Payroll entry for " + id + " is incomplete. ");
            payFile.clear();
            string restLines;
            getline(payFile, restLines); // Read the rest of the line
            continue;
        } 
        
        bool isFound = false; // To keep track if employee is found

        // To find the employee from an array
        for (int i = 0; i < employeeCount; i++) {
            if (toLowerCase(id) == toLowerCase(employees[i].id)) {
                // Calculate payroll details
                double basePay = employees[i].rate * hours; 
                double annualIncome = basePay * months;
                double taxableIncome = annualIncome - taxAllowance;
                if (taxableIncome < 0) taxableIncome = 0; // Ensure taxable income is not negative
                double annualTax = taxableIncome * taxRate;
                double monthlyTax = annualTax / months;
                double finalPay = basePay - monthlyTax;

                // Display the payroll details under the column headers
                cout << left << setw(12) << employees[i].id << setw(15) 
                     << employees[i].name << right << fixed << setprecision(2) << setw(14) 
                     << hours << setw(12) << basePay << setw(15) << monthlyTax 
                     << setw(12) << finalPay << endl;

                // Write payroll details to output file
                outputFile << employees[i].id << setw(12) << fixed << setprecision(2) 
                           << basePay << setw(15) << finalPay << endl;

                // Store payroll details for later use
                payrolls[i].id = employees[i].id;
                payrolls[i].monthlyPayment[currentMonth] = finalPay;
                payrolls[i].totalPayment += finalPay;

                isFound = true; // Employee found
                break; // Exit the loop
            }
        }

        if (!isFound) {
            displayError(payFileName, id + " is not a valid employee ID");
        }
    }
    payFile.close();
    outputFile.close();
}

// Function to display payroll details for a specific month and employee
void displayEmployeePayroll(const string& payFileName, employee employees[], int employeeCount, string id) {
    ifstream payFile(payFileName);
    if (!payFile) {
        cout << "ERROR: File named " << payFileName << " not found" << endl;
        displayError(payFileName, "File not found");
        return;
    }

    string lowerId = toLowerCase(id); // Convert ID to lowercase for comparisons
    string empId;
    double hours;
    bool isFound = false; // To keep track if employee is found

    // Processing the data to find the relevant employee
    while (payFile >> empId) {
        if (toLowerCase(empId) == lowerId) { // Check if ID matches
            // Check if hours are provided
            if (!(payFile >> hours)) {
                displayError(payFileName, " Payroll entry for " + empId + " is incomplete. ");
                payFile.clear(); // Clear errors
                string restLines;
                getline(payFile, restLines);
                continue;
            }
            
            for (int i = 0; i < employeeCount; i++) {
                if (toLowerCase(empId) == toLowerCase(employees[i].id)) {
                    // Calculate the payroll details
                    double basePay = employees[i].rate * hours;
                    double annualIncome = basePay * months;
                    double taxableIncome = annualIncome - taxAllowance;
                    if (taxableIncome < 0) taxableIncome = 0;
                    double annualTax = taxableIncome * taxRate;
                    double monthlyTax = annualTax / months;
                    double finalPay = basePay - monthlyTax;
                    
                    // Display payroll details
                    cout << "Payroll details for " << employees[i].name 
                         << " (" << employees[i].id << ") for " << payFileName << ":" << endl;
                    
                    cout << "Hours Worked: " << hours << endl;
                    cout << "Base Payment: " << fixed << setprecision(2) << basePay << endl;
                    cout << "Monthly Tax: " << fixed << setprecision(2) << monthlyTax << endl;
                    cout << "Final Payment: " << fixed << setprecision(2) << finalPay << endl;

                    isFound = true;
                    break;
                }
            }
            break;
        }
    }

    if (!isFound) {
        displayError(payFileName, id + " is not a valid employee ID");
    }

    payFile.close();
}

// Function to show all payrolls for a relevant employee
void showAllEmployeePayrolls(employee employees[], int employeeCount, const string& id, payroll payrolls[]) {
    string lowerId = toLowerCase(id);
    bool isFound = false;

    for (int i = 0; i < employeeCount; i++) {
        if (lowerId == toLowerCase(employees[i].id)) { // Check if ID matches 
            // Display payroll details for all months and total
            cout << "Payroll details for " << employees[i].name << "--" << employees[i].id << endl;
            cout << "January: " << fixed << setprecision(2) << payrolls[i].monthlyPayment[0] << endl;
            cout << "February: " << fixed << setprecision(2) << payrolls[i].monthlyPayment[1] << endl;
            cout << "March: " << fixed << setprecision(2) << payrolls[i].monthlyPayment[2] << endl;
            cout << "April: " << fixed << setprecision(2) << payrolls[i].monthlyPayment[3] << endl;

            cout << "Total: " << fixed << setprecision(2) << payrolls[i].totalPayment << endl;

            isFound = true;
            break;
        }
    }
    if (!isFound) {
        displayError(" ", "Employee ID: " + id + " not found"); // Log error if ID not found
    }
}

// Function to find employee by their ID number and display the details for relevant month
void findEmployeePayrollByID(employee employees[], int employeeCount, const string& id, payroll payrolls[]) {
    int monthChoice;
    string payFileName;

    // Making the sub menu for the user to select the month and to continue 
    cout << "=====================================================" << endl;
    cout << " " <<endl;
    cout << "Select Month to view payroll details for employee ID " << id << endl;
    cout << "1. January" << endl;
    cout << "2. February" << endl;
    cout << "3. March" << endl;
    cout << "4. April" << endl;
    cout << "5. View All Payrolls" << endl;
    cout << "6. Return to Main Menu" << endl;
    cout << "=====================================================" << endl;
    cout << "Enter your choice: ";
    cin >> monthChoice;

    // Switch case to handle user inputs
    switch (monthChoice) {
        case 1:
            payFileName = janPayFile; // Set file name to January
            displayEmployeePayroll(payFileName, employees, employeeCount, id);
            break;
        case 2:
            payFileName = febPayFile; // Set file name to February
            displayEmployeePayroll(payFileName, employees, employeeCount, id);
            break;
        case 3:
            payFileName = marPayFile; // Set file name to March
            displayEmployeePayroll(payFileName, employees, employeeCount, id);
            break;
        case 4:
            payFileName = aprPayFile; // Set file name to April
            displayEmployeePayroll(payFileName, employees, employeeCount, id);
            break;
        case 5:
            showAllEmployeePayrolls(employees, employeeCount, id, payrolls);
            break;
        case 6:
            cout << "Returning to Main Menu..." << endl;
            return;
        default:
            cout << "Invalid choice. Returning to Main Menu..." << endl;
            return;
    }
}

int main() {
    employee employees[maxEmployees]; // Array to store employee data
    payroll payrolls[maxEmployees]; // Array to store payroll data
    int employeeCount = loadEmployeeData(employees); // Load employee data
    if (employeeCount == 0) {
        cout << "No employee data found. Exiting..." << endl;
        return 1;
    }

    ofstream errorFile(errorFileName);
    if (!errorFile) {
        cout << "Error: Unable to clear " << errorFileName << "." << endl;
    }
    errorFile.close();

    int mainChoice; 
    string payFileName;  // Declare payFileName
    do {
        // Making the main menu 
        cout << "************************" << endl;
        cout << "Payroll System Main Menu" << endl;
        cout << "************************" << endl;
        cout << " " << endl;
        cout << "1. View Monthly Payrolls" << endl;
        cout << "2. Search employees by id" << endl;
        cout << "3. Exit" << endl;
        cout << " " << endl;
        cout << "************************" << endl;
        cout << "Enter your choice: ";
        cin >> mainChoice;

        // To handle the user input "1"
        if (mainChoice == 1) {
            do {
                // Making the sub menu to get the user input
                cout << "================================" << endl;
                cout << "File names > (Jan25.txt, Feb25.txt, Mar25.txt, Apr25.txt)." << endl;
                cout << " " << endl;
                cout << "Enter payroll file name or type (return) to go to the main menu: " << endl;
                cin >> payFileName;

                string toLowerPayFile = toLowerCase(payFileName);

                if (toLowerPayFile == "return") {
                    cout << "Returning to main menu..." << endl;
                    break;
                } else if (toLowerPayFile == toLowerCase(janPayFile)) {
                    processPayroll(janPayFile, employees, employeeCount, payrolls, 0);
                } else if (toLowerPayFile == toLowerCase(febPayFile)) {
                    processPayroll(febPayFile, employees, employeeCount, payrolls, 1);
                } else if (toLowerPayFile == toLowerCase(marPayFile)) {
                    processPayroll(marPayFile, employees, employeeCount, payrolls, 2);
                } else if (toLowerPayFile == toLowerCase(aprPayFile)) {
                    processPayroll(aprPayFile, employees, employeeCount, payrolls, 3);
                } else {
                    cout << "Invalid file name. Please try again!" << endl; // Handle invalid file name inputs
                }

            } while (payFileName != "return");
        } 

        // To handle the user input "2"
        else if (mainChoice == 2) {
            string id;
            cout << "Enter Employee ID to search: " << endl;
            cin >> id;
            findEmployeePayrollByID(employees, employeeCount, id, payrolls);

        // To handle the user Input "3"     
        } else if (mainChoice == 3) {
            cout << "Exiting program..." << endl;

        // To handle invalid user inputs
        } else {
            cout << "Invalid choice. Please try again." << endl;
        }
    } while (mainChoice != 3); // Continue until user chooses to exit

    return 0; 
}
