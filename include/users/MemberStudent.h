#ifndef MEMBER_STUDENT_H
#define MEMBER_STUDENT_H

#include "users/Student.h"

// INHERITANCE: MemberStudent represents enrolled club members who receive special discounts.
class MemberStudent : public Student {
public:
    MemberStudent(int id, const std::string& name, const std::string& email, const std::string& rawPassword,
                  const std::string& rollNo, const std::string& department, bool isHashed = false);

    virtual ~MemberStudent() override = default;

    // POLYMORPHISM: Overrides fee calculation to apply a 20% club membership discount.
    double calculateFee(double baseFee) const override;
};

#endif // MEMBER_STUDENT_H
