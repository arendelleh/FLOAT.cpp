#include "Calculations.h"
#include <limits>
#include <stdexcept>

namespace floatapp {
namespace {
long long addAmount(long long total, long long amount) {
    if (amount < 0 || total < 0 || amount > std::numeric_limits<long long>::max() - total)
        throw std::overflow_error("Total exceeds supported money range.");
    return total + amount;
}
long long subtractAmount(long long value, long long amount) {
    if (amount < 0 || value < std::numeric_limits<long long>::min() + amount)
        throw std::overflow_error("Balance exceeds supported money range.");
    return value - amount;
}
}
long long totalIncomeCentavos(const AppData& data) {
    long long total = 0;
    for (const auto& income : data.receivedIncomes) total = addAmount(total, income.amountCentavos);
    return total;
}
long long totalExpensesCentavos(const AppData& data) {
    long long total = 0;
    for (const auto& expense : data.expenses) total = addAmount(total, expense.amountCentavos);
    return total;
}
long long unpaidBillsCentavos(const AppData& data) {
    long long total = 0;
    for (const auto& bill : data.bills) if (!bill.paid) total = addAmount(total, bill.amountCentavos);
    return total;
}
long long currentBalanceCentavos(const AppData& data) {
    return subtractAmount(totalIncomeCentavos(data), totalExpensesCentavos(data));
}
long long spendableBalanceCentavos(const AppData& data) {
    return subtractAmount(subtractAmount(currentBalanceCentavos(data), unpaidBillsCentavos(data)),
                          data.reservedSavingsCentavos);
}
long long dailySpendingLimitCentavos(const AppData& data, int daysUntilNextIncome) {
    if (daysUntilNextIncome <= 0) return 0;
    const auto spendable = spendableBalanceCentavos(data);
    return spendable <= 0 ? 0 : spendable / daysUntilNextIncome;
}
}
