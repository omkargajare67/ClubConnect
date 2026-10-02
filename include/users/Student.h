#ifndef STUDENT_H
#define STUDENT_H

#include "users/User.h"

// INHERITANCE: Student derives from User, inheriting identity and authentication mechanics.
class Student : public User {
protected:
    std::string rollNo;
    std::string department;
    std::string membershipType; // "member" or "guest"

public:
    Student(int id, const std::string& name, const std::string& email, const std::string& rawPassword,
            const std::string& rollNo, const std::string& department, const std::string& membershipType = "guest");

    virtual ~Student() override = default;

    // ENCAPSULATION: Const getters for student-specific fields.
    std::string getRollNo() const;
    std::string getDepartment() const;
    std::string getMembershipType() const;

    // POLYMORPHISM: Base fee calculation; derived classes override this to apply discounts.
    virtual double calculateFee(double baseFee) const;

    // POLYMORPHISM: Concrete implementation of User pure virtual methods.
    std::string getRole() const override;
    void showMenu() const override;

    // POLYMORPHISM: Serializes student profile including membership.
    nlohmann::json toJson() const override;
};

#endif // STUDENT_H
