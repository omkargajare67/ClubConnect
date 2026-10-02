#ifndef GUEST_STUDENT_H
#define GUEST_STUDENT_H

#include "users/Student.h"

// INHERITANCE: GuestStudent represents external or non-member students paying standard rates.
class GuestStudent : public Student {
public:
    GuestStudent(int id, const std::string& name, const std::string& email, const std::string& rawPassword,
                 const std::string& rollNo, const std::string& department);

    virtual ~GuestStudent() override = default;

    // POLYMORPHISM: Guest students pay the full 100% base fee without discount.
    double calculateFee(double baseFee) const override;
};

#endif // GUEST_STUDENT_H
