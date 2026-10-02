#include "events/CulturalEvent.h"
#include <iostream>
#include <sstream>

CulturalEvent::CulturalEvent(int eventId, const std::string& name, const std::string& description,
                             const std::string& clubName, const std::string& date, const std::string& time,
                             const std::string& venue, double fee, int totalSeats, const std::string& category,
                             int seatsLeft, const std::string& status)
    : Event(eventId, name, description, clubName, date, time, venue, fee, totalSeats, seatsLeft, status),
      category(category) {}

std::string CulturalEvent::getCategory() const {
    return category;
}

// POLYMORPHISM: Serializes cultural event details with artistic genre/category.
nlohmann::json CulturalEvent::toJson() const {
    return {
        {"eventId", eventId},
        {"type", "CulturalEvent"},
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
        {"category", category}
    };
}

// POLYMORPHISM: Console detail view for cultural event.
void CulturalEvent::displayDetails() const {
    std::cout << "[CULTURAL EVENT] " << name << " | Category: " << category << "\n"
              << "  Club: " << clubName << " | Date: " << date << " " << time
              << " | Venue: " << venue << " | Seats Left: " << getSeatsLeft() << "/" << totalSeats
              << " | Fee: INR " << fee << "\n"
              << "  Description: " << description << "\n";
}

// POLYMORPHISM: Certificate for cultural event participation.
std::string CulturalEvent::generateCertificate(const std::string& studentName) const {
    std::ostringstream oss;
    oss << "========================================================\n"
        << "           CERTIFICATE OF PARTICIPATION                 \n"
        << "========================================================\n"
        << "This certifies that:\n\n"
        << "                " << studentName << "\n\n"
        << "has actively performed in the cultural festival showcase:\n"
        << "       \"" << name << "\" (" << category << ")\n"
        << "organized by " << clubName << " on " << date << " at " << venue << ".\n"
        << "========================================================\n";
    return oss.str();
}

std::shared_ptr<CulturalEvent> CulturalEvent::fromJson(const nlohmann::json& j) {
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
    std::string category = j.value("category", "");

    return std::make_shared<CulturalEvent>(id, name, desc, club, date, time, venue, fee, totalSeats, category, seatsLeft, status);
}
