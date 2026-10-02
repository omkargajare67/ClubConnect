#ifndef RECEIPT_H
#define RECEIPT_H

#include <string>
#include <iostream>
#include "json.hpp"
#include "payments/Payment.h"

// ENCAPSULATION & FRIEND FUNCTION: Receipt records a successful payment transaction.
// All fields are strictly private and read-only once constructed.
// A friend function is granted privileged access to print comprehensive receipt & payment audit details.
class Receipt {
public:
    static int nextReceiptNo;

private:
    int receiptNo;
    std::string receiptCode; // e.g. "CC-RCPT-1001"
    int enrollmentId;
    int studentId;
    int eventId;
    double amountPaid;
    std::string paymentMethod;
    std::string transactionId;
    std::string timestamp;

public:
    Receipt(int receiptNo, int enrollmentId, int studentId, int eventId,
            double amountPaid, const std::string& paymentMethod,
            const std::string& transactionId, const std::string& timestamp,
            const std::string& receiptCode = "");

    // ENCAPSULATION: Read-only accessors maintaining integrity of financial audit records.
    int getReceiptNo() const;
    std::string getReceiptCode() const;
    int getEnrollmentId() const;
    int getStudentId() const;
    int getEventId() const;
    double getAmountPaid() const;
    std::string getPaymentMethod() const;
    std::string getTransactionId() const;
    std::string getTimestamp() const;

    // OPERATOR OVERLOADING: Stream output for receipt summary.
    friend std::ostream& operator<<(std::ostream& os, const Receipt& receipt);

    // FRIEND FUNCTION: Accesses private state of Receipt and Payment to print combined audit voucher.
    friend void printReceiptWithPayment(std::ostream& os, const Receipt& receipt, const Payment& payment);

    nlohmann::json toJson() const;
    static Receipt fromJson(const nlohmann::json& j);
};

#endif // RECEIPT_H
