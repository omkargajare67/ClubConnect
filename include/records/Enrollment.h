#ifndef ENROLLMENT_H
#define ENROLLMENT_H

#include <string>
#include "json.hpp"

// ENCAPSULATION: Represents an enrollment binding a student to an event and payment.
class Enrollment {
private:
    int enrollmentId;
    int studentId;
    int eventId;
    std::string paymentId;
    bool attended;
    std::string enrollmentDate;

public:
    Enrollment(int enrollmentId, int studentId, int eventId, const std::string& paymentId,
               bool attended, const std::string& enrollmentDate);

    // ENCAPSULATION: Const getters.
    int getEnrollmentId() const;
    int getStudentId() const;
    int getEventId() const;
    std::string getPaymentId() const;
    bool isAttended() const;
    std::string getEnrollmentDate() const;

    // State mutators
    void setPaymentId(const std::string& pid);
    void markAttended(bool status = true);

    nlohmann::json toJson() const;
    static Enrollment fromJson(const nlohmann::json& j);
};

#endif // ENROLLMENT_H
