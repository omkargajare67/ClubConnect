#include "records/Receipt.h"
#include <iomanip>
#include <sstream>

int Receipt::nextReceiptNo = 1001;

Receipt::Receipt(int receiptNo, int enrollmentId, int studentId, int eventId,
                 double amountPaid, const std::string& paymentMethod,
                 const std::string& transactionId, const std::string& timestamp,
                 const std::string& receiptCode)
    : receiptNo(receiptNo), enrollmentId(enrollmentId), studentId(studentId), eventId(eventId),
      amountPaid(amountPaid), paymentMethod(paymentMethod), transactionId(transactionId),
      timestamp(timestamp) {
    if (receiptCode.empty()) {
        std::ostringstream oss;
        oss << "CC-RCPT-" << receiptNo;
        this->receiptCode = oss.str();
    } else {
        this->receiptCode = receiptCode;
    }
}

int Receipt::getReceiptNo() const { return receiptNo; }
std::string Receipt::getReceiptCode() const { return receiptCode; }
int Receipt::getEnrollmentId() const { return enrollmentId; }
int Receipt::getStudentId() const { return studentId; }
int Receipt::getEventId() const { return eventId; }
double Receipt::getAmountPaid() const { return amountPaid; }
std::string Receipt::getPaymentMethod() const { return paymentMethod; }
std::string Receipt::getTransactionId() const { return transactionId; }
std::string Receipt::getTimestamp() const { return timestamp; }

// OPERATOR OVERLOADING: Standard stream representation.
std::ostream& operator<<(std::ostream& os, const Receipt& receipt) {
    os << "Receipt [" << receipt.receiptCode << "] | Enrollment #" << receipt.enrollmentId
       << " | Student #" << receipt.studentId << " | Event #" << receipt.eventId
       << " | Paid: INR " << std::fixed << std::setprecision(2) << receipt.amountPaid
       << " via " << receipt.paymentMethod << " (" << receipt.timestamp << ")";
    return os;
}

// FRIEND FUNCTION: Accesses private properties of Receipt and Payment to render an official receipt voucher.
void printReceiptWithPayment(std::ostream& os, const Receipt& receipt, const Payment& payment) {
    os << "========================================================\n"
       << "                 OFFICIAL PAYMENT RECEIPT               \n"
       << "                   ClubConnect Portal                   \n"
       << "========================================================\n"
       << "Receipt Code  : " << receipt.receiptCode << "\n"
       << "Enrollment ID : #" << receipt.enrollmentId << "\n"
       << "Student ID    : #" << receipt.studentId << "\n"
       << "Event ID      : #" << receipt.eventId << "\n"
       << "Amount Paid   : INR " << std::fixed << std::setprecision(2) << receipt.amountPaid << "\n"
       << "Payment Method: " << receipt.paymentMethod << "\n"
       << "Txn Identifier: " << receipt.transactionId << "\n"
       << "Payment Status: " << payment.getStatus() << "\n"
       << "Payment Date  : " << payment.getDate() << "\n"
       << "Generated At  : " << receipt.timestamp << "\n"
       << "QR Code Data  : " << receipt.receiptCode << "\n"
       << "========================================================\n";
}

nlohmann::json Receipt::toJson() const {
    return {
        {"receiptNo", receiptNo},
        {"receiptCode", receiptCode},
        {"enrollmentId", enrollmentId},
        {"studentId", studentId},
        {"eventId", eventId},
        {"amountPaid", amountPaid},
        {"paymentMethod", paymentMethod},
        {"transactionId", transactionId},
        {"timestamp", timestamp}
    };
}

Receipt Receipt::fromJson(const nlohmann::json& j) {
    return Receipt(
        j.value("receiptNo", 0),
        j.value("enrollmentId", 0),
        j.value("studentId", 0),
        j.value("eventId", 0),
        j.value("amountPaid", 0.0),
        j.value("paymentMethod", ""),
        j.value("transactionId", ""),
        j.value("timestamp", ""),
        j.value("receiptCode", "")
    );
}
