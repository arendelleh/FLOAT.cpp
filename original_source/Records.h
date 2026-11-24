#ifndef FLOATAPP_RECORDS_H
#define FLOATAPP_RECORDS_H

#include <string>
#include <vector>

namespace floatapp {

struct Income {
    int id = 0;
    std::string source;
    std::string date;
    std::string description;
    long long amountCentavos = 0;
};

struct ExpectedPayment {
    int id = 0;
    std::string source;
    std::string expectedDate;
    std::string description;
    long long amountCentavos = 0;
    bool received = false;
};

struct Expense {
    int id = 0;
    std::string category;
    std::string date;
    std::string description;
    long long amountCentavos = 0;
};

struct Bill {
    int id = 0;
    std::string title;
    std::string dueDate;
    long long amountCentavos = 0;
    bool paid = false;
};

struct AppData {
    std::vector<Income> receivedIncomes;
    std::vector<ExpectedPayment> expectedPayments;
    std::vector<Expense> expenses;
    std::vector<Bill> bills;
    long long reservedSavingsCentavos = 0;
};

}
#endif
