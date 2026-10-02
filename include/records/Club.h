#ifndef CLUB_H
#define CLUB_H

#include <string>
#include "json.hpp"

// ENCAPSULATION: Models a recognized university student organization.
class Club {
private:
    int clubId;
    std::string name;
    std::string description;
    std::string leadAdminEmail;

public:
    Club(int clubId, const std::string& name, const std::string& description, const std::string& leadAdminEmail);

    // ENCAPSULATION: Read-only const getters.
    int getClubId() const;
    std::string getName() const;
    std::string getDescription() const;
    std::string getLeadAdminEmail() const;

    nlohmann::json toJson() const;
    static Club fromJson(const nlohmann::json& j);
};

#endif // CLUB_H
