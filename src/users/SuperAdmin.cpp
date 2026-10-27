#include "users/SuperAdmin.h"
#include <iostream>

// INHERITANCE: Initializes top-level administrative user.
SuperAdmin::SuperAdmin(int id, const std::string& name, const std::string& email, const std::string& rawPassword, bool isHashed)
    : User(id, name, email, rawPassword, isHashed) {}

// POLYMORPHISM: SuperAdmin role string.
std::string SuperAdmin::getRole() const {
    return "SuperAdmin";
}

// POLYMORPHISM: Displays the system administrator menu.
void SuperAdmin::showMenu() const {
    std::cout << "--- Super Admin Menu ---\n"
              << "1. Review & Approve/Reject Pending Events\n"
              << "2. View Global Analytics & Revenue Reports\n";
}

// POLYMORPHISM: Base User JSON.
nlohmann::json SuperAdmin::toJson() const {
    return User::toJson();
}
