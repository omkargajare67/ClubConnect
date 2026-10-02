#ifndef CASH_PAYMENT_H
#define CASH_PAYMENT_H

#include "payments/Payment.h"

// INHERITANCE: CashPayment handles physical on-campus ticket counter payments.
class CashPayment : public Payment {
private:
    std::string counterName;

public:
    CashPayment(double amount, const std::string& transactionId, const std::string& date,
                const std::string& counterName = "Student Affairs Helpdesk (Room 102)",
                const std::string& status = "PENDING");

    virtual ~CashPayment() override = default;

    // ENCAPSULATION: Read-only getter.
    std::string getCounterName() const;

    // POLYMORPHISM: Concrete implementations for Payment interface.
    bool processPayment() override;
    std::string getPaymentInstruction() const override;
    std::string getPaymentMethod() const override;
    nlohmann::json toJson() const override;
};

#endif // CASH_PAYMENT_H
