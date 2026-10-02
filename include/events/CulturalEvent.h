#ifndef CULTURAL_EVENT_H
#define CULTURAL_EVENT_H

#include "events/Event.h"

// INHERITANCE: CulturalEvent specializes Event for performing arts, music, dance, and cultural festivals.
class CulturalEvent : public Event {
private:
    std::string category;

public:
    CulturalEvent(int eventId, const std::string& name, const std::string& description,
                  const std::string& clubName, const std::string& date, const std::string& time,
                  const std::string& venue, double fee, int totalSeats, const std::string& category,
                  int seatsLeft = -1, const std::string& status = "PENDING");

    virtual ~CulturalEvent() override = default;

    // ENCAPSULATION: Const getter.
    std::string getCategory() const;

    // POLYMORPHISM: Specialized implementations.
    nlohmann::json toJson() const override;
    void displayDetails() const override;
    std::string generateCertificate(const std::string& studentName) const override;

    // Static deserializer
    static std::shared_ptr<CulturalEvent> fromJson(const nlohmann::json& j);
};

#endif // CULTURAL_EVENT_H
