#ifndef CLUB_ADMIN_H
#define CLUB_ADMIN_H

#include "users/User.h"

// INHERITANCE: ClubAdmin derives from User and manages events and attendance for a specific club.
class ClubAdmin : public User {
private:
    std::string clubName;

public:
    ClubAdmin(int id, const std::string& name, const std::string& email, const std::string& rawPassword,
              const std::string& clubName);

    virtual ~ClubAdmin() override = default;

    // ENCAPSULATION: Read-only accessor for assigned club.
    std::string getClubName() const;

    // POLYMORPHISM: User interface overrides.
    std::string getRole() const override;
    void showMenu() const override;
    nlohmann::json toJson() const override;
};

#endif // CLUB_ADMIN_H
