#include "services/AuthService.h"
#include "users/UserFactory.h"
#include <random>
#include <sstream>
#include <iomanip>
#include <stdexcept>
#include <regex>
#include <cctype>
#include <algorithm>

AuthService::AuthService(std::shared_ptr<Repository<User>> userRepo)
    : userRepo(userRepo) {}

std::string AuthService::generateToken() {
    std::random_device rd;
    std::mt19937_64 gen(rd());
    std::uniform_int_distribution<uint64_t> dis;

    std::stringstream ss;
    ss << "CC-TOK-" << std::hex << dis(gen) << dis(gen);
    return ss.str();
}

static std::string normalizeEmail(const std::string& email) {
    auto start = email.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    auto end = email.find_last_not_of(" \t\r\n");
    std::string s = email.substr(start, end - start + 1);
    std::transform(s.begin(), s.end(), s.begin(), ::tolower);
    return s;
}

bool AuthService::isValidEmail(const std::string& email) {
    if (email.empty() || email.length() > 254) return false;
    static const std::regex emailPattern(R"(^[a-zA-Z0-9.!#$%&'*+/=?^_`{|}~-]+@[a-zA-Z0-9](?:[a-zA-Z0-9-]{0,61}[a-zA-Z0-9])?(?:\.[a-zA-Z0-9](?:[a-zA-Z0-9-]{0,61}[a-zA-Z0-9])?)+$)");
    return std::regex_match(email, emailPattern);
}

bool AuthService::isValidPassword(const std::string& password, std::string& errorReason) {
    if (password.length() < 8) {
        errorReason = "Password must be at least 8 characters long.";
        return false;
    }
    bool hasUpper = false;
    bool hasLower = false;
    bool hasDigit = false;
    bool hasSpecial = false;

    for (char c : password) {
        if (std::isupper(static_cast<unsigned char>(c))) hasUpper = true;
        else if (std::islower(static_cast<unsigned char>(c))) hasLower = true;
        else if (std::isdigit(static_cast<unsigned char>(c))) hasDigit = true;
        else if (std::ispunct(static_cast<unsigned char>(c))) hasSpecial = true;
    }

    if (!hasUpper) {
        errorReason = "Password must contain at least one uppercase letter (A-Z).";
        return false;
    }
    if (!hasLower) {
        errorReason = "Password must contain at least one lowercase letter (a-z).";
        return false;
    }
    if (!hasDigit) {
        errorReason = "Password must contain at least one number (0-9).";
        return false;
    }
    if (!hasSpecial) {
        errorReason = "Password must contain at least one special character (!@#$%^&* etc.).";
        return false;
    }

    return true;
}

std::string AuthService::login(const std::string& email, const std::string& plainPassword) {
    std::string normEmail = normalizeEmail(email);
    if (normEmail.empty() || plainPassword.empty()) {
        throw std::runtime_error("Email and password are required.");
    }
    if (!isValidEmail(normEmail)) {
        throw std::runtime_error("Please enter a valid email address.");
    }

    auto user = userRepo->find([normEmail](const std::shared_ptr<User>& u) {
        return normalizeEmail(u->getEmail()) == normEmail;
    });

    if (!user || !user->checkPassword(plainPassword)) {
        throw std::runtime_error("Invalid email or password credentials.");
    }

    std::string token = generateToken();
    {
        std::lock_guard<std::mutex> lock(sessionMutex);
        activeSessions[token] = user->getId();
    }
    return token;
}

std::shared_ptr<User> AuthService::validateToken(const std::string& token) const {
    int userId = -1;
    {
        std::lock_guard<std::mutex> lock(sessionMutex);
        auto it = activeSessions.find(token);
        if (it != activeSessions.end()) {
            userId = it->second;
        }
    }

    if (userId == -1) {
        return nullptr;
    }

    return userRepo->find([userId](const std::shared_ptr<User>& u) {
        return u->getId() == userId;
    });
}

std::shared_ptr<User> AuthService::registerUser(const nlohmann::json& userData) {
    std::string rawName = userData.value("name", "");
    auto nameStart = rawName.find_first_not_of(" \t\r\n");
    if (nameStart == std::string::npos || rawName.length() < 2) {
        throw std::runtime_error("Full name must be at least 2 characters.");
    }

    std::string rawEmail = userData.value("email", "");
    std::string normEmail = normalizeEmail(rawEmail);
    if (normEmail.empty() || !isValidEmail(normEmail)) {
        throw std::runtime_error("Please enter a valid email address (e.g. name@campus.edu).");
    }

    auto existing = userRepo->find([normEmail](const std::shared_ptr<User>& u) {
        return normalizeEmail(u->getEmail()) == normEmail;
    });
    if (existing) {
        throw std::runtime_error("An account with email " + normEmail + " already exists.");
    }

    std::string rawPassword = userData.value("password", "");
    std::string passwordError;
    if (!isValidPassword(rawPassword, passwordError)) {
        throw std::runtime_error(passwordError);
    }

    std::string role = userData.value("role", "Student");
    if (role != "Student" && role != "ClubAdmin" && role != "SuperAdmin") {
        role = "Student";
    }

    nlohmann::json fullData = userData;

    if (role == "Student") {
        std::string rollNo = userData.value("rollNo", "");
        if (rollNo.find_first_not_of(" \t\r\n") == std::string::npos) {
            throw std::runtime_error("Roll number is required for student registration.");
        }
        std::string dept = userData.value("department", "");
        if (dept.find_first_not_of(" \t\r\n") == std::string::npos) {
            throw std::runtime_error("Department is required for student registration.");
        }
        std::string membership = userData.value("membership", "guest");
        if (membership != "member" && membership != "guest") membership = "guest";
        fullData["rollNo"] = rollNo;
        fullData["department"] = dept;
        fullData["membership"] = membership;
    } else if (role == "ClubAdmin") {
        std::string club = userData.value("clubName", "");
        if (club.find_first_not_of(" \t\r\n") == std::string::npos) {
            throw std::runtime_error("Club name is required for ClubAdmin registration.");
        }
        fullData["clubName"] = club;
    } else if (role == "SuperAdmin") {
        std::string adminKey = userData.value("adminKey", "");
        if (adminKey != "SUPERADMIN2026") {
            throw std::runtime_error("Invalid SuperAdmin authorization security key. (Enter 'SUPERADMIN2026')");
        }
    }

    int nextUserId = 1;
    for (const auto& u : userRepo->getAll()) {
        if (u->getId() >= nextUserId) {
            nextUserId = u->getId() + 1;
        }
    }

    fullData["id"] = nextUserId;
    fullData["name"] = rawName;
    fullData["email"] = normEmail;
    fullData["password"] = rawPassword;
    fullData["role"] = role;
    fullData["isRawPassword"] = true;
    fullData["isHashed"] = false;

    auto newUser = UserFactory::createUser(fullData);
    userRepo->add(newUser);
    return newUser;
}

std::shared_ptr<User> AuthService::registerStudent(const nlohmann::json& studentData) {
    nlohmann::json d = studentData;
    d["role"] = "Student";
    return registerUser(d);
}

bool AuthService::changePassword(int userId, const std::string& oldPassword, const std::string& newPassword) {
    auto user = userRepo->find([userId](const std::shared_ptr<User>& u) {
        return u->getId() == userId;
    });
    if (!user) {
        throw std::runtime_error("User not found.");
    }
    if (!user->checkPassword(oldPassword)) {
        throw std::runtime_error("Current password does not match.");
    }
    std::string reason;
    if (!isValidPassword(newPassword, reason)) {
        throw std::runtime_error(reason);
    }
    user->setPassword(newPassword);
    return true;
}

void AuthService::logout(const std::string& token) {
    std::lock_guard<std::mutex> lock(sessionMutex);
    activeSessions.erase(token);
}
