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
/**
 * Sums received income amounts without changing data.
 * @param data Account records; income amounts must be nonnegative centavos.
 * @return Total received income in centavos; zero if there are no incomes.
 * @throws std::overflow_error If an amount is negative or the sum overflows.
 * Expected payments are excluded. This function performs no file I/O.
 */
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
/**
 * Calculates a nonnegative daily limit using current spendable funds.
 * @param data Valid account records, with nonnegative reservation amounts.
 * @param daysUntilNextIncome Number of days to cover; dates are handled by caller.
 * @return Zero if days are nonpositive or funds are nonpositive; otherwise the
 *         centavo quotient, rounded down by integer division.
 * @throws std::overflow_error If underlying totals or balances exceed range.
 * All unpaid bills and reserved savings are deducted. Expected income is excluded.
 */
long long dailySpendingLimitCentavos(const AppData& data, int daysUntilNextIncome) {
    if (daysUntilNextIncome <= 0) return 0;
    const auto spendable = spendableBalanceCentavos(data);
    return spendable <= 0 ? 0 : spendable / daysUntilNextIncome;
}
}

