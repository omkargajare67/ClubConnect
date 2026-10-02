#ifndef TEAM_H
#define TEAM_H

#include <string>
#include <vector>
#include "json.hpp"
#include "users/Student.h"

// OPERATOR OVERLOADING & AGGREGATION: Models a student hackathon squad.
// Demonstrates operator+ overloading to merge two individual students into a collaborative Team.
class Team {
private:
    std::string teamName;
    std::vector<int> studentIds;
    std::vector<std::string> memberNames;

public:
    Team();
    Team(const std::string& teamName, const std::vector<int>& ids, const std::vector<std::string>& names);

    // ENCAPSULATION: Read-only accessors.
    std::string getTeamName() const;
    const std::vector<int>& getStudentIds() const;
    const std::vector<std::string>& getMemberNames() const;

    // OPERATOR OVERLOADING: Merges another student into this existing team.
    Team operator+(const Student& student) const;

    // OPERATOR OVERLOADING: Friend operator merging two individual students into a newly formed Team.
    friend Team operator+(const Student& s1, const Student& s2);

    nlohmann::json toJson() const;
    static Team fromJson(const nlohmann::json& j);
};

// Declared at namespace scope so lookup succeeds with Student arguments
Team operator+(const Student& s1, const Student& s2);

#endif // TEAM_H
