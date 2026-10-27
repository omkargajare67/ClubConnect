#ifndef SUPER_ADMIN_H
#define SUPER_ADMIN_H

#include "users/User.h"

// INHERITANCE: SuperAdmin has platform-wide authority to approve events and inspect global reports.
class SuperAdmin : public User {
public:
    SuperAdmin(int id, const std::string& name, const std::string& email, const std::string& rawPassword, bool isHashed = false);

    virtual ~SuperAdmin() override = default;

    // POLYMORPHISM: User interface overrides.
    std::string getRole() const override;
    void showMenu() const override;
    nlohmann::json toJson() const override;
};

#endif // SUPER_ADMIN_H
