#include "records/Club.h"

Club::Club(int clubId, const std::string& name, const std::string& description, const std::string& leadAdminEmail)
    : clubId(clubId), name(name), description(description), leadAdminEmail(leadAdminEmail) {}

int Club::getClubId() const { return clubId; }
std::string Club::getName() const { return name; }
std::string Club::getDescription() const { return description; }
std::string Club::getLeadAdminEmail() const { return leadAdminEmail; }

nlohmann::json Club::toJson() const {
    return {
        {"clubId", clubId},
        {"name", name},
        {"description", description},
        {"leadAdminEmail", leadAdminEmail}
    };
}

Club Club::fromJson(const nlohmann::json& j) {
    return Club(
        j.value("clubId", 0),
        j.value("name", ""),
        j.value("description", ""),
        j.value("leadAdminEmail", "")
    );
}
