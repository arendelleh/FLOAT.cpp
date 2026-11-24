#include "Calculations.h"
#include "FileStorage.h"
#include "InputValidation.h"
#include "Records.h"

#include <algorithm>
#include <climits>
#include <ctime>
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

namespace {
using namespace std;
using floatapp::AppData;

string dataPath() {
    namespace fs = filesystem;
    for (const string suffix : {"", ".bak", ".tmp"}) {
        error_code error;
        auto state = fs::symlink_status("peraprudent-data.txt" + suffix, error);
        if (fs::exists(state) || (error && error != errc::no_such_file_or_directory))
            return "peraprudent-data.txt";
    }
    return "float-data.txt";
}

string money(long long amount) {
    // Unsigned magnitude also supports LLONG_MIN without signed negation overflow.
    const auto magnitude = amount < 0 ? 0ULL - static_cast<unsigned long long>(amount)
                                      : static_cast<unsigned long long>(amount);
    ostringstream out;
    out << (amount < 0 ? "-PHP " : "PHP ") << magnitude / 100 << '.'
        << setfill('0') << setw(2) << magnitude % 100;
    return out.str();
}

bool text(const char* prompt, string& value) {
    cout << prompt;
    return floatapp::readRequiredText(cin, cout, value);
}
bool date(const char* prompt, string& value) {
    cout << prompt;
    return floatapp::readDate(cin, cout, value);
}
bool amount(const char* prompt, long long& value, bool allowZero = false) {
    cout << prompt;
    return floatapp::readMoney(cin, cout, value, allowZero);
}
bool choice(const char* prompt, int low, int high, int& value) {
    cout << prompt;
    return floatapp::readMenuChoice(cin, cout, low, high, value);
}

bool allocateId(const AppData& data, int& id) {
    string error;
    if (floatapp::nextRecordId(data, id, error)) return true;
    cerr << error << '\n';
    return false;
}
bool persist(AppData& active, const AppData& proposed, const string& path) {
    string error;
    if (!floatapp::saveData(path, proposed, error)) {
        cerr << "SAVE FAILED: " << error
                  << "\nChanges were not applied to active records. Inspect storage before retrying.\n";
        return false;
    }
    active = proposed;
    cout << "Saved successfully.\n";
    return true;
}

template <typename T>
auto findId(vector<T>& records, int id) {
    for (auto record = records.begin(); record != records.end(); ++record) {
        if (record->id == id) return record;
    }
    return records.end();
}

void viewIncome(const AppData& data) {
    if (data.receivedIncomes.empty()) cout << "No received income recorded.\n";
    for (const auto& r : data.receivedIncomes)
        cout << r.id << " | " << r.date << " | " << r.source << " | "
                  << money(r.amountCentavos) << " | " << r.description << '\n';
}
void addIncome(AppData& data, const string& path) {
    floatapp::Income r;
    if (!allocateId(data, r.id) || !text("Source: ", r.source) ||
        !date("Date received (YYYY-MM-DD): ", r.date) ||
        !amount("Amount received (pesos): ", r.amountCentavos) ||
        !text("Description: ", r.description)) return;
    auto proposed = data;
    proposed.receivedIncomes.push_back(r);
    persist(data, proposed, path);
}

void viewExpected(const AppData& data) {
    if (data.expectedPayments.empty()) cout << "No expected payments.\n";
    for (const auto& r : data.expectedPayments)
        cout << r.id << " | " << r.expectedDate << " | " << r.source
                  << " | estimated " << money(r.amountCentavos)
                  << " | " << (r.received ? "RECEIVED" : "PENDING") << '\n';
}
void addExpected(AppData& data, const string& path) {
    floatapp::ExpectedPayment r;
    if (!allocateId(data, r.id) || !text("Expected source: ", r.source) ||
        !date("Expected date (YYYY-MM-DD): ", r.expectedDate) ||
        !amount("Estimated amount (pesos): ", r.amountCentavos) ||
        !text("Description: ", r.description)) return;
    auto proposed = data;
    proposed.expectedPayments.push_back(r);
    persist(data, proposed, path);
}
void receivePayment(AppData& data, const string& path) {
    viewExpected(data);
    int id;
    if (!choice("Payment ID (0 cancels): ", 0, INT_MAX - 1, id) || id == 0) return;
    auto found = findId(data.expectedPayments, id);
    if (found == data.expectedPayments.end()) { cout << "Payment not found.\n"; return; }
    if (found->received) { cout << "Already received; no duplicate income added.\n"; return; }
    floatapp::Income income;
    if (!allocateId(data, income.id) || !date("Actual received date: ", income.date) ||
        !amount("Actual received amount (pesos): ", income.amountCentavos)) return;
    income.source = found->source;
    income.description = found->description;
    auto proposed = data;
    findId(proposed.expectedPayments, id)->received = true;
    proposed.receivedIncomes.push_back(income);
    persist(data, proposed, path);
}
void reschedulePayment(AppData& data, const string& path) {
    viewExpected(data);
    int id;
    if (!choice("Pending payment ID (0 cancels): ", 0, INT_MAX - 1, id) || id == 0) return;
    auto proposed = data;
    auto r = findId(proposed.expectedPayments, id);
    if (r == proposed.expectedPayments.end() || r->received) {
        cout << "Choose an existing pending payment.\n"; return;
    }
    if (!date("Revised expected date: ", r->expectedDate)) return;
    persist(data, proposed, path);
}

void printExpense(const floatapp::Expense& r) {
    cout << r.id << " | " << r.date << " | " << r.category << " | "
              << money(r.amountCentavos) << " | " << r.description << '\n';
}
void viewExpenses(const AppData& data) {
    if (data.expenses.empty()) cout << "No expenses recorded.\n";
    for (const auto& r : data.expenses) printExpense(r);
}
bool expenseFields(floatapp::Expense& r) {
    while (text("Category: ", r.category)) {
        if (r.category != "Bill payment")
            return date("Expense date: ", r.date) && amount("Amount (pesos): ", r.amountCentavos)
                   && text("Description: ", r.description);
        cout << "Bill payment is reserved for the bill-payment operation.\n";
    }
    return false;
}
void addExpense(AppData& data, const string& path) {
    floatapp::Expense r;
    if (!allocateId(data, r.id) || !expenseFields(r)) return;
    auto proposed = data;
    proposed.expenses.push_back(r);
    persist(data, proposed, path);
}
void searchExpenses(const AppData& data) {
    string term;
    if (!text("Search date/category/description (case-sensitive): ", term)) return;
    bool any = false;
    for (const auto& r : data.expenses)
        if (r.date.find(term) != string::npos || r.category.find(term) != string::npos ||
            r.description.find(term) != string::npos) { printExpense(r); any = true; }
    if (!any) cout << "No matches.\n";
}
void editExpense(AppData& data, const string& path, bool remove) {
    viewExpenses(data);
    int id;
    if (!choice("Expense ID (0 cancels): ", 0, INT_MAX - 1, id) || id == 0) return;
    auto proposed = data;
    auto r = findId(proposed.expenses, id);
    if (r == proposed.expenses.end()) { cout << "Expense not found.\n"; return; }
    if (r->category == "Bill payment") {
        cout << "Linked bill expenses cannot be edited/deleted here.\n"; return;
    }
    if (remove) {
        int confirm;
        if (!choice("Delete? 1 = Yes, 0 = Cancel: ", 0, 1, confirm) || !confirm) return;
        proposed.expenses.erase(r);
    } else if (!expenseFields(*r)) return;
    persist(data, proposed, path);
}

void viewBills(const AppData& data) {
    if (data.bills.empty()) cout << "No bills.\n";
    for (const auto& r : data.bills)
        cout << r.id << " | " << r.title << " | due " << r.dueDate << " | "
                  << money(r.amountCentavos) << " | " << (r.paid ? "PAID" : "UNPAID / RESERVED") << '\n';
}
void addBill(AppData& data, const string& path) {
    floatapp::Bill r;
    if (!allocateId(data, r.id) || !text("Bill title: ", r.title) ||
        !date("Due date: ", r.dueDate) || !amount("Bill amount (pesos): ", r.amountCentavos)) return;
    auto proposed = data;
    proposed.bills.push_back(r);
    persist(data, proposed, path);
}
void payBill(AppData& data, const string& path) {
    viewBills(data);
    int id;
    if (!choice("Bill ID (0 cancels): ", 0, INT_MAX - 1, id) || !id) return;
    auto bill = findId(data.bills, id);
    if (bill == data.bills.end()) { cout << "Bill not found.\n"; return; }
    if (bill->paid) { cout << "Already paid; no duplicate expense added.\n"; return; }
    floatapp::Expense expense;
    if (!allocateId(data, expense.id) || !date("Actual payment date: ", expense.date)) return;
    expense.category = "Bill payment";
    expense.description = "Bill ID " + to_string(id);
    expense.amountCentavos = bill->amountCentavos;
    auto proposed = data;
    findId(proposed.bills, id)->paid = true;
    proposed.expenses.push_back(expense);
    persist(data, proposed, path);
}
void setSavings(AppData& data, const string& path) {
    auto proposed = data;
    if (!amount("Total savings to reserve (pesos; 0 clears): ", proposed.reservedSavingsCentavos, true)) return;
    persist(data, proposed, path);
}

// Valid Gregorian dates are supplied by the shared validator.
int dayNumber(const string& value) {
    int y = stoi(value.substr(0, 4)), m = stoi(value.substr(5, 2));
    int d = stoi(value.substr(8, 2));
    const int previousYear = y - 1;
    int result = 365 * previousYear + previousYear / 4 - previousYear / 100 + previousYear / 400;
    const int months[] = {31,28,31,30,31,30,31,31,30,31,30,31};
    for (int month = 1; month < m; ++month) result += months[month - 1];
    if (m > 2 && (y % 400 == 0 || (y % 4 == 0 && y % 100 != 0))) ++result;
    return result + d;
}
void summary(const AppData& data) {
    cout << "Received income: " << money(floatapp::totalIncomeCentavos(data))
              << "\nPaid expenses: " << money(floatapp::totalExpensesCentavos(data))
              << "\nCurrent balance: " << money(floatapp::currentBalanceCentavos(data))
              << "\nUnpaid bills reserved (all dates): " << money(floatapp::unpaidBillsCentavos(data))
              << "\nSavings reserved: " << money(data.reservedSavingsCentavos)
              << "\nAvailable spending funds: " << money(floatapp::spendableBalanceCentavos(data)) << '\n';
    const auto available = floatapp::spendableBalanceCentavos(data);
    if (available < 0) cout << "SHORTFALL: available funds are negative.\n";
    vector<string> categories;
    for (const auto& r : data.expenses)
        if (find(categories.begin(), categories.end(), r.category) == categories.end()) categories.push_back(r.category);
    for (const auto& category : categories) {
        long long total = 0;
        for (const auto& r : data.expenses) if (r.category == category) {
            if (r.amountCentavos > LLONG_MAX - total) throw overflow_error("Category total too large.");
            total += r.amountCentavos;
        }
        cout << "Category " << category << ": " << money(total) << '\n';
    }
}
void dailyPlan(const AppData& data) {
    string planningDate;
    if (!date("Planning date (YYYY-MM-DD; use today's date for current plan): ", planningDate)) return;
    const floatapp::ExpectedPayment* next = nullptr;
    bool overdue = false;
    for (const auto& r : data.expectedPayments) if (!r.received) {
        if (r.expectedDate <= planningDate) overdue = true;
        else if (!next || r.expectedDate < next->expectedDate) next = &r;
    }
    if (overdue) cout << "Pending payment(s) are due/overdue. Update their dates or record receipt.\n";
    if (!next) { cout << "No future pending payment. Add or reschedule one before calculating.\n"; return; }
    const int days = dayNumber(next->expectedDate) - dayNumber(planningDate);
    summary(data);
    cout << "Next expected income: " << next->expectedDate << " | " << next->source
              << "\nDays to cover: " << days << '\n';
    if (floatapp::spendableBalanceCentavos(data) < 0) {
        cout << "Resolve the shortfall; no positive daily budget is available.\n"; return;
    }
    cout << "Estimated daily limit: " << money(floatapp::dailySpendingLimitCentavos(data, days))
              << "\nExpected income is excluded until received. All unpaid bills are reserved.\n";
}
void menu() {
    cout << "\n";
    cout << "+--------------------------------------------------------------------------+\n";
    cout << "|                                  FLOAT                                   |\n";
    cout << "|       Flexible Ledger for Obligations and Allowance Tracking             |\n";
    cout << "+--------------------------------------------------------------------------+\n";
    cout << "| INCOME AND PAYMENTS                | EXPENSES AND BILLS                  |\n";
    cout << "+------------------------------------+-------------------------------------+\n";
    cout << "| 1  Add income                      | 7  Add expense                      |\n";
    cout << "| 2  View income                     | 8  View expenses                    |\n";
    cout << "| 3  Add expected payment            | 9  Search expenses                  |\n";
    cout << "| 4  View expected payments          | 10 Update expense                   |\n";
    cout << "| 5  Receive payment                 | 11 Delete expense                   |\n";
    cout << "| 6  Reschedule expected payment     | 12 Add bill                         |\n";
    cout << "|                                    | 13 View bills                       |\n";
    cout << "|                                    | 14 Pay bill                         |\n";
    cout << "+------------------------------------+-------------------------------------+\n";
    cout << "| SAVINGS AND PLANNING               | SUMMARY                             |\n";
    cout << "+------------------------------------+-------------------------------------+\n";
    cout << "| 15 Reserve savings                 | 16 Financial summary                |\n";
    cout << "| 17 Daily spending plan             | 0  Exit                             |\n";
    cout << "+------------------------------------+-------------------------------------+\n";
    cout << "Choose an option: ";
}
} // namespace

int main() {
    AppData data;
    const string path = dataPath();
    string error;
    const auto status = floatapp::loadData(path, data, error);
    if (status == floatapp::LoadStatus::Error) {
        cerr << "Cannot load records: " << error << '\n'; return 1;
    }
    cout << (status == floatapp::LoadStatus::Loaded ? "Saved records loaded.\n" : "First run: no saved records yet.\n")
              << "Data file: " << path << '\n';
    int option;
    while (true) {
        menu();
        if (!choice("", 0, 17, option) || option == 0) break;
        try {
            switch (option) {
            case 1: addIncome(data, path); break;
            case 2: viewIncome(data); break;
            case 3: addExpected(data, path); break;
            case 4: viewExpected(data); break;
            case 5: receivePayment(data, path); break;
            case 6: reschedulePayment(data, path); break;
            case 7: addExpense(data, path); break;
            case 8: viewExpenses(data); break;
            case 9: searchExpenses(data); break;
            case 10: editExpense(data, path, false); break;
            case 11: editExpense(data, path, true); break;
            case 12: addBill(data, path); break;
            case 13: viewBills(data); break;
            case 14: payBill(data, path); break;
            case 15: setSavings(data, path); break;
            case 16: summary(data); break;
            case 17: dailyPlan(data); break;
            }
        } catch (const overflow_error& e) {
            cerr << "Calculation unavailable: " << e.what() << '\n';
        }
    }
    cout << "Goodbye. Successful changes were saved immediately.\n";
    return 0;
}
