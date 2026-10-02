#ifndef EVENT_H
#define EVENT_H

#include <string>
#include <memory>
#include <iostream>
#include "json.hpp"
#include "core/Exceptions.h"

// ABSTRACTION: Abstract base class representing a club event.
// Enforces private seat encapsulation, pure virtual polymorphism, and relational operators.
class Event {
public:
    static int nextEventId;

protected:
    int eventId;
    std::string name;
    std::string description;
    std::string clubName;
    std::string date; // YYYY-MM-DD
    std::string time; // HH:MM
    std::string venue;
    double fee;
    std::string status; // "PENDING", "APPROVED", "REJECTED"
    int totalSeats;

private:
    // ENCAPSULATION: seatsLeft is strictly private and can only change via enroll()
    int seatsLeft;

public:
    Event(int eventId, const std::string& name, const std::string& description,
          const std::string& clubName, const std::string& date, const std::string& time,
          const std::string& venue, double fee, int totalSeats,
          int seatsLeft = -1, const std::string& status = "PENDING");

    // POLYMORPHISM: Virtual destructor ensures proper cleanup of derived event types.
    virtual ~Event() = default;

    // ENCAPSULATION: Read-only const accessors for all event attributes.
    int getEventId() const;
    std::string getName() const;
    std::string getDescription() const;
    std::string getClubName() const;
    std::string getDate() const;
    std::string getTime() const;
    std::string getVenue() const;
    double getFee() const;
    std::string getStatus() const;
    int getTotalSeats() const;
    int getSeatsLeft() const;

    // Controlled mutation methods
    void setStatus(const std::string& newStatus);

    // ENCAPSULATION & EXCEPTION HANDLING: Mutates seatsLeft safely, throwing if capacity exhausted.
    void enroll();

    // ABSTRACTION & POLYMORPHISM: Pure virtual methods required by all specialized events.
    virtual nlohmann::json toJson() const = 0;
    virtual void displayDetails() const = 0;
    virtual std::string generateCertificate(const std::string& studentName) const = 0;

    // OPERATOR OVERLOADING: Natural ordering of events chronologically by date.
    bool operator<(const Event& other) const;

    // OPERATOR OVERLOADING: Stream insertion operator for human-readable console output.
    friend std::ostream& operator<<(std::ostream& os, const Event& event);
};

#endif // EVENT_H
