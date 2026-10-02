#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <exception>
#include <string>

// EXCEPTION HANDLING: Custom domain-specific exception hierarchy derived from std::exception.
// Demonstrates inheritance and polymorphism via the what() virtual function override.

class EventFullException : public std::exception {
private:
    std::string message;
public:
    explicit EventFullException(const std::string& msg = "Event is completely full. No seats remaining.")
        : message(msg) {}
    
    // POLYMORPHISM: Overriding what() to return customized domain error message
    const char* what() const noexcept override {
        return message.c_str();
    }
};

class AlreadyEnrolledException : public std::exception {
private:
    std::string message;
public:
    explicit AlreadyEnrolledException(const std::string& msg = "Student is already enrolled in this event.")
        : message(msg) {}

    const char* what() const noexcept override {
        return message.c_str();
    }
};

class TimeClashException : public std::exception {
private:
    std::string message;
public:
    explicit TimeClashException(const std::string& msg = "Schedule conflict: Student is already enrolled in another event at the same date and time.")
        : message(msg) {}

    const char* what() const noexcept override {
        return message.c_str();
    }
};

class PaymentFailedException : public std::exception {
private:
    std::string message;
public:
    explicit PaymentFailedException(const std::string& msg = "Payment processing failed. Please verify payment credentials.")
        : message(msg) {}

    const char* what() const noexcept override {
        return message.c_str();
    }
};

#endif // EXCEPTIONS_H
