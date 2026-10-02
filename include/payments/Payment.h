#ifndef PAYMENT_H
#define PAYMENT_H

#include <string>
#include <memory>
#include "json.hpp"

// ABSTRACTION: Abstract base class representing a payment transaction.
// Enforces immutability: financial fields are strictly private with no public mutators.
// State transition (PENDING -> SUCCESS/FAILED) occurs strictly through processPayment().
class Payment {
private:
    double amount;
    std::string transactionId;
    std::string date;
    std::string status; // "PENDING", "SUCCESS", "FAILED"

protected:
    // ENCAPSULATION: Status mutation is protected so only derived payment implementations can set final outcome.
    void updateStatus(const std::string& newStatus);

public:
    Payment(double amount, const std::string& transactionId, const std::string& date, const std::string& status = "PENDING");

    // POLYMORPHISM: Virtual destructor for polymorphic cleanup of derived payment channels.
    virtual ~Payment() = default;

    // ENCAPSULATION: Const accessors ensuring immutability for billing records.
    double getAmount() const;
    std::string getTransactionId() const;
    std::string getDate() const;
    std::string getStatus() const;

    // ABSTRACTION & POLYMORPHISM: Pure virtual workflow methods for concrete payment gateways.
    virtual bool processPayment() = 0;
    virtual std::string getPaymentInstruction() const = 0;
    virtual std::string getPaymentMethod() const = 0;

    // POLYMORPHISM: Serializes transaction data to JSON.
    virtual nlohmann::json toJson() const;
};

#endif // PAYMENT_H
