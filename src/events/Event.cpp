#include "events/Event.h"
#include <iomanip>

// STATIC MEMBER: Initializer for auto-incrementing event identifier.
int Event::nextEventId = 1;

Event::Event(int eventId, const std::string& name, const std::string& description,
             const std::string& clubName, const std::string& date, const std::string& time,
             const std::string& venue, double fee, int totalSeats,
             int seatsLeft, const std::string& status)
    : eventId(eventId), name(name), description(description),
      clubName(clubName), date(date), time(time),
      venue(venue), fee(fee), status(status), totalSeats(totalSeats),
      seatsLeft(seatsLeft < 0 ? totalSeats : seatsLeft) {}

// ENCAPSULATION: Const getters.
int Event::getEventId() const { return eventId; }
std::string Event::getName() const { return name; }
std::string Event::getDescription() const { return description; }
std::string Event::getClubName() const { return clubName; }
std::string Event::getDate() const { return date; }
std::string Event::getTime() const { return time; }
std::string Event::getVenue() const { return venue; }
double Event::getFee() const { return fee; }
std::string Event::getStatus() const { return status; }
int Event::getTotalSeats() const { return totalSeats; }
int Event::getSeatsLeft() const { return seatsLeft; }

void Event::setStatus(const std::string& newStatus) {
    status = newStatus;
}

// ENCAPSULATION: Ensures capacity constraint cannot be bypassed.
void Event::enroll() {
    if (seatsLeft <= 0) {
        throw EventFullException("Cannot enroll: Event '" + name + "' is completely full (0 seats left).");
    }
    --seatsLeft;
}

// OPERATOR OVERLOADING: Enables std::sort to arrange events by date automatically.
bool Event::operator<(const Event& other) const {
    if (this->date != other.date) {
        return this->date < other.date;
    }
    return this->time < other.time;
}

// OPERATOR OVERLOADING: Stream output formatting event details.
std::ostream& operator<<(std::ostream& os, const Event& event) {
    os << "[" << event.eventId << "] " << event.name << " (" << event.clubName << ") "
       << "| Date: " << event.date << " " << event.time << " @ " << event.venue
       << " | Fee: INR " << std::fixed << std::setprecision(2) << event.fee
       << " | Seats: " << event.seatsLeft << "/" << event.totalSeats
       << " | Status: " << event.status;
    return os;
}
