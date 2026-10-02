#ifndef HACKATHON_H
#define HACKATHON_H

#include "events/Event.h"

// INHERITANCE: Hackathon specializes Event for competitive software sprint events.
class Hackathon : public Event {
private:
    int teamSize;
    std::string prize;

public:
    Hackathon(int eventId, const std::string& name, const std::string& description,
              const std::string& clubName, const std::string& date, const std::string& time,
              const std::string& venue, double fee, int totalSeats, int teamSize, const std::string& prize,
              int seatsLeft = -1, const std::string& status = "PENDING");

    virtual ~Hackathon() override = default;

    // ENCAPSULATION: Const getters.
    int getTeamSize() const;
    std::string getPrize() const;

    // POLYMORPHISM: Specialized implementations.
    nlohmann::json toJson() const override;
    void displayDetails() const override;
    std::string generateCertificate(const std::string& studentName) const override;

    // Static deserializer
    static std::shared_ptr<Hackathon> fromJson(const nlohmann::json& j);
};

#endif // HACKATHON_H
