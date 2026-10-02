#include "events/EventFactory.h"
#include "events/Workshop.h"
#include "events/Hackathon.h"
#include "events/Seminar.h"
#include "events/CulturalEvent.h"
#include <stdexcept>

// FACTORY METHOD: Inspects JSON "type" and instantiates the proper polymorphic subclass.
std::shared_ptr<Event> EventFactory::createEvent(const nlohmann::json& j) {
    std::string type = j.value("type", "Workshop");
    if (type == "Workshop") {
        return Workshop::fromJson(j);
    } else if (type == "Hackathon") {
        return Hackathon::fromJson(j);
    } else if (type == "Seminar") {
        return Seminar::fromJson(j);
    } else if (type == "CulturalEvent" || type == "Cultural") {
        return CulturalEvent::fromJson(j);
    }
    throw std::runtime_error("Unknown event type in factory: " + type);
}
