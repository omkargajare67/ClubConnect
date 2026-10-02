#include "users/ClubAdmin.h"
#include <iostream>

// INHERITANCE: Initializes base User properties alongside club assignment.
ClubAdmin::ClubAdmin(int id, const std::string& name, const std::string& email, const std::string& rawPassword,
                     const std::string& clubName)
    : User(id, name, email, rawPassword), clubName(clubName) {}

// ENCAPSULATION: Const getter for club affiliation.
std::string ClubAdmin::getClubName() const {
    return clubName;
}

// POLYMORPHISM: ClubAdmin role string.
std::string ClubAdmin::getRole() const {
    return "ClubAdmin";
}

// POLYMORPHISM: Displays the club organizer action menu.
void ClubAdmin::showMenu() const {
    std::cout << "--- Club Admin Menu (" << clubName << ") ---\n"
              << "1. Create New Event (Pending SuperAdmin Approval)\n"
              << "2. View Club Events\n"
              << "3. Mark Attendance for Event\n";
}

// POLYMORPHISM: Adds club affiliation to JSON representation.
nlohmann::json ClubAdmin::toJson() const {
    nlohmann::json j = User::toJson();
    j["clubName"] = clubName;
    return j;
}
