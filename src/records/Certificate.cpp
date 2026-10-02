#include "records/Certificate.h"

Certificate::Certificate(int certificateId, int enrollmentId, const std::string& studentName,
                         const std::string& eventName, const std::string& issueDate,
                         const std::string& content)
    : certificateId(certificateId), enrollmentId(enrollmentId),
      studentName(studentName), eventName(eventName),
      issueDate(issueDate), content(content) {}

int Certificate::getCertificateId() const { return certificateId; }
int Certificate::getEnrollmentId() const { return enrollmentId; }
std::string Certificate::getStudentName() const { return studentName; }
std::string Certificate::getEventName() const { return eventName; }
std::string Certificate::getIssueDate() const { return issueDate; }
std::string Certificate::getContent() const { return content; }

nlohmann::json Certificate::toJson() const {
    return {
        {"certificateId", certificateId},
        {"enrollmentId", enrollmentId},
        {"studentName", studentName},
        {"eventName", eventName},
        {"issueDate", issueDate},
        {"content", content}
    };
}

Certificate Certificate::fromJson(const nlohmann::json& j) {
    return Certificate(
        j.value("certificateId", 0),
        j.value("enrollmentId", 0),
        j.value("studentName", ""),
        j.value("eventName", ""),
        j.value("issueDate", ""),
        j.value("content", "")
    );
}
