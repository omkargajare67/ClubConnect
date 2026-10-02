#include "payments/CashPayment.h"

CashPayment::CashPayment(double amount, const std::string& transactionId, const std::string& date,
                         const std::string& counterName, const std::string& status)
    : Payment(amount, transactionId, date, status), counterName(counterName) {}

std::string CashPayment::getCounterName() const {
    return counterName;
}

// POLYMORPHISM: Marks cash receipt as collected at counter.
bool CashPayment::processPayment() {
    updateStatus("SUCCESS");
    return true;
}

// POLYMORPHISM: Instructs student where on campus to deposit payment.
std::string CashPayment::getPaymentInstruction() const {
    return "Pay at counter " + counterName;
}

std::string CashPayment::getPaymentMethod() const {
    return "Cash";
}

nlohmann::json CashPayment::toJson() const {
    nlohmann::json j = Payment::toJson();
    j["counterName"] = counterName;
    j["instruction"] = getPaymentInstruction();
    return j;
}
