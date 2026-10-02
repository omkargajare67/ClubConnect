#include "records/Team.h"

Team::Team() : teamName("Unnamed Squad") {}

Team::Team(const std::string& teamName, const std::vector<int>& ids, const std::vector<std::string>& names)
    : teamName(teamName), studentIds(ids), memberNames(names) {}

std::string Team::getTeamName() const { return teamName; }
const std::vector<int>& Team::getStudentIds() const { return studentIds; }
const std::vector<std::string>& Team::getMemberNames() const { return memberNames; }

// OPERATOR OVERLOADING: Appends a student to this team.
Team Team::operator+(const Student& student) const {
    std::vector<int> newIds = studentIds;
    std::vector<std::string> newNames = memberNames;

    newIds.push_back(student.getId());
    newNames.push_back(student.getName());

    return Team(teamName, newIds, newNames);
}

// OPERATOR OVERLOADING: Creates a merged two-person team from two students.
Team operator+(const Student& s1, const Student& s2) {
    std::string mergedName = "Team " + s1.getName() + " & " + s2.getName();
    std::vector<int> ids = { s1.getId(), s2.getId() };
    std::vector<std::string> names = { s1.getName(), s2.getName() };
    return Team(mergedName, ids, names);
}

nlohmann::json Team::toJson() const {
    return {
        {"teamName", teamName},
        {"studentIds", studentIds},
        {"memberNames", memberNames}
    };
}

Team Team::fromJson(const nlohmann::json& j) {
    std::vector<int> ids = j.value("studentIds", std::vector<int>{});
    std::vector<std::string> names = j.value("memberNames", std::vector<std::string>{});
    std::string name = j.value("teamName", "Team");
    return Team(name, ids, names);
}
