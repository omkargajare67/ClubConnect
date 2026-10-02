#ifndef EVENT_FACTORY_H
#define EVENT_FACTORY_H

#include <memory>
#include "json.hpp"
#include "events/Event.h"

// FACTORY PATTERN: Encapsulates instantiation logic of Event derived classes.
// Dynamically creates Workshop, Hackathon, Seminar, or CulturalEvent based on JSON "type" discriminator.
class EventFactory {
public:
    static std::shared_ptr<Event> createEvent(const nlohmann::json& j);
};

#endif // EVENT_FACTORY_H
