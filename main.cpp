#include <iostream>
#include <vector>
#include <memory>
#include "users/Student.h"
#include "users/MemberStudent.h"
#include "users/GuestStudent.h"
#include "users/ClubAdmin.h"
#include "users/SuperAdmin.h"
#include "events/Workshop.h"
#include "events/Hackathon.h"
#include "events/Seminar.h"
#include "events/CulturalEvent.h"
#include "core/Exceptions.h"

int main() {
    std::cout << "====================================================\n";
    std::cout << "      ClubConnect - Phase 1 Verification Demo        \n";
    std::cout << "====================================================\n\n";

    // 1. INHERITANCE & POLYMORPHISM: Fee Calculation (Member vs Guest Student)
    std::cout << "--- 1. Testing Student Fee Calculation (Polymorphism) ---\n";
    double baseFee = 500.0;
    std::shared_ptr<Student> member = std::make_shared<MemberStudent>(
        1, "Aarav Sharma", "aarav@campus.edu", "pass123", "CS23B001", "Computer Science"
    );
    std::shared_ptr<Student> guest = std::make_shared<GuestStudent>(
        2, "Diya Patel", "diya@other.edu", "pass456", "EXT2026", "External"
    );

    std::cout << "Base Event Fee: INR " << baseFee << "\n";
    std::cout << "Member (" << member->getName() << ", 20% discount): INR " 
              << member->calculateFee(baseFee) << "\n";
    std::cout << "Guest  (" << guest->getName() << ", full fee): INR " 
              << guest->calculateFee(baseFee) << "\n\n";

    // 2. POLYMORPHISM: Event Hierarchy via std::vector<std::shared_ptr<Event>>
    std::cout << "--- 2. Testing Event Polymorphism & JSON Serialization ---\n";
    std::vector<std::shared_ptr<Event>> events;
    events.push_back(std::make_shared<Workshop>(
        1, "Full-Stack Web Dev Workshop", "Hands-on modern web development",
        "Coding Club", "2026-10-15", "10:00", "Lab 301", 300.0, 50, "Dr. Vikram Sen", 45, "APPROVED"
    ));
    events.push_back(std::make_shared<Hackathon>(
        2, "HackCampus 2026", "24-hour national student hackathon",
        "Coding Club", "2026-11-01", "09:00", "Auditorium Main", 500.0, 100, 4, "INR 50,000", 98, "APPROVED"
    ));
    events.push_back(std::make_shared<Seminar>(
        3, "AI & Ethics Keynote", "Seminar on responsible AI research",
        "Robotics Society", "2026-10-20", "14:00", "Seminar Hall B", 0.0, 80, "Prof. Ananya Roy", 70, "APPROVED"
    ));
    events.push_back(std::make_shared<CulturalEvent>(
        4, "Sanskriti Annual Fest", "Inter-college classical music and dance showcase",
        "Cultural Club", "2026-10-25", "18:00", "Open Air Theatre", 150.0, 200, "Music & Dance", 190, "APPROVED"
    ));

    for (const auto& ev : events) {
        // Dynamic dispatch: calls specific subclass displayDetails()
        ev->displayDetails();
        // Dynamic dispatch: calls specific subclass toJson()
        std::cout << "JSON: " << ev->toJson().dump() << "\n\n";
    }

    // 3. ENCAPSULATION & EXCEPTION HANDLING: Testing enroll() and EventFullException
    std::cout << "--- 3. Testing Event Capacity Encapsulation & Exceptions ---\n";
    auto smallWorkshop = std::make_shared<Workshop>(
        5, "Micro-Workshop", "Exclusive 1-seat training",
        "Coding Club", "2026-10-12", "11:00", "Room 101", 100.0, 1, "Mentor John", 1, "APPROVED"
    );
    std::cout << "Initial Seats: " << smallWorkshop->getSeatsLeft() << "/" << smallWorkshop->getTotalSeats() << "\n";
    
    try {
        std::cout << "Enrolling student 1...\n";
        smallWorkshop->enroll();
        std::cout << "Seats left: " << smallWorkshop->getSeatsLeft() << "\n";

        std::cout << "Enrolling student 2 (should fail)...\n";
        smallWorkshop->enroll();
    } catch (const EventFullException& e) {
        std::cout << "Successfully caught expected exception: " << e.what() << "\n";
    }

    std::cout << "\n>>> Phase 1 Verification Completed Successfully! <<<\n";
    return 0;
}
