#ifndef PAYMENT_SERVICE_H
#define PAYMENT_SERVICE_H

#include <memory>
#include <string>
#include "json.hpp"
#include "core/Repository.h"
#include "payments/Payment.h"
#include "records/Receipt.h"
#include "records/Enrollment.h"
#include "events/Event.h"
#include "users/User.h"

// SERVICE LAYER & FACTORY PATTERN: Handles multi-channel payment lifecycle and immutable receipt issuance.
class PaymentService {
public:
    static int nextTxnCounter;

private:
    std::shared_ptr<Repository<Payment>> paymentRepo;
    std::shared_ptr<Repository<Receipt>> receiptRepo;
    std::shared_ptr<Repository<Enrollment>> enrollmentRepo;
    std::shared_ptr<Repository<Event>> eventRepo;
    std::shared_ptr<Repository<User>> userRepo;

public:
    PaymentService(std::shared_ptr<Repository<Payment>> paymentRepo,
                   std::shared_ptr<Repository<Receipt>> receiptRepo,
                   std::shared_ptr<Repository<Enrollment>> enrollmentRepo,
                   std::shared_ptr<Repository<Event>> eventRepo,
                   std::shared_ptr<Repository<User>> userRepo);

    // FACTORY METHOD & POLYMORPHISM: Instantiates appropriate Payment subclass and initiates transaction.
    // Handles free event exemption directly.
    std::shared_ptr<Payment> initiatePayment(int enrollmentId, const std::string& mode, const nlohmann::json& details);

    // Confirms payment, triggers processPayment(), and generates immutable audit Receipt.
    std::shared_ptr<Receipt> confirmPayment(const std::string& paymentId);

    // Queries receipt by ID
    std::shared_ptr<Receipt> getReceiptByEnrollmentId(int enrollmentId) const;
    std::shared_ptr<Receipt> getReceiptById(int receiptNo) const;
};

#endif // PAYMENT_SERVICE_H
