#include "services/EventService.h"
#include "events/EventFactory.h"
#include "events/EventSearch.h"
#include <algorithm>
#include <stdexcept>

EventService::EventService(std::shared_ptr<Repository<Event>> eventRepo,
                           std::shared_ptr<Repository<Enrollment>> enrollmentRepo,
                           std::shared_ptr<Repository<Receipt>> receiptRepo,
                           std::shared_ptr<Repository<User>> userRepo,
                           std::shared_ptr<Repository<Club>> clubRepo)
    : eventRepo(eventRepo), enrollmentRepo(enrollmentRepo), receiptRepo(receiptRepo),
      userRepo(userRepo), clubRepo(clubRepo) {}

std::vector<std::shared_ptr<Event>> EventService::getApprovedEvents(
    const std::string& name, const std::string& club, double maxFee, const std::string& sortBy) const {
    
    // Filter by APPROVED status first
    auto approved = eventRepo->filter([](const std::shared_ptr<Event>& e) {
        return e->getStatus() == "APPROVED";
    });

    // Apply FUNCTION OVERLOADING search filters
    if (!name.empty()) {
        approved = EventSearch::searchEvent(approved, name);
    }
    if (!club.empty()) {
        auto c = clubRepo->find([club](const std::shared_ptr<Club>& cl) {
            return cl->getName() == club;
        });
        if (c) {
            approved = EventSearch::searchEvent(approved, *c);
        } else {
            // Filter by name directly if club not registered as object
            approved = eventRepo->filter([club](const std::shared_ptr<Event>& e) {
                return e->getClubName() == club && e->getStatus() == "APPROVED";
            });
        }
    }
    if (maxFee >= 0.0) {
        approved = EventSearch::searchEvent(approved, maxFee);
    }

    // Apply OPERATOR OVERLOADING or LAMBDA sorting
    if (sortBy == "fee") {
        // Lambda comparator sorting by ascending fee
        std::sort(approved.begin(), approved.end(), [](const std::shared_ptr<Event>& a, const std::shared_ptr<Event>& b) {
            return a->getFee() < b->getFee();
        });
    } else {
        // Natural chronological order via Event::operator<
        std::sort(approved.begin(), approved.end(), [](const std::shared_ptr<Event>& a, const std::shared_ptr<Event>& b) {
            return (*a) < (*b);
        });
    }

    return approved;
}

std::vector<std::shared_ptr<Event>> EventService::getPendingEvents() const {
    return eventRepo->filter([](const std::shared_ptr<Event>& e) {
        return e->getStatus() == "PENDING";
    });
}

// FACTORY PATTERN: Posts event with PENDING status for club admin
std::shared_ptr<Event> EventService::createEvent(const nlohmann::json& eventData, const std::string& authorizedClub) {
    nlohmann::json fullData = eventData;
    fullData["eventId"] = Event::nextEventId++;
    fullData["status"] = "PENDING";
    if (!authorizedClub.empty()) {
        fullData["clubName"] = authorizedClub;
    }
    int totalSeats = fullData.value("totalSeats", 50);
    fullData["seatsLeft"] = totalSeats;

    auto newEvent = EventFactory::createEvent(fullData);
    eventRepo->add(newEvent);
    return newEvent;
}

bool EventService::setEventStatus(int eventId, const std::string& status) {
    auto event = eventRepo->find([eventId](const std::shared_ptr<Event>& e) {
        return e->getEventId() == eventId;
    });
    if (!event) {
        throw std::runtime_error("Event #" + std::to_string(eventId) + " not found.");
    }
    event->setStatus(status);
    return true;
}

int EventService::markAttendance(int eventId, const std::vector<int>& studentIds) {
    auto enrollments = enrollmentRepo->filter([eventId](const std::shared_ptr<Enrollment>& en) {
        return en->getEventId() == eventId;
    });

    int markedCount = 0;
    for (const auto& en : enrollments) {
        if (std::find(studentIds.begin(), studentIds.end(), en->getStudentId()) != studentIds.end()) {
            en->markAttended(true);
            markedCount++;
        }
    }
    return markedCount;
}

// POLYMORPHISM & BUSINESS RULE: Certificate issued ONLY to attendees
std::string EventService::generateCertificate(int enrollmentId) const {
    auto enrollment = enrollmentRepo->find([enrollmentId](const std::shared_ptr<Enrollment>& en) {
        return en->getEnrollmentId() == enrollmentId;
    });
    if (!enrollment) {
        throw std::runtime_error("Enrollment #" + std::to_string(enrollmentId) + " not found.");
    }

    if (!enrollment->isAttended()) {
        throw std::runtime_error("Certificate cannot be issued: Attendance has not been confirmed for this student.");
    }

    auto event = eventRepo->find([enrollment](const std::shared_ptr<Event>& e) {
        return e->getEventId() == enrollment->getEventId();
    });
    if (!event) {
        throw std::runtime_error("Event not found for this enrollment.");
    }

    auto user = userRepo->find([enrollment](const std::shared_ptr<User>& u) {
        return u->getId() == enrollment->getStudentId();
    });
    if (!user) {
        throw std::runtime_error("Student not found for this enrollment.");
    }

    // Dynamic dispatch to Event subclass certificate generator
    return event->generateCertificate(user->getName());
}

// BUSINESS INTELLIGENCE ANALYTICS: SuperAdmin reporting
nlohmann::json EventService::getReports() const {
    nlohmann::json reportArr = nlohmann::json::array();
    auto allEvents = eventRepo->getAll();

    for (const auto& ev : allEvents) {
        int eventId = ev->getEventId();
        auto enrollments = enrollmentRepo->filter([eventId](const std::shared_ptr<Enrollment>& en) {
            return en->getEventId() == eventId;
        });

        double totalRevenue = 0.0;
        for (const auto& en : enrollments) {
            auto receipt = receiptRepo->find([en](const std::shared_ptr<Receipt>& r) {
                return r->getEnrollmentId() == en->getEnrollmentId();
            });
            if (receipt) {
                totalRevenue += receipt->getAmountPaid();
            }
        }

        reportArr.push_back({
            {"eventId", eventId},
            {"name", ev->getName()},
            {"clubName", ev->getClubName()},
            {"status", ev->getStatus()},
            {"totalSeats", ev->getTotalSeats()},
            {"seatsLeft", ev->getSeatsLeft()},
            {"seatsEnrolled", static_cast<int>(enrollments.size())},
            {"revenue", totalRevenue}
        });
    }

    return reportArr;
}
