#include "users/Student.h"
#include <iostream>

// INHERITANCE: Calls User base constructor to initialize shared user fields.
Student::Student(int id, const std::string& name, const std::string& email, const std::string& rawPassword,
                 const std::string& rollNo, const std::string& department, const std::string& membershipType)
    : User(id, name, email, rawPassword), rollNo(rollNo), department(department), membershipType(membershipType) {}

// ENCAPSULATION: Read-only accessors for private student attributes.
std::string Student::getRollNo() const { return rollNo; }
std::string Student::getDepartment() const { return department; }
std::string Student::getMembershipType() const { return membershipType; }

// POLYMORPHISM: Default student fee calculation returns base fee unmodified.
double Student::calculateFee(double baseFee) const {
    return baseFee;
}

// POLYMORPHISM: Returns role identifier for Student.
std::string Student::getRole() const {
    return "Student";
}

// POLYMORPHISM: Displays the student-specific action menu.
void Student::showMenu() const {
    std::cout << "--- Student Menu (" << name << ") ---\n"
              << "1. Browse & Search Events\n"
              << "2. Enroll in Event\n"
              << "3. View My Enrollments & Receipts\n"
              << "4. Download Certificate\n";
}

// POLYMORPHISM: Extends User base JSON with academic attributes.
nlohmann::json Student::toJson() const {
    nlohmann::json j = User::toJson();
    j["rollNo"] = rollNo;
    j["department"] = department;
    j["membership"] = membershipType;
    return j;
}
