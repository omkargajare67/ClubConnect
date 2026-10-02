#include "events/EventSearch.h"
#include <algorithm>
#include <cctype>

static std::string toLower(const std::string& str) {
    std::string lower = str;
    std::transform(lower.begin(), lower.end(), lower.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return lower;
}

// FUNCTION OVERLOADING - Signature 1: Search by name/keyword
std::vector<std::shared_ptr<Event>> EventSearch::searchEvent(
    const std::vector<std::shared_ptr<Event>>& events, const std::string& name) {
    std::vector<std::shared_ptr<Event>> results;
    std::string needle = toLower(name);
    for (const auto& ev : events) {
        if (toLower(ev->getName()).find(needle) != std::string::npos ||
            toLower(ev->getDescription()).find(needle) != std::string::npos) {
            results.push_back(ev);
        }
    }
    return results;
}

// FUNCTION OVERLOADING - Signature 2: Search by Club object
std::vector<std::shared_ptr<Event>> EventSearch::searchEvent(
    const std::vector<std::shared_ptr<Event>>& events, const Club& club) {
    std::vector<std::shared_ptr<Event>> results;
    std::string clubName = toLower(club.getName());
    for (const auto& ev : events) {
        if (toLower(ev->getClubName()) == clubName) {
            results.push_back(ev);
        }
    }
    return results;
}

// FUNCTION OVERLOADING - Signature 3: Search by maximum fee limit
std::vector<std::shared_ptr<Event>> EventSearch::searchEvent(
    const std::vector<std::shared_ptr<Event>>& events, double maxFee) {
    std::vector<std::shared_ptr<Event>> results;
    for (const auto& ev : events) {
        if (ev->getFee() <= maxFee) {
            results.push_back(ev);
        }
    }
    return results;
}
