#include "services/AuthService.h"
#include "users/UserFactory.h"
#include <random>
#include <sstream>
#include <iomanip>
#include <stdexcept>

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

std::string AuthService::login(const std::string& email, const std::string& plainPassword) {
    auto user = userRepo->find([email](const std::shared_ptr<User>& u) {
        return u->getEmail() == email;
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

std::shared_ptr<User> AuthService::registerStudent(const nlohmann::json& studentData) {
    std::string email = studentData.value("email", "");
    if (email.empty()) {
        throw std::runtime_error("Email address is required.");
    }

    auto existing = userRepo->find([email](const std::shared_ptr<User>& u) {
        return u->getEmail() == email;
    });
    if (existing) {
        throw std::runtime_error("An account with email " + email + " already exists.");
    }

    int nextUserId = 1;
    for (const auto& u : userRepo->getAll()) {
        if (u->getId() >= nextUserId) {
            nextUserId = u->getId() + 1;
        }
    }

    nlohmann::json fullData = studentData;
    fullData["id"] = nextUserId;
    fullData["role"] = "Student";

    auto newUser = UserFactory::createUser(fullData);
    userRepo->add(newUser);
    return newUser;
}

void AuthService::logout(const std::string& token) {
    std::lock_guard<std::mutex> lock(sessionMutex);
    activeSessions.erase(token);
}
