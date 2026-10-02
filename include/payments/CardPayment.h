#ifndef CARD_PAYMENT_H
#define CARD_PAYMENT_H

#include "payments/Payment.h"

// INHERITANCE: CardPayment processes credit/debit card transactions securely.
class CardPayment : public Payment {
private:
    std::string last4Digits;
    std::string cardHolderName;

public:
    CardPayment(double amount, const std::string& transactionId, const std::string& date,
                const std::string& rawCardNumber, const std::string& cardHolderName,
                const std::string& status = "PENDING");

    // Overload for re-instantiation from storage (already masked to 4 digits)
    CardPayment(double amount, const std::string& transactionId, const std::string& date,
                const std::string& cardHolderName, const std::string& last4Digits, bool isMasked,
                const std::string& status = "PENDING");

    virtual ~CardPayment() override = default;

    // ENCAPSULATION: Only the non-sensitive masked card digits are exposed.
    std::string getLast4Digits() const;
    std::string getCardHolderName() const;

    // POLYMORPHISM: Concrete implementations for Payment interface.
    bool processPayment() override;
    std::string getPaymentInstruction() const override;
    std::string getPaymentMethod() const override;
    nlohmann::json toJson() const override;
};

#endif // CARD_PAYMENT_H
