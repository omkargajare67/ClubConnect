#include "records/Enrollment.h"

Enrollment::Enrollment(int enrollmentId, int studentId, int eventId, const std::string& paymentId,
                       bool attended, const std::string& enrollmentDate)
    : enrollmentId(enrollmentId), studentId(studentId), eventId(eventId),
      paymentId(paymentId), attended(attended), enrollmentDate(enrollmentDate) {}

int Enrollment::getEnrollmentId() const { return enrollmentId; }
int Enrollment::getStudentId() const { return studentId; }
int Enrollment::getEventId() const { return eventId; }
std::string Enrollment::getPaymentId() const { return paymentId; }
bool Enrollment::isAttended() const { return attended; }
std::string Enrollment::getEnrollmentDate() const { return enrollmentDate; }

void Enrollment::setPaymentId(const std::string& pid) {
    paymentId = pid;
}

void Enrollment::markAttended(bool status) {
    attended = status;
}

nlohmann::json Enrollment::toJson() const {
    return {
        {"enrollmentId", enrollmentId},
        {"studentId", studentId},
        {"eventId", eventId},
        {"paymentId", paymentId},
        {"attended", attended},
        {"enrollmentDate", enrollmentDate}
    };
}

Enrollment Enrollment::fromJson(const nlohmann::json& j) {
    return Enrollment(
        j.value("enrollmentId", 0),
        j.value("studentId", 0),
        j.value("eventId", 0),
        j.value("paymentId", ""),
        j.value("attended", false),
        j.value("enrollmentDate", "")
    );
}
