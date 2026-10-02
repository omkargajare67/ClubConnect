#include "events/Workshop.h"
#include <iostream>
#include <sstream>

Workshop::Workshop(int eventId, const std::string& name, const std::string& description,
                   const std::string& clubName, const std::string& date, const std::string& time,
                   const std::string& venue, double fee, int totalSeats, const std::string& trainer,
                   int seatsLeft, const std::string& status)
    : Event(eventId, name, description, clubName, date, time, venue, fee, totalSeats, seatsLeft, status),
      trainer(trainer) {}

std::string Workshop::getTrainer() const {
    return trainer;
}

// POLYMORPHISM: Serializes workshop-specific attributes alongside base fields.
nlohmann::json Workshop::toJson() const {
    return {
        {"eventId", eventId},
        {"type", "Workshop"},
        {"name", name},
        {"description", description},
        {"clubName", clubName},
        {"date", date},
        {"time", time},
        {"venue", venue},
        {"fee", fee},
        {"totalSeats", totalSeats},
        {"seatsLeft", getSeatsLeft()},
        {"status", status},
        {"trainer", trainer}
    };
}

// POLYMORPHISM: Renders console description of workshop with trainer name.
void Workshop::displayDetails() const {
    std::cout << "[WORKSHOP] " << name << " | Conducted by: " << trainer << "\n"
              << "  Club: " << clubName << " | Date: " << date << " " << time
              << " | Venue: " << venue << " | Seats Left: " << getSeatsLeft() << "/" << totalSeats
              << " | Fee: INR " << fee << "\n"
              << "  Description: " << description << "\n";
}

// POLYMORPHISM: Formats certificate of participation for workshop attendees.
std::string Workshop::generateCertificate(const std::string& studentName) const {
    std::ostringstream oss;
    oss << "========================================================\n"
        << "           CERTIFICATE OF PARTICIPATION                 \n"
        << "========================================================\n"
        << "This certifies that:\n\n"
        << "                " << studentName << "\n\n"
        << "has actively participated in the technical workshop:\n"
        << "       \"" << name << "\"\n"
        << "conducted by Trainer: " << trainer << "\n"
        << "organized by " << clubName << " on " << date << " at " << venue << ".\n"
        << "========================================================\n";
    return oss.str();
}

std::shared_ptr<Workshop> Workshop::fromJson(const nlohmann::json& j) {
    int id = j.value("eventId", 0);
    std::string name = j.value("name", "");
    std::string desc = j.value("description", "");
    std::string club = j.value("clubName", "");
    std::string date = j.value("date", "");
    std::string time = j.value("time", "");
    std::string venue = j.value("venue", "");
    double fee = j.value("fee", 0.0);
    int totalSeats = j.value("totalSeats", 0);
    int seatsLeft = j.value("seatsLeft", totalSeats);
    std::string status = j.value("status", "PENDING");
    std::string trainer = j.value("trainer", "");

    return std::make_shared<Workshop>(id, name, desc, club, date, time, venue, fee, totalSeats, trainer, seatsLeft, status);
}
