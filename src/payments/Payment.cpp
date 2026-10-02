#include "payments/Payment.h"

// ENCAPSULATION: Constructor initializes immutable transaction fields.
Payment::Payment(double amount, const std::string& transactionId, const std::string& date, const std::string& status)
    : amount(amount), transactionId(transactionId), date(date), status(status) {}

// ENCAPSULATION: State transition only through controlled method.
void Payment::updateStatus(const std::string& newStatus) {
    status = newStatus;
}

double Payment::getAmount() const { return amount; }
std::string Payment::getTransactionId() const { return transactionId; }
std::string Payment::getDate() const { return date; }
std::string Payment::getStatus() const { return status; }

// POLYMORPHISM: Base JSON serialization.
nlohmann::json Payment::toJson() const {
    return {
        {"transactionId", transactionId},
        {"amount", amount},
        {"date", date},
        {"status", status},
        {"method", getPaymentMethod()}
    };
}
