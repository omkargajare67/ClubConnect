#ifndef ENROLLMENT_SERVICE_H
#define ENROLLMENT_SERVICE_H

#include <memory>
#include <mutex>
#include <string>
#include "core/Repository.h"
#include "events/Event.h"
#include "users/User.h"
#include "records/Enrollment.h"

// SERVICE LAYER & THREAD SAFETY: Orchestrates seat reservation and business rule enforcement.
// Uses std::mutex with std::lock_guard to prevent race conditions during concurrent seat acquisition.
class EnrollmentService {
public:
    static int nextEnrollmentId;

private:
    std::shared_ptr<Repository<Event>> eventRepo;
    std::shared_ptr<Repository<User>> userRepo;
    std::shared_ptr<Repository<Enrollment>> enrollmentRepo;
    std::mutex enrollmentMutex;

public:
    EnrollmentService(std::shared_ptr<Repository<Event>> eventRepo,
                      std::shared_ptr<Repository<User>> userRepo,
                      std::shared_ptr<Repository<Enrollment>> enrollmentRepo);

    // CORE BUSINESS LOGIC: Enforces all enrollment validation rules sequentially under mutex lock.
    std::shared_ptr<Enrollment> enroll(int studentId, int eventId);

    // Queries student's active enrollments
    std::vector<std::shared_ptr<Enrollment>> getStudentEnrollments(int studentId) const;

    // Queries enrollments for a specific event
    std::vector<std::shared_ptr<Enrollment>> getEventEnrollments(int eventId) const;
};

#endif // ENROLLMENT_SERVICE_H
