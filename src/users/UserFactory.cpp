#include "users/UserFactory.h"
#include "users/MemberStudent.h"
#include "users/GuestStudent.h"
#include "users/ClubAdmin.h"
#include "users/SuperAdmin.h"
#include <stdexcept>

// FACTORY METHOD: Rebuilds concrete User hierarchy objects based on role and membership fields.
std::shared_ptr<User> UserFactory::createUser(const nlohmann::json& j) {
    int id = j.value("id", 0);
    std::string name = j.value("name", "");
    std::string email = j.value("email", "");
    std::string password = j.value("password", "");
    if (password.empty()) {
        throw std::runtime_error("Password is required for user account.");
    }
    std::string role = j.value("role", "Student");

    bool isHashed = j.value("isHashed", false);
    if (!isHashed && !j.value("isRawPassword", false)) {
        if (password.length() == 16) {
            bool allHex = true;
            for (char c : password) {
                if (!std::isxdigit(static_cast<unsigned char>(c))) {
                    allHex = false;
                    break;
                }
            }
            if (allHex) isHashed = true;
        }
    }

    if (role == "Student") {
        std::string rollNo = j.value("rollNo", "STU" + std::to_string(id));
        std::string dept = j.value("department", "Engineering");
        std::string membership = j.value("membership", "guest");

        if (membership == "member") {
            return std::make_shared<MemberStudent>(id, name, email, password, rollNo, dept, isHashed);
        } else {
            return std::make_shared<GuestStudent>(id, name, email, password, rollNo, dept, isHashed);
        }
    } else if (role == "ClubAdmin") {
        std::string club = j.value("clubName", "Coding Club");
        return std::make_shared<ClubAdmin>(id, name, email, password, club, isHashed);
    } else if (role == "SuperAdmin") {
        return std::make_shared<SuperAdmin>(id, name, email, password, isHashed);
    }

    throw std::runtime_error("Unknown user role in factory: " + role);
}
