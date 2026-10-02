#include "users/MemberStudent.h"

// INHERITANCE: Initializes student fields with fixed "member" membership status.
MemberStudent::MemberStudent(int id, const std::string& name, const std::string& email, const std::string& rawPassword,
                             const std::string& rollNo, const std::string& department)
    : Student(id, name, email, rawPassword, rollNo, department, "member") {}

// POLYMORPHISM: Applies a 20% discount on any club event base fee.
double MemberStudent::calculateFee(double baseFee) const {
    return baseFee * 0.80;
}
