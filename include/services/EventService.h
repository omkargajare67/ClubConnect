#ifndef EVENT_SERVICE_H
#define EVENT_SERVICE_H

#include <memory>
#include <vector>
#include <string>
#include "json.hpp"
#include "core/Repository.h"
#include "events/Event.h"
#include "records/Enrollment.h"
#include "records/Receipt.h"
#include "records/Certificate.h"
#include "records/Club.h"
#include "users/User.h"

// SERVICE LAYER: Handles event lifecycle, approval state transitions, attendance logging,
// polymorphic certificate generation, and administrative business intelligence analytics.
class EventService {
private:
    std::shared_ptr<Repository<Event>> eventRepo;
    std::shared_ptr<Repository<Enrollment>> enrollmentRepo;
    std::shared_ptr<Repository<Receipt>> receiptRepo;
    std::shared_ptr<Repository<User>> userRepo;
    std::shared_ptr<Repository<Club>> clubRepo;

public:
    EventService(std::shared_ptr<Repository<Event>> eventRepo,
                 std::shared_ptr<Repository<Enrollment>> enrollmentRepo,
                 std::shared_ptr<Repository<Receipt>> receiptRepo,
                 std::shared_ptr<Repository<User>> userRepo,
                 std::shared_ptr<Repository<Club>> clubRepo);

    // Filter and sort publicly approved events
    std::vector<std::shared_ptr<Event>> getApprovedEvents(
        const std::string& name = "", const std::string& club = "",
        double maxFee = -1.0, const std::string& sortBy = "date") const;

    // Retrieves events pending administrative clearance
    std::vector<std::shared_ptr<Event>> getPendingEvents() const;

    // ClubAdmin posts new event in PENDING status
    std::shared_ptr<Event> createEvent(const nlohmann::json& eventData, const std::string& authorizedClub);

    // SuperAdmin approval / rejection
    bool setEventStatus(int eventId, const std::string& status);

    // ClubAdmin marks attendance for student attendees (strictly scoped to authorized club)
    int markAttendance(int eventId, const std::vector<int>& studentIds, const std::string& adminClubName = "");

    // Enrolled attendees list for an event (strictly scoped to authorized club)
    nlohmann::json getEventAttendees(int eventId, const std::string& adminClubName = "") const;

    // POLYMORPHISM & BUSINESS RULE: Issues certificate ONLY if student was marked present
    std::string generateCertificate(int enrollmentId) const;
    nlohmann::json getCertificateDetails(int enrollmentId) const;

    // SuperAdmin reports: enrollment and revenue analytics per event
    nlohmann::json getReports() const;

    // Clubs directory & creation
    std::vector<std::shared_ptr<Club>> getAllClubs() const;
    std::shared_ptr<Club> createClub(const nlohmann::json& clubData);
};

#endif // EVENT_SERVICE_H
