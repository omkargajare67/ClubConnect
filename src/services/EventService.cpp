#include "services/EventService.h"
#include "events/EventFactory.h"
#include "events/EventSearch.h"
#include "users/Student.h"
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

int EventService::markAttendance(int eventId, const std::vector<int>& studentIds, const std::string& adminClubName) {
    auto event = eventRepo->find([eventId](const std::shared_ptr<Event>& e) {
        return e->getEventId() == eventId;
    });
    if (!event) {
        throw std::runtime_error("Event #" + std::to_string(eventId) + " not found.");
    }

    // STRICT CLUB AUTHORIZATION: Only the club admin managing this club can mark attendance
    if (!adminClubName.empty() && event->getClubName() != adminClubName) {
        throw std::runtime_error("Forbidden: You can only record attendance for events hosted by your assigned club ('" +
                                 adminClubName + "'). This event belongs to '" + event->getClubName() + "'.");
    }

    auto enrollments = enrollmentRepo->filter([eventId](const std::shared_ptr<Enrollment>& en) {
        return en->getEventId() == eventId;
    });

    int markedCount = 0;
    for (const auto& en : enrollments) {
        bool isPresent = (std::find(studentIds.begin(), studentIds.end(), en->getStudentId()) != studentIds.end());
        en->markAttended(isPresent);
        if (isPresent) {
            markedCount++;
        }
    }
    return markedCount;
}

nlohmann::json EventService::getEventAttendees(int eventId, const std::string& adminClubName) const {
    auto event = eventRepo->find([eventId](const std::shared_ptr<Event>& e) {
        return e->getEventId() == eventId;
    });
    if (!event) {
        throw std::runtime_error("Event #" + std::to_string(eventId) + " not found.");
    }

    // STRICT CLUB AUTHORIZATION: Only the club admin managing this club can view attendee list
    if (!adminClubName.empty() && event->getClubName() != adminClubName) {
        throw std::runtime_error("Forbidden: You can only view attendees for events hosted by your assigned club ('" +
                                 adminClubName + "'). This event belongs to '" + event->getClubName() + "'.");
    }

    auto enrollments = enrollmentRepo->filter([eventId](const std::shared_ptr<Enrollment>& en) {
        return en->getEventId() == eventId;
    });

    nlohmann::json arr = nlohmann::json::array();
    for (const auto& en : enrollments) {
        auto user = userRepo->find([en](const std::shared_ptr<User>& u) {
            return u->getId() == en->getStudentId();
        });

        nlohmann::json item;
        item["enrollmentId"] = en->getEnrollmentId();
        item["studentId"] = en->getStudentId();
        item["eventId"] = en->getEventId();
        item["attended"] = en->isAttended();
        item["paymentId"] = en->getPaymentId();
        item["enrollmentDate"] = en->getEnrollmentDate();

        if (user) {
            item["studentName"] = user->getName();
            item["studentEmail"] = user->getEmail();
            auto student = std::dynamic_pointer_cast<Student>(user);
            if (student) {
                item["rollNo"] = student->getRollNo();
                item["department"] = student->getDepartment();
                item["membership"] = student->getMembershipType();
            } else {
                item["rollNo"] = "STU" + std::to_string(user->getId());
                item["department"] = "-";
                item["membership"] = "guest";
            }
        } else {
            item["studentName"] = "Student #" + std::to_string(en->getStudentId());
            item["studentEmail"] = "-";
            item["rollNo"] = "STU" + std::to_string(en->getStudentId());
            item["department"] = "-";
            item["membership"] = "guest";
        }
        arr.push_back(item);
    }
    return arr;
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

nlohmann::json EventService::getCertificateDetails(int enrollmentId) const {
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

    nlohmann::json j;
    j["enrollmentId"] = enrollmentId;
    j["certificateId"] = "CERT-CC-2026-" + std::to_string(1000 + enrollmentId);
    j["studentName"] = user->getName();
    j["eventName"] = event->getName();
    j["eventType"] = event->toJson().value("type", "Event");
    j["clubName"] = event->getClubName();
    j["date"] = event->getDate();
    j["time"] = event->getTime();
    j["venue"] = event->getVenue();
    j["certificate"] = event->generateCertificate(user->getName());
    return j;
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

std::vector<std::shared_ptr<Club>> EventService::getAllClubs() const {
    return clubRepo->getAll();
}

std::shared_ptr<Club> EventService::createClub(const nlohmann::json& clubData) {
    std::string name = clubData.value("name", "");
    if (name.empty()) {
        throw std::runtime_error("Club name is required.");
    }
    auto existing = clubRepo->find([&name](const std::shared_ptr<Club>& c) {
        return c->getName() == name;
    });
    if (existing) {
        throw std::runtime_error("A club with name '" + name + "' already exists.");
    }
    int nextId = 1;
    for (const auto& c : clubRepo->getAll()) {
        if (c->getClubId() >= nextId) nextId = c->getClubId() + 1;
    }
    auto newClub = std::make_shared<Club>(
        nextId,
        name,
        clubData.value("description", ""),
        clubData.value("leadAdminEmail", "")
    );
    clubRepo->add(newClub);
    return newClub;
}
