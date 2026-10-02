#include "services/EnrollmentService.h"
#include "core/Exceptions.h"
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

int EnrollmentService::nextEnrollmentId = 1;

EnrollmentService::EnrollmentService(std::shared_ptr<Repository<Event>> eventRepo,
                                     std::shared_ptr<Repository<User>> userRepo,
                                     std::shared_ptr<Repository<Enrollment>> enrollmentRepo)
    : eventRepo(eventRepo), userRepo(userRepo), enrollmentRepo(enrollmentRepo) {}

// THREAD SAFETY & BUSINESS RULE ENFORCEMENT:
// std::lock_guard guarantees atomic checks so two threads cannot oversubscribe the last seat.
std::shared_ptr<Enrollment> EnrollmentService::enroll(int studentId, int eventId) {
    std::lock_guard<std::mutex> lock(enrollmentMutex);

    // 1. Verify User exists
    auto user = userRepo->find([studentId](const std::shared_ptr<User>& u) {
        return u->getId() == studentId;
    });
    if (!user) {
        throw std::runtime_error("Student with ID " + std::to_string(studentId) + " does not exist.");
    }

    // 2. Verify Event exists and is approved
    auto event = eventRepo->find([eventId](const std::shared_ptr<Event>& e) {
        return e->getEventId() == eventId;
    });
    if (!event) {
        throw std::runtime_error("Event with ID " + std::to_string(eventId) + " not found.");
    }
    if (event->getStatus() != "APPROVED") {
        throw std::runtime_error("Event is not yet approved for student enrollment (Status: " + event->getStatus() + ").");
    }

    // 3. Check for Duplicate Enrollment
    auto existingEnrollment = enrollmentRepo->find([studentId, eventId](const std::shared_ptr<Enrollment>& en) {
        return en->getStudentId() == studentId && en->getEventId() == eventId;
    });
    if (existingEnrollment) {
        throw AlreadyEnrolledException("Student is already enrolled in event '" + event->getName() + "'.");
    }

    // 4. Check for Time Clash with other events the student is enrolled in
    auto studentEnrollments = enrollmentRepo->filter([studentId](const std::shared_ptr<Enrollment>& en) {
        return en->getStudentId() == studentId;
    });

    for (const auto& en : studentEnrollments) {
        auto enrolledEvent = eventRepo->find([en](const std::shared_ptr<Event>& e) {
            return e->getEventId() == en->getEventId();
        });
        if (enrolledEvent && enrolledEvent->getDate() == event->getDate() && enrolledEvent->getTime() == event->getTime()) {
            throw TimeClashException("Time Clash: Event '" + event->getName() + "' clashes with '" +
                                    enrolledEvent->getName() + "' on " + event->getDate() + " at " + event->getTime() + ".");
        }
    }

    // 5. Reserve Seat (throws EventFullException if seatsLeft <= 0)
    event->enroll();

    // 6. Generate current timestamp
    auto now = std::chrono::system_clock::now();
    std::time_t nowTime = std::chrono::system_clock::to_time_t(now);
    std::tm ltm;
#if defined(_WIN32) || defined(_WIN64)
    localtime_s(&ltm, &nowTime);
#else
    localtime_r(&nowTime, &ltm);
#endif
    std::ostringstream dateStream;
    dateStream << std::put_time(&ltm, "%Y-%m-%d %H:%M:%S");

    // 7. Create enrollment record
    int newId = nextEnrollmentId++;
    auto enrollment = std::make_shared<Enrollment>(newId, studentId, eventId, "", false, dateStream.str());
    enrollmentRepo->add(enrollment);

    return enrollment;
}

std::vector<std::shared_ptr<Enrollment>> EnrollmentService::getStudentEnrollments(int studentId) const {
    return enrollmentRepo->filter([studentId](const std::shared_ptr<Enrollment>& en) {
        return en->getStudentId() == studentId;
    });
}

std::vector<std::shared_ptr<Enrollment>> EnrollmentService::getEventEnrollments(int eventId) const {
    return enrollmentRepo->filter([eventId](const std::shared_ptr<Enrollment>& en) {
        return en->getEventId() == eventId;
    });
}
