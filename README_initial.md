# FLOAT.cpp

Initial AI-generated documentation draft for the existing C++ project, prepared by ChatGPT for TA5. This is separate from the earlier Java activity drafts.

## Description

FLOAT is a console tracker for received income, expected payments, expenses, bills, and reserved savings. It calculates balances and estimates a daily spending limit.

## Features

- Add and view received income and expected payments.
- Receive and reschedule pending payments.
- Add, view, search, update, and delete manual expenses.
- Add, view, and pay bills.
- Set reserved savings, view a financial summary, and calculate a daily plan.
- Validate inputs and save records locally.

## Technology and setup

C++17 and the C++ standard library. Use a compiler with C++17 filesystem support. Put the eight .cpp/.h source files in one folder. No external library or database is referenced by the application code.

## Build and run on Windows

```powershell
g++ -std=c++17 main.cpp Calculations.cpp InputValidation.cpp FileStorage.cpp -o FLOAT.exe
.\FLOAT.exe
```

## Usage and input/output

Select a menu number and answer its prompts. Enter money in pesos and dates as YYYY-MM-DD. For example, option 1 records income and option 2 displays it. A successfully saved record produces `Saved successfully.` Menu option 0 exits.

## Structure

main.cpp provides the menu and operations. Records.h defines the shared structures. Calculations.cpp/.h calculates financial totals. InputValidation.cpp/.h validates data. FileStorage.cpp/.h saves and loads records.

## Limitations and future work

The program is local and manually operated. It has no banking connection or user authentication. Expected income is excluded until received. All unpaid bills are reserved. Future improvements could include graphical input and explicit permanent transaction links.
