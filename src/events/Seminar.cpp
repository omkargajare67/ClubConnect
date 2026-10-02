#include "events/Seminar.h"
#include <iostream>
#include <sstream>

Seminar::Seminar(int eventId, const std::string& name, const std::string& description,
                 const std::string& clubName, const std::string& date, const std::string& time,
                 const std::string& venue, double fee, int totalSeats, const std::string& speaker,
                 int seatsLeft, const std::string& status)
    : Event(eventId, name, description, clubName, date, time, venue, fee, totalSeats, seatsLeft, status),
      speaker(speaker) {}

std::string Seminar::getSpeaker() const {
    return speaker;
}

// POLYMORPHISM: Serializes seminar attributes including keynote speaker.
nlohmann::json Seminar::toJson() const {
    return {
        {"eventId", eventId},
        {"type", "Seminar"},
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
        {"speaker", speaker}
    };
}

// POLYMORPHISM: Console detail view for seminar.
void Seminar::displayDetails() const {
    std::cout << "[SEMINAR] " << name << " | Distinguished Speaker: " << speaker << "\n"
              << "  Club: " << clubName << " | Date: " << date << " " << time
              << " | Venue: " << venue << " | Seats Left: " << getSeatsLeft() << "/" << totalSeats
              << " | Fee: INR " << fee << "\n"
              << "  Description: " << description << "\n";
}

// POLYMORPHISM: Certificate for seminar attendance.
std::string Seminar::generateCertificate(const std::string& studentName) const {
    std::ostringstream oss;
    oss << "========================================================\n"
        << "           CERTIFICATE OF ATTENDANCE                    \n"
        << "========================================================\n"
        << "This certifies that:\n\n"
        << "                " << studentName << "\n\n"
        << "has attended the distinguished academic seminar:\n"
        << "       \"" << name << "\"\n"
        << "delivered by Keynote Speaker: " << speaker << "\n"
        << "organized by " << clubName << " on " << date << " at " << venue << ".\n"
        << "========================================================\n";
    return oss.str();
}

std::shared_ptr<Seminar> Seminar::fromJson(const nlohmann::json& j) {
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
    std::string speaker = j.value("speaker", "");

    return std::make_shared<Seminar>(id, name, desc, club, date, time, venue, fee, totalSeats, speaker, seatsLeft, status);
}
