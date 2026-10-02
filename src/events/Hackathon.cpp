#include "events/Hackathon.h"
#include <iostream>
#include <sstream>

Hackathon::Hackathon(int eventId, const std::string& name, const std::string& description,
                     const std::string& clubName, const std::string& date, const std::string& time,
                     const std::string& venue, double fee, int totalSeats, int teamSize, const std::string& prize,
                     int seatsLeft, const std::string& status)
    : Event(eventId, name, description, clubName, date, time, venue, fee, totalSeats, seatsLeft, status),
      teamSize(teamSize), prize(prize) {}

int Hackathon::getTeamSize() const { return teamSize; }
std::string Hackathon::getPrize() const { return prize; }

// POLYMORPHISM: Serializes hackathon details with team size and prize pool.
nlohmann::json Hackathon::toJson() const {
    return {
        {"eventId", eventId},
        {"type", "Hackathon"},
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
        {"teamSize", teamSize},
        {"prize", prize}
    };
}

// POLYMORPHISM: Console detail view for hackathon.
void Hackathon::displayDetails() const {
    std::cout << "[HACKATHON] " << name << " | Team Size: " << teamSize << " | Prize Pool: " << prize << "\n"
              << "  Club: " << clubName << " | Date: " << date << " " << time
              << " | Venue: " << venue << " | Seats Left: " << getSeatsLeft() << "/" << totalSeats
              << " | Fee: INR " << fee << "\n"
              << "  Description: " << description << "\n";
}

// POLYMORPHISM: Generates hackathon achievement certificate.
std::string Hackathon::generateCertificate(const std::string& studentName) const {
    std::ostringstream oss;
    oss << "========================================================\n"
        << "            CERTIFICATE OF EXCELLENCE                   \n"
        << "========================================================\n"
        << "This certifies that:\n\n"
        << "                " << studentName << "\n\n"
        << "has successfully participated in the competitive Hackathon:\n"
        << "       \"" << name << "\"\n"
        << "Award Category: Participation & Finalist (Team Size: " << teamSize << ")\n"
        << "Total Prize Pool: " << prize << "\n"
        << "organized by " << clubName << " on " << date << " at " << venue << ".\n"
        << "========================================================\n";
    return oss.str();
}

std::shared_ptr<Hackathon> Hackathon::fromJson(const nlohmann::json& j) {
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
    int teamSize = j.value("teamSize", 1);
    std::string prize = j.value("prize", "");

    return std::make_shared<Hackathon>(id, name, desc, club, date, time, venue, fee, totalSeats, teamSize, prize, seatsLeft, status);
}
