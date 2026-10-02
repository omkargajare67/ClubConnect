#include "payments/PaymentFactory.h"
#include "payments/UPIPayment.h"
#include "payments/CardPayment.h"
#include "payments/CashPayment.h"
#include <stdexcept>

// FACTORY METHOD: Rebuilds appropriate Payment subclass based on method attribute.
std::shared_ptr<Payment> PaymentFactory::createPayment(const nlohmann::json& j) {
    double amount = j.value("amount", 0.0);
    std::string txnId = j.value("transactionId", "");
    std::string date = j.value("date", "");
    std::string status = j.value("status", "SUCCESS");
    std::string method = j.value("method", "UPI");

    if (method == "UPI") {
        std::string eventName = j.value("eventName", "Event Registration");
        std::string studentUpi = j.value("studentUpiId", "student@upi");
        std::string merchantUpi = j.value("merchantUpiId", UPIPayment::DEFAULT_MERCHANT_VPA);
        return std::make_shared<UPIPayment>(amount, txnId, date, eventName, studentUpi, merchantUpi, status);
    } else if (method == "Card") {
        std::string holder = j.value("cardHolderName", "Student");
        std::string last4 = j.value("last4Digits", "0000");
        return std::make_shared<CardPayment>(amount, txnId, date, holder, last4, true, status);
    } else if (method == "Cash" || method == "FREE") {
        std::string counter = j.value("counterName", "Helpdesk");
        return std::make_shared<CashPayment>(amount, txnId, date, counter, status);
    }

    throw std::runtime_error("Unknown payment method in factory: " + method);
}
