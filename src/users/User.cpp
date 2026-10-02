#include "users/User.h"
#include <functional>
#include <sstream>

// ENCAPSULATION: Constructor initializes identity and encapsulates hashed credential storage.
User::User(int id, const std::string& name, const std::string& email, const std::string& rawPassword)
    : id(id), name(name), email(email), passwordHash(hashPassword(rawPassword)) {}

// ENCAPSULATION: Const getters provide read-only access.
int User::getId() const { return id; }
std::string User::getName() const { return name; }
std::string User::getEmail() const { return email; }
std::string User::getPasswordHash() const { return passwordHash; }

// ENCAPSULATION: Validates plain text against stored hash without returning the secret.
bool User::checkPassword(const std::string& plainPassword) const {
    return passwordHash == hashPassword(plainPassword);
}

// Static utility: simple hashing for academic demonstration. Note: production systems use bcrypt/argon2.
std::string User::hashPassword(const std::string& plainPassword) {
    // If the input already looks like a 64-bit unsigned hash number, don't re-hash it
    // But to keep it simple, std::hash produces size_t
    std::hash<std::string> hasher;
    size_t hashValue = hasher(plainPassword);
    std::stringstream ss;
    ss << std::hex << hashValue;
    return ss.str();
}

// POLYMORPHISM: Base JSON serialization providing standard fields.
nlohmann::json User::toJson() const {
    return {
        {"id", id},
        {"name", name},
        {"email", email},
        {"role", getRole()}
    };
}
