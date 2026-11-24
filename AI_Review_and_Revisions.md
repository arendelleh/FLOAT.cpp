# TA5 Documentation Review

This record concerns the C++ FLOAT copy, not the earlier Java Notify drafts. ChatGPT analyzed the existing source and prepared an initial README, revised README, and comments following the student's request to set up the C++ activity. Keep the actual conversation as prompt evidence.

## Three verified analysis statements

| Statement | Classification | Evidence |
| :--- | :--- | :--- |
| Expected income is excluded from the received-income total. | Correct | totalIncomeCentavos iterates receivedIncomes only. |
| All unpaid bills and savings reduce spendable funds. | Correct | spendableBalanceCentavos deducts unpaidBillsCentavos and reservedSavingsCentavos. |
| Successful changes are saved before replacing active records. | Correct | persist calls saveData and assigns active only after success. |

## Revision table

The instructor template permits factual checks when fewer than two factual errors are found. No factual error was found in the two checked claims below.

| Type | Original draft | Revised or verified finding | Evidence / reason |
| :--- | :--- | :--- | :--- |
| Factual check 1 | Expected income is excluded until received. | No factual error found. | The total sums receivedIncomes; receivePayment creates received income. |
| Factual check 2 | All unpaid bills are reserved. | No factual error found. | unpaidBillsCentavos includes every unpaid bill without a due-date filter. |
| Clarity 1 | Compilation and execution appear together. | Separate build and run steps, with run conditional on successful compilation. | Prevents confusing a failed build with a successful run. |
| Organization 2 | Project files described in one paragraph. | Added a file/responsibility table. | Easier to locate modules. |
| Missing information | Storage filenames and legacy selection not explained. | Added float-data.txt, legacy path, and recovery-file guidance. | dataPath checks legacy files and otherwise selects float-data.txt. |

## Two documented functions

| Function | Comment contents | Verification |
| :--- | :--- | :--- |
| totalIncomeCentavos | Purpose, AppData parameter, centavo return, zero case, excluded expected payments, and overflow/negative-amount exception. | Compared with its receivedIncomes loop and addAmount checks. |
| dailySpendingLimitCentavos | Data/days parameters, zero cases, integer division, deductions, exception, and date-selection limitation. | Compared with the early return and spendable balance calculation. |

Comments are in Calculations.cpp. The original unmodified files are preserved in original_source/.

## Final review decisions

| Suggestion | Decision | Reason |
| :--- | :--- | :--- |
| Explain that expected income is excluded. | Accept | Matches totalIncomeCentavos and avoids overstating available money. |
| State all unpaid bills are reserved. | Accept | Matches calculation behavior, even for later due dates. |
| Describe successful compilation as proof of runtime correctness. | Reject | Execution and output need separate evidence. |

## Student verification still required

Capture source/comment screenshots and a successful C++ run using the documented example. If Windows blocks execution, retain the actual error and obtain runtime evidence on a permitted working machine. Do not reuse Java screenshots as C++ evidence. Save the final User Guide as PDF or DOCX as required by the template.
