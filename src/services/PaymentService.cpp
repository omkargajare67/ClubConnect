#include "services/PaymentService.h"
#include "payments/UPIPayment.h"
#include "payments/CardPayment.h"
#include "payments/CashPayment.h"
#include "users/Student.h"
#include "core/Exceptions.h"
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

int PaymentService::nextTxnCounter = 5001;

PaymentService::PaymentService(std::shared_ptr<Repository<Payment>> paymentRepo,
                               std::shared_ptr<Repository<Receipt>> receiptRepo,
                               std::shared_ptr<Repository<Enrollment>> enrollmentRepo,
                               std::shared_ptr<Repository<Event>> eventRepo,
                               std::shared_ptr<Repository<User>> userRepo)
    : paymentRepo(paymentRepo), receiptRepo(receiptRepo),
      enrollmentRepo(enrollmentRepo), eventRepo(eventRepo), userRepo(userRepo) {}

// FACTORY PATTERN & POLYMORPHISM:
// Creates appropriate Payment derivative based on mode parameter.
std::shared_ptr<Payment> PaymentService::initiatePayment(int enrollmentId, const std::string& mode, const nlohmann::json& details) {
    auto enrollment = enrollmentRepo->find([enrollmentId](const std::shared_ptr<Enrollment>& en) {
        return en->getEnrollmentId() == enrollmentId;
    });
    if (!enrollment) {
        throw std::runtime_error("Enrollment #" + std::to_string(enrollmentId) + " not found.");
    }

    auto event = eventRepo->find([enrollment](const std::shared_ptr<Event>& e) {
        return e->getEventId() == enrollment->getEventId();
    });
    if (!event) {
        throw std::runtime_error("Enrolled event not found.");
    }

    auto user = userRepo->find([enrollment](const std::shared_ptr<User>& u) {
        return u->getId() == enrollment->getStudentId();
    });
    if (!user) {
        throw std::runtime_error("Student record not found.");
    }

    // Dynamic fee calculation based on Student type (MemberStudent vs GuestStudent)
    double baseFee = event->getFee();
    double finalAmount = baseFee;
    auto student = std::dynamic_pointer_cast<Student>(user);
    if (student) {
        finalAmount = student->calculateFee(baseFee);
    }

    // Generate transaction ID & date
    std::string txnId = "TXN-" + std::to_string(nextTxnCounter++);
    auto now = std::chrono::system_clock::now();
    std::time_t nowTime = std::chrono::system_clock::to_time_t(now);
    std::tm ltm;
#if defined(_WIN32) || defined(_WIN64)
    localtime_s(&ltm, &nowTime);
#else
    localtime_r(&nowTime, &ltm);
#endif
    std::ostringstream dateStream;
    dateStream << std::put_time(&ltm, "%Y-%m-%d %H:%M:%S");
    std::string dateStr = dateStream.str();

    // BUSINESS RULE: Free events (fee 0) skip payment gateway and generate immediate SUCCESS receipt
    if (finalAmount <= 0.0001) {
        auto freePayment = std::make_shared<CashPayment>(0.0, txnId, dateStr, "System Auto-Exempt (Free Event)", "SUCCESS");
        paymentRepo->add(freePayment);
        enrollment->setPaymentId(txnId);

        // Auto-generate free receipt
        int rcptNo = Receipt::nextReceiptNo++;
        auto receipt = std::make_shared<Receipt>(
            rcptNo, enrollmentId, enrollment->getStudentId(), event->getEventId(),
            0.0, "FREE", txnId, dateStr
        );
        receiptRepo->add(receipt);
        return freePayment;
    }

    // Create payment by payment mode
    std::shared_ptr<Payment> payment = nullptr;
    if (mode == "UPI") {
        std::string studentUpi = details.value("studentUpiId", "student@upi");
        payment = std::make_shared<UPIPayment>(finalAmount, txnId, dateStr, event->getName(), studentUpi);
    } else if (mode == "Card") {
        std::string cardNum = details.value("cardNumber", "");
        std::string holderName = details.value("cardHolderName", user->getName());
        payment = std::make_shared<CardPayment>(finalAmount, txnId, dateStr, cardNum, holderName);
    } else if (mode == "Cash") {
        std::string counter = details.value("counterName", "Student Affairs Helpdesk (Room 102)");
        payment = std::make_shared<CashPayment>(finalAmount, txnId, dateStr, counter);
    } else {
        throw std::runtime_error("Unsupported payment mode: " + mode);
    }

    paymentRepo->add(payment);
    enrollment->setPaymentId(txnId);
    return payment;
}

// POLYMORPHISM & ENCAPSULATION:
// Calls processPayment() polymorphic method and creates immutable Receipt upon success.
std::shared_ptr<Receipt> PaymentService::confirmPayment(const std::string& paymentId) {
    auto payment = paymentRepo->find([paymentId](const std::shared_ptr<Payment>& p) {
        return p->getTransactionId() == paymentId;
    });
    if (!payment) {
        throw PaymentFailedException("Transaction ID '" + paymentId + "' not found.");
    }

    // If already successful, return existing receipt
    auto existingReceipt = receiptRepo->find([paymentId](const std::shared_ptr<Receipt>& r) {
        return r->getTransactionId() == paymentId;
    });
    if (existingReceipt) {
        return existingReceipt;
    }

    bool success = payment->processPayment();
    if (!success) {
        throw PaymentFailedException("Transaction confirmation failed.");
    }

    auto enrollment = enrollmentRepo->find([paymentId](const std::shared_ptr<Enrollment>& en) {
        return en->getPaymentId() == paymentId;
    });
    if (!enrollment) {
        throw std::runtime_error("Associated enrollment record not found for transaction " + paymentId);
    }

    auto now = std::chrono::system_clock::now();
    std::time_t nowTime = std::chrono::system_clock::to_time_t(now);
    std::tm ltm;
#if defined(_WIN32) || defined(_WIN64)
    localtime_s(&ltm, &nowTime);
#else
    localtime_r(&nowTime, &ltm);
#endif
    std::ostringstream dateStream;
    dateStream << std::put_time(&ltm, "%Y-%m-%d %H:%M:%S");

    int rcptNo = Receipt::nextReceiptNo++;
    auto receipt = std::make_shared<Receipt>(
        rcptNo, enrollment->getEnrollmentId(), enrollment->getStudentId(), enrollment->getEventId(),
        payment->getAmount(), payment->getPaymentMethod(), payment->getTransactionId(), dateStream.str()
    );
    receiptRepo->add(receipt);

    return receipt;
}

std::shared_ptr<Receipt> PaymentService::getReceiptByEnrollmentId(int enrollmentId) const {
    return receiptRepo->find([enrollmentId](const std::shared_ptr<Receipt>& r) {
        return r->getEnrollmentId() == enrollmentId;
    });
}

std::shared_ptr<Receipt> PaymentService::getReceiptById(int receiptNo) const {
    return receiptRepo->find([receiptNo](const std::shared_ptr<Receipt>& r) {
        return r->getReceiptNo() == receiptNo;
    });
}
