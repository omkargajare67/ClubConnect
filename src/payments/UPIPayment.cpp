#include "payments/UPIPayment.h"
#include <iomanip>
#include <sstream>
#include <cctype>

UPIPayment::UPIPayment(double amount, const std::string& transactionId, const std::string& date,
                       const std::string& eventName, const std::string& studentUpiId,
                       const std::string& merchantUpiId, const std::string& status)
    : Payment(amount, transactionId, date, status),
      merchantUpiId(merchantUpiId.empty() ? DEFAULT_MERCHANT_VPA : merchantUpiId),
      studentUpiId(studentUpiId), eventName(eventName) {}

std::string UPIPayment::getMerchantUpiId() const { return merchantUpiId; }
std::string UPIPayment::getStudentUpiId() const { return studentUpiId; }
std::string UPIPayment::getEventName() const { return eventName; }

std::string UPIPayment::urlEncode(const std::string& value) {
    std::ostringstream escaped;
    escaped.fill('0');
    escaped << std::hex;
    for (char c : value) {
        if (std::isalnum(static_cast<unsigned char>(c)) || c == '-' || c == '_' || c == '.' || c == '~') {
            escaped << c;
        } else {
            escaped << '%' << std::setw(2) << static_cast<int>(static_cast<unsigned char>(c));
        }
    }
    return escaped.str();
}

// POLYMORPHISM: Simulates external bank confirmation, marking transaction successful.
bool UPIPayment::processPayment() {
    updateStatus("SUCCESS");
    return true;
}

// POLYMORPHISM: Generates authentic standard UPI payment deep link string.
std::string UPIPayment::getPaymentInstruction() const {
    std::ostringstream amountStr;
    amountStr << std::fixed << std::setprecision(2) << getAmount();

    return "upi://pay?pa=" + merchantUpiId +
           "&pn=ClubConnect&am=" + amountStr.str() +
           "&cu=INR&tn=" + urlEncode(eventName);
}

std::string UPIPayment::getPaymentMethod() const {
    return "UPI";
}

nlohmann::json UPIPayment::toJson() const {
    nlohmann::json j = Payment::toJson();
    j["merchantUpiId"] = merchantUpiId;
    j["studentUpiId"] = studentUpiId;
    j["eventName"] = eventName;
    j["instruction"] = getPaymentInstruction();
    return j;
}
