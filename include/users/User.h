#ifndef USER_H
#define USER_H

#include <string>
#include <memory>
#include "json.hpp"

// ABSTRACTION: Abstract base class representing a generic user in the ClubConnect system.
// Defines common identity, authentication fields, and pure virtual methods for polymorphic role behavior.
class User {
protected:
    int id;
    std::string name;
    std::string email;

private:
    // ENCAPSULATION: Password hash is strictly private to prevent unauthorized exposure.
    // Real-world production systems use bcrypt or argon2; std::hash is used here for academic demonstration.
    std::string passwordHash;

public:
    // POLYMORPHISM: Virtual destructor ensures proper cleanup of derived class resources.
    virtual ~User() = default;

    User(int id, const std::string& name, const std::string& email, const std::string& rawPassword);

    // ENCAPSULATION: Read-only accessors maintaining immutability of identity fields.
    int getId() const;
    std::string getName() const;
    std::string getEmail() const;
    std::string getPasswordHash() const;

    // ENCAPSULATION: Secure password verification without revealing the raw hash.
    bool checkPassword(const std::string& plainPassword) const;

    // Helper static method for hashing passwords
    static std::string hashPassword(const std::string& plainPassword);

    // ABSTRACTION & POLYMORPHISM: Pure virtual methods forcing derived classes to provide role identity and UI menu.
    virtual std::string getRole() const = 0;
    virtual void showMenu() const = 0;

    // POLYMORPHISM: Virtual serialization method to be enriched by derived classes.
    virtual nlohmann::json toJson() const;
};

#endif // USER_H
