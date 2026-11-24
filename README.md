# FLOAT.cpp

Flexible Ledger for Obligations and Allowance Tracking is a C++ console system for manually tracking irregular income and planning spending.

## Features

- Record received income and pending payments; receive or reschedule pending payments.
- Add, view, search, update, and delete manual expenses.
- Add and pay bills with duplicate-payment protection.
- Reserve savings and view balances and expense-category totals.
- Estimate a daily limit until the earliest future pending payment.
- Validate inputs and save changes to local text storage.

## Requirements and setup

A C++17 compiler with standard-library filesystem support and a terminal. No external packages, database, or messaging services are required. Open this folder in VS Code and select Terminal > New Terminal.

Verify the compiler with `g++ --version`. If the installed MSYS2 UCRT64 compiler is not on the current Windows terminal PATH, use:

```powershell
$env:Path = "C:\msys64\ucrt64\bin;" + $env:Path
```

That path is specific to this student's setup. Other installations may use a different compiler location.

## Build and run

```powershell
g++ -std=c++17 -Wall -Wextra -Wpedantic main.cpp Calculations.cpp InputValidation.cpp FileStorage.cpp -o FLOAT.exe
```

After compilation succeeds:

```powershell
.\FLOAT.exe
```

Windows Application Control may block execution on this student's device. Record that restriction honestly and obtain runtime evidence on a permitted working computer; successful compilation alone is not runtime verification.

## Usage example

In a clean demo folder with no saved financial data, select option 1 and enter:

```text
Source: Sample allowance
Date received (YYYY-MM-DD): 2026-11-24
Amount received (pesos): 500.00
Description: Documentation demo
Saved successfully.
```

Select option 2. On a clean account, the expected record is:

```text
1 | 2026-11-24 | Sample allowance | PHP 500.00 | Documentation demo
```

Actual IDs depend on existing records. This is an expected transcript until verified on the documented build. Exit with option 0. Successful mutations are saved immediately.

## Input and output

Menu options range from 0 to 17. Dates use YYYY-MM-DD and amounts are entered in pesos, then stored as integer centavos. Required text, numeric ranges, and dates are validated. Output includes records, totals, validation feedback, and storage errors.

Current balance is received income minus recorded expenses. Spendable funds also deduct all unpaid bills and reserved savings. Future expected amounts do not increase the balance until received. Positive daily limits use integer division in centavos, so fractional centavos are discarded. Shortfalls and missing future payment dates produce explanatory messages.

## Project structure

| File | Responsibility |
| :--- | :--- |
| main.cpp | Startup, menu, record operations, summary, and planning |
| Records.h | Shared records and AppData |
| Calculations.cpp / .h | Totals, balances, and daily limit |
| InputValidation.cpp / .h | Input parsing and record validation |
| FileStorage.cpp / .h | Local save/load and recovery checks |
| original_source/ | Unmodified project snapshot before adding TA5 comments |

## Storage and limitations

New folders use float-data.txt. If legacy peraprudent-data.txt or its recovery files exist, the application selects that legacy path. Temporary and backup files support save safeguards; do not delete recovery files or reset personal data just to repeat an example.

This is a single-user local application. No bank integration, authentication, or guaranteed payment prediction is provided. Expense searches are case-sensitive. The daily planner uses a user-entered planning date and reserves all unpaid bills, including those due after the next income. IDs are unique among current records, not permanent historical IDs. Linked bill-payment expenses cannot be changed through generic expense editing.

## Possible improvements

Add a graphical interface, clearer recovery options, permanent transaction links, and more automated workflow tests.

## Credits

FLOAT was developed as a group project by Erin Dale T. Gaetos, Louie Emanuel B. Gravidez, and Timothy Michael M. Uy. This separate TA5 copy preserves that attribution and adds software documentation. Original group repository: https://github.com/arendelleh/CS0053-TN35-GitLTE-FLOAT .

