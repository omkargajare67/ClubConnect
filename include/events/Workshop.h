#ifndef WORKSHOP_H
#define WORKSHOP_H

#include "events/Event.h"

// INHERITANCE: Workshop specializes Event with instructor/hands-on training specifics.
class Workshop : public Event {
private:
    std::string trainer;

public:
    Workshop(int eventId, const std::string& name, const std::string& description,
             const std::string& clubName, const std::string& date, const std::string& time,
             const std::string& venue, double fee, int totalSeats, const std::string& trainer,
             int seatsLeft = -1, const std::string& status = "PENDING");

    virtual ~Workshop() override = default;

    // ENCAPSULATION: Const getter.
    std::string getTrainer() const;

    // POLYMORPHISM: Specialized implementations of pure virtual methods.
    nlohmann::json toJson() const override;
    void displayDetails() const override;
    std::string generateCertificate(const std::string& studentName) const override;

    // Static deserializer
    static std::shared_ptr<Workshop> fromJson(const nlohmann::json& j);
};

#endif // WORKSHOP_H
