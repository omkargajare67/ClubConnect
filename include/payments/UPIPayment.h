#ifndef UPI_PAYMENT_H
#define UPI_PAYMENT_H

#include "payments/Payment.h"

// INHERITANCE: UPIPayment implements Unified Payments Interface deep linking and QR generation.
class UPIPayment : public Payment {
public:
    // Global academic demo merchant placeholder
    static constexpr const char* DEFAULT_MERCHANT_VPA = "clubfund@upi";

private:
    std::string merchantUpiId;
    std::string studentUpiId;
    std::string eventName;

    static std::string urlEncode(const std::string& value);

public:
    UPIPayment(double amount, const std::string& transactionId, const std::string& date,
               const std::string& eventName, const std::string& studentUpiId = "",
               const std::string& merchantUpiId = DEFAULT_MERCHANT_VPA,
               const std::string& status = "PENDING");

    virtual ~UPIPayment() override = default;

    // ENCAPSULATION: Read-only getters.
    std::string getMerchantUpiId() const;
    std::string getStudentUpiId() const;
    std::string getEventName() const;

    // POLYMORPHISM: Concrete implementations for Payment interface.
    bool processPayment() override;
    std::string getPaymentInstruction() const override;
    std::string getPaymentMethod() const override;
    nlohmann::json toJson() const override;
};

#endif // UPI_PAYMENT_H
