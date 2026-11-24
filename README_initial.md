# FLOAT — Flexible Ledger for Obligations and Allowance Tracking

## Description

FLOAT (Flexible Ledger for Obligations and Allowance Tracking) is a command-line personal finance and budgeting application written in C++. It tracks received income, expected (pending) payments, expenses, bills, reserved savings, financial summaries, and daily spending plans using monetary amounts formatted in Philippine pesos (PHP) from centavo values.

State modifications are applied to a proposed copy of AppData and persisted via floatapp::saveData before updating the active in-memory state.

## Features

### Income & Expected Payments

- Add received income records (source, date, amountCentavos, description).
- View recorded received income.
- Add expected payments (expectedDate, source, amountCentavos, description).
- View expected payments with RECEIVED or PENDING status.
- Receive a pending expected payment (marks it received and appends a corresponding Income record with user-supplied actual received date and amount).
- Reschedule the expectedDate of an existing pending expected payment.

### Expenses & Bills

- Add non-Bill payment expenses (category, date, amountCentavos, description).
- View all expenses.
- Search expenses with a case-sensitive substring match across date, category, and description.
- Update or delete non-Bill payment expenses (with confirmation when deleting).
- Add bills (title, dueDate, amountCentavos).
- View bills with PAID or UNPAID / RESERVED status.
- Pay an unpaid bill (marks the bill paid and appends an expense with category "Bill payment", description "Bill ID <id>", and the bill's amountCentavos on a user-supplied payment date).

### Savings & Planning

- Set or clear reserved savings (reservedSavingsCentavos, allowing 0 to clear).
- View a financial summary showing received income, paid expenses, current balance, unpaid bills reserved (all dates), savings reserved, available spending funds (with a shortfall notice if negative), and per-category expense totals.
- Generate a daily spending plan for a given YYYY-MM-DD planning date up to the earliest future unreceived expected payment, reporting overdue/due pending payments, days to cover, and an estimated daily limit (excluding unreceived expected income and reserving all unpaid bills).

## Technologies Used

- Language: C++ (uses <filesystem> and standard C++ headers: <algorithm>, <climits>, <ctime>, <iomanip>, <iostream>, <sstream>, <stdexcept>, <string>)
- Project Headers (floatapp namespace):
  - Calculations.h
  - FileStorage.h
  - InputValidation.h
  - Records.h

## Installation/Setup Instructions

Only the main translation unit (main() and anonymous-namespace helpers) is provided above; the implementations and build configuration for Calculations.h, FileStorage.h, InputValidation.h, and Records.h are not included in the snippet.

- Place this source file alongside Calculations.h, FileStorage.h, InputValidation.h, Records.h, and their corresponding implementation sources/libraries in your project directory.
- Compile the sources together with a C++ compiler supporting C++17 or later (required for <filesystem>).

## How to Run the Program

Run the compiled executable from your terminal/command prompt:

- On startup, the program resolves the data file path via dataPath():
  - Checks symlink_status for "peraprudent-data.txt", "peraprudent-data.txt.bak", and "peraprudent-data.txt.tmp" in the current working directory. If any exists (or symlink_status returns an error other than errc::no_such_file_or_directory), it selects "peraprudent-data.txt".
  - Otherwise, it selects "float-data.txt".
- It calls floatapp::loadData(path, data, error):
  - If LoadStatus::Error, it prints Cannot load records: <error> to cerr and exits with code 1.
  - Otherwise, it prints either Saved records loaded.\n or First run: no saved records yet.\n followed by Data file: <path>\n.
- Enter menu options 1–17 to perform actions, or 0 to exit.

## Usage Example

```text
Saved records loaded.
Data file: float-data.txt

+--------------------------------------------------------------------------+
|                                  FLOAT                                   |
|       Flexible Ledger for Obligations and Allowance Tracking             |
+--------------------------------------------------------------------------+
| INCOME AND PAYMENTS                | EXPENSES AND BILLS                  |
+------------------------------------+-------------------------------------+
| 1  Add income                      | 7  Add expense                      |
| 2  View income                     | 8  View expenses                    |
| 3  Add expected payment            | 9  Search expenses                  |
| 4  View expected payments          | 10 Update expense                   |
| 5  Receive payment                 | 11 Delete expense                   |
| 6  Reschedule expected payment     | 12 Add bill                         |
|                                    | 13 View bills                       |
|                                    | 14 Pay bill                         |
+------------------------------------+-------------------------------------+
| SAVINGS AND PLANNING               | SUMMARY                             |
+------------------------------------+-------------------------------------+
| 15 Reserve savings                 | 16 Financial summary                |
| 17 Daily spending plan             | 0  Exit                             |
+------------------------------------+-------------------------------------+
Choose an option: 1
Source: Freelance Project
Date received (YYYY-MM-DD): 2026-11-24
Amount received (pesos): 1500.50
Description: Client payout
Saved successfully.
```

## Input and Output Information

### Inputs (Standard Input cin)

- Menu choice: Integer in range 0–17 via floatapp::readMenuChoice.
- ID / confirmation choices: Integers validated via floatapp::readMenuChoice (0 to cancel ID prompts; 0 or 1 for delete confirmation).
- Text fields: Strings validated via floatapp::readRequiredText (source, description, category, title, and search term).
- Dates: Strings validated via floatapp::readDate (YYYY-MM-DD format for income dates, expected/revised dates, expense dates, bill due/payment dates, and planning dates).
- Amounts: Peso values parsed into centavos via floatapp::readMoney (disallowing zero except for setSavings, which passes allowZero = true).

### Outputs (Standard Output cout and Standard Error cerr)

- Menu & status messages: ASCII menu box, startup status, Saved successfully., Goodbye. Successful changes were saved immediately., and prompt/validation messages.
- Record listings: Line-delimited fields with monetary values formatted by money(...) as PHP <pesos>.<cc> (or -PHP <pesos>.<cc> for negative values).
- Financial summary & daily plan: Totals for received income, paid expenses, current balance, unpaid bills reserved (all dates), savings reserved, available spending funds, category totals, next expected income details, days to cover, and estimated daily limit.
- Errors (cerr): Record ID allocation errors, SAVE FAILED: <error>..., Cannot load records: <error> (exit code 1), and Calculation unavailable: <what> on std::overflow_error when summing category expenses.
- Data file (stdout / FileStorage.h): Reads from and writes to "peraprudent-data.txt" or "float-data.txt" in the current working directory.

## Project Structure

Based on the provided file:

- main.cpp (or unnamed main source file): Contains dataPath, money, input/persistence/lookup helpers, CRUD and reporting menu handlers (viewIncome, addIncome, viewExpected, addExpected, receivePayment, reschedulePayment, printExpense, viewExpenses, expenseFields, addExpense, searchExpenses, editExpense, viewBills, addBill, payBill, setSavings, dayNumber, summary, dailyPlan, menu), and main.
- Calculations.h: Referenced header in floatapp namespace (provides totalIncomeCentavos, totalExpensesCentavos, currentBalanceCentavos, unpaidBillsCentavos, spendableBalanceCentavos, dailySpendingLimitCentavos).
- FileStorage.h: Referenced header in floatapp namespace (provides LoadStatus, loadData, saveData).
- InputValidation.h: Referenced header in floatapp namespace (provides readRequiredText, readDate, readMoney, readMenuChoice).
- Records.h: Referenced header in floatapp namespace (provides AppData, Income, ExpectedPayment, Expense, Bill, nextRecordId).

## Limitations

- Single-file visibility: Only the main source file is provided; internal behavior of Calculations.h, FileStorage.h, InputValidation.h, and Records.h is not shown.
- Case-sensitive expense search: searchExpenses uses std::string::find, which is case-sensitive and matches across date, category, and description rather than filtering by a specific field.
- Reserved "Bill payment" category: Users cannot manually add, edit, or delete expenses with category "Bill payment"; such entries are only created when paying a bill (payBill), and editExpense blocks editing/deleting them.
- Fixed currency & current-directory storage: Monetary output is hardcoded to PHP (pesos and centavos), and data file detection looks only in the current working directory (peraprudent-data.txt / .bak / .tmp or float-data.txt).
- Daily plan behavior with overdue or missing future payments: dailyPlan warns if any pending payment has expectedDate <= planningDate and returns early without calculating a limit if there is no unreceived payment with expectedDate > planningDate.

## Possible Future Improvements

- Add support for editing or deleting income records, expected payments, or bills directly from the menu.
- Add case-insensitive search and field-specific filters (by date, category, or description) in searchExpenses.
- Support configurable data file paths or command-line flags instead of relying solely on current-directory auto-detection.
- Expand dailyPlan to handle cases where no future pending payment is scheduled or when overdue items exist.
