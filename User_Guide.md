# FLOAT User Guide

## Purpose

FLOAT helps a local user record irregular income, expenses, bills, and savings and estimate available daily spending. It does not connect to a bank.

## Requirements and setup

Use a computer with a C++17 compiler and terminal. Keep all eight .cpp/.h files in one folder. In VS Code, open that folder and choose Terminal > New Terminal. The folder must permit writing local save files.

## Launch

Compile from the project folder:

```powershell
g++ -std=c++17 main.cpp Calculations.cpp InputValidation.cpp FileStorage.cpp -o FLOAT.exe
```

If compilation succeeds, run `.\FLOAT.exe` on Windows. If execution is blocked by Windows Application Control, record the issue and run on an approved working computer. Do not claim the program ran solely because an executable was created.

## Basic usage

1. Choose option 1 to record income. Enter its source, received date, amount in pesos, and description.
2. Choose option 2 to view saved income.
3. Use 3–6 for expected payments, 7–11 for expenses, and 12–14 for bills.
4. Use 15 for reserved savings, 16 for the summary, or 17 for the daily spending plan.
5. Enter 0 at the main menu to exit. Successful changes are saved immediately.

## Example to verify

On a clean demo account, add Sample allowance dated 2026-11-24 for PHP 500.00 with description Documentation demo. The program should display `Saved successfully.` Viewing income should show `1 | 2026-11-24 | Sample allowance | PHP 500.00 | Documentation demo`. Existing accounts can have a different ID.

This transcript must be confirmed on the actual build before being labelled a verified example in the report.

## Troubleshooting

| Issue | Action |
| :--- | :--- |
| g++ not recognized | Check the compiler installation and PATH. |
| Source file missing | Open the terminal in the folder containing all eight source files. |
| Invalid input | Follow the displayed amount, date, text, or menu requirements. |
| Cannot load records or save failed | Keep the existing files and inspect the reported storage error. Do not overwrite potentially useful recovery data. |
| No daily estimate | Add or reschedule a pending payment to a future date. Resolve any reported spending shortfall. |

## Limitations

Manual entries and expected dates may be inaccurate. Expected income is excluded until received. All unpaid bills and reserved savings reduce spending funds. There is no bank connection, login system, or graphical interface.
