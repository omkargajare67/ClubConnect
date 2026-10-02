#ifndef SEMINAR_H
#define SEMINAR_H

#include "events/Event.h"

// INHERITANCE: Seminar specializes Event for expert keynote / lecture presentations.
class Seminar : public Event {
private:
    std::string speaker;

public:
    Seminar(int eventId, const std::string& name, const std::string& description,
            const std::string& clubName, const std::string& date, const std::string& time,
            const std::string& venue, double fee, int totalSeats, const std::string& speaker,
            int seatsLeft = -1, const std::string& status = "PENDING");

    virtual ~Seminar() override = default;

    // ENCAPSULATION: Const getter.
    std::string getSpeaker() const;

    // POLYMORPHISM: Specialized implementations.
    nlohmann::json toJson() const override;
    void displayDetails() const override;
    std::string generateCertificate(const std::string& studentName) const override;

    // Static deserializer
    static std::shared_ptr<Seminar> fromJson(const nlohmann::json& j);
};

#endif // SEMINAR_H
