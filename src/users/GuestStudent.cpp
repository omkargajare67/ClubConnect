#include "users/GuestStudent.h"

// INHERITANCE: Initializes student fields with fixed "guest" membership status.
GuestStudent::GuestStudent(int id, const std::string& name, const std::string& email, const std::string& rawPassword,
                           const std::string& rollNo, const std::string& department, bool isHashed)
    : Student(id, name, email, rawPassword, rollNo, department, "guest", isHashed) {}

// POLYMORPHISM: Guest students pay the standard full event fee.
double GuestStudent::calculateFee(double baseFee) const {
    return baseFee;
}
