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
    std::string password = j.value("password", "pass123");
    std::string role = j.value("role", "Student");

    if (role == "Student") {
        std::string rollNo = j.value("rollNo", "STU" + std::to_string(id));
        std::string dept = j.value("department", "Engineering");
        std::string membership = j.value("membership", "guest");

        if (membership == "member") {
            return std::make_shared<MemberStudent>(id, name, email, password, rollNo, dept);
        } else {
            return std::make_shared<GuestStudent>(id, name, email, password, rollNo, dept);
        }
    } else if (role == "ClubAdmin") {
        std::string club = j.value("clubName", "Coding Club");
        return std::make_shared<ClubAdmin>(id, name, email, password, club);
    } else if (role == "SuperAdmin") {
        return std::make_shared<SuperAdmin>(id, name, email, password);
    }

    throw std::runtime_error("Unknown user role in factory: " + role);
}
