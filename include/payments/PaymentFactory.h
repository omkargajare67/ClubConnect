#ifndef PAYMENT_FACTORY_H
#define PAYMENT_FACTORY_H

#include <memory>
#include "json.hpp"
#include "payments/Payment.h"

// FACTORY PATTERN: Reconstructs Payment subclasses from persistent JSON records.
class PaymentFactory {
public:
    static std::shared_ptr<Payment> createPayment(const nlohmann::json& j);
};

#endif // PAYMENT_FACTORY_H
