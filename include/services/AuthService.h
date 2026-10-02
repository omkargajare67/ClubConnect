#ifndef AUTH_SERVICE_H
#define AUTH_SERVICE_H

#include <memory>
#include <string>
#include <map>
#include <mutex>
#include "json.hpp"
#include "core/Repository.h"
#include "users/User.h"

// SERVICE LAYER: Handles user authentication, token-based session management, and role authorization.
class AuthService {
private:
    std::shared_ptr<Repository<User>> userRepo;
    std::map<std::string, int> activeSessions; // Token -> User ID
    mutable std::mutex sessionMutex;

    static std::string generateToken();

public:
    explicit AuthService(std::shared_ptr<Repository<User>> userRepo);

    // Authenticates credentials and issues a secure session token
    std::string login(const std::string& email, const std::string& plainPassword);

    // Validates Bearer token and returns the authenticated User entity
    std::shared_ptr<User> validateToken(const std::string& token) const;

    // Registers a new student and adds them to repository
    std::shared_ptr<User> registerStudent(const nlohmann::json& studentData);

    // Terminates an active session
    void logout(const std::string& token);
};

#endif // AUTH_SERVICE_H
