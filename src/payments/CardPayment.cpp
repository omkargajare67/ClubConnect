#include "payments/CardPayment.h"
#include "core/Exceptions.h"
#include <algorithm>
#include <cctype>

// ENCAPSULATION & DATA SECURITY: Validates 16-digit PAN card number and stores ONLY the last 4 digits.
CardPayment::CardPayment(double amount, const std::string& transactionId, const std::string& date,
                         const std::string& rawCardNumber, const std::string& cardHolderName,
                         const std::string& status)
    : Payment(amount, transactionId, date, status), cardHolderName(cardHolderName) {
    
    // Strip hyphens and spaces if any
    std::string cleanNumber;
    for (char c : rawCardNumber) {
        if (std::isdigit(static_cast<unsigned char>(c))) {
            cleanNumber += c;
        } else if (c != ' ' && c != '-') {
            throw PaymentFailedException("Invalid card number format: contains non-numeric characters.");
        }
    }

    if (cleanNumber.length() != 16) {
        throw PaymentFailedException("Invalid card number: Card must contain exactly 16 digits.");
    }

    last4Digits = cleanNumber.substr(12, 4);
}

CardPayment::CardPayment(double amount, const std::string& transactionId, const std::string& date,
                         const std::string& cardHolderName, const std::string& last4Digits, bool isMasked,
                         const std::string& status)
    : Payment(amount, transactionId, date, status), last4Digits(last4Digits), cardHolderName(cardHolderName) {
    (void)isMasked;
}

std::string CardPayment::getLast4Digits() const { return last4Digits; }
std::string CardPayment::getCardHolderName() const { return cardHolderName; }

// POLYMORPHISM: Approves card transaction simulation.
bool CardPayment::processPayment() {
    updateStatus("SUCCESS");
    return true;
}

// POLYMORPHISM: User prompt instruction for card payment.
std::string CardPayment::getPaymentInstruction() const {
    return "Enter card details";
}

std::string CardPayment::getPaymentMethod() const {
    return "Card";
}

nlohmann::json CardPayment::toJson() const {
    nlohmann::json j = Payment::toJson();
    j["cardHolderName"] = cardHolderName;
    j["last4Digits"] = last4Digits;
    j["instruction"] = getPaymentInstruction();
    return j;
}
