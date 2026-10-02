#ifndef EVENT_SEARCH_H
#define EVENT_SEARCH_H

#include <vector>
#include <memory>
#include <string>
#include "events/Event.h"
#include "records/Club.h"

// FUNCTION OVERLOADING: Polymorphic static search facilities providing distinct query signatures
// for students searching events by title keyword, hosting club entity, or budget ceiling.
class EventSearch {
public:
    // FUNCTION OVERLOADING - Signature 1: Search by title substring
    static std::vector<std::shared_ptr<Event>> searchEvent(
        const std::vector<std::shared_ptr<Event>>& events, const std::string& name);

    // FUNCTION OVERLOADING - Signature 2: Search by Club object
    static std::vector<std::shared_ptr<Event>> searchEvent(
        const std::vector<std::shared_ptr<Event>>& events, const Club& club);

    // FUNCTION OVERLOADING - Signature 3: Search by maximum registration fee
    static std::vector<std::shared_ptr<Event>> searchEvent(
        const std::vector<std::shared_ptr<Event>>& events, double maxFee);
};

#endif // EVENT_SEARCH_H
