#ifndef CERTIFICATE_H
#define CERTIFICATE_H

#include <string>
#include "json.hpp"

// ENCAPSULATION: Models an immutable, verifiable certificate of attendance/achievement.
class Certificate {
private:
    int certificateId;
    int enrollmentId;
    std::string studentName;
    std::string eventName;
    std::string issueDate;
    std::string content;

public:
    Certificate(int certificateId, int enrollmentId, const std::string& studentName,
                const std::string& eventName, const std::string& issueDate,
                const std::string& content);

    // ENCAPSULATION: Read-only accessors.
    int getCertificateId() const;
    int getEnrollmentId() const;
    std::string getStudentName() const;
    std::string getEventName() const;
    std::string getIssueDate() const;
    std::string getContent() const;

    nlohmann::json toJson() const;
    static Certificate fromJson(const nlohmann::json& j);
};

#endif // CERTIFICATE_H
