#include <iostream>
#include <memory>
#include <vector>
#include "users/Student.h"
#include "users/MemberStudent.h"
#include "users/GuestStudent.h"
#include "events/Workshop.h"
#include "events/Hackathon.h"
#include "events/Seminar.h"
#include "records/Club.h"
#include "records/Enrollment.h"
#include "records/Receipt.h"
#include "records/Certificate.h"
#include "records/Team.h"
#include "payments/Payment.h"
#include "payments/UPIPayment.h"
#include "payments/CardPayment.h"
#include "payments/CashPayment.h"
#include "core/Repository.h"
#include "core/Exceptions.h"
#include "services/EnrollmentService.h"
#include "services/PaymentService.h"

int main() {
    std::cout << "========================================================\n";
    std::cout << "       ClubConnect - Phase 2 Verification Demo          \n";
    std::cout << "========================================================\n\n";

    // Initialize in-memory Repositories
    auto userRepo = std::make_shared<Repository<User>>();
    auto eventRepo = std::make_shared<Repository<Event>>();
    auto enrollmentRepo = std::make_shared<Repository<Enrollment>>();
    auto paymentRepo = std::make_shared<Repository<Payment>>();
    auto receiptRepo = std::make_shared<Repository<Receipt>>();

    // Seed test students
    auto s1 = std::make_shared<MemberStudent>(101, "Aarav Sharma", "aarav@campus.edu", "pass1", "CS01", "CSE");
    auto s2 = std::make_shared<GuestStudent>(102, "Diya Patel", "diya@campus.edu", "pass2", "EC01", "ECE");
    auto s3 = std::make_shared<MemberStudent>(103, "Rohan Verma", "rohan@campus.edu", "pass3", "ME01", "MECH");
    userRepo->add(s1);
    userRepo->add(s2);
    userRepo->add(s3);

    // Seed test events
    // Event 1: Workshop with 2 seats, fee 500, on 2026-10-15 at 10:00
    auto ev1 = std::make_shared<Workshop>(1, "AI Workshop", "Deep Learning Hands-on",
                                          "Coding Club", "2026-10-15", "10:00", "Lab 1", 500.0, 2, "Dr. Sen", 2, "APPROVED");
    // Event 2: Time-clashing event on SAME date and SAME time
    auto ev2 = std::make_shared<Seminar>(2, "Quantum Computing Keynote", "Future Tech",
                                         "Tech Club", "2026-10-15", "10:00", "Auditorium", 200.0, 50, "Prof. Bose", 50, "APPROVED");
    // Event 3: Free Seminar on 2026-10-20 at 14:00
    auto ev3 = std::make_shared<Seminar>(3, "Open Source Tech Talk", "FOSS intro",
                                         "Coding Club", "2026-10-20", "14:00", "Hall C", 0.0, 30, "Linus K", 30, "APPROVED");

    eventRepo->add(ev1);
    eventRepo->add(ev2);
    eventRepo->add(ev3);

    // Initialize Services
    EnrollmentService enrollmentService(eventRepo, userRepo, enrollmentRepo);
    PaymentService paymentService(paymentRepo, receiptRepo, enrollmentRepo, eventRepo, userRepo);

    // 1. HAPPY PATH: Enroll -> Initiate Payment (UPI) -> Confirm Payment -> Print Receipt with Friend Function
    std::cout << "--- 1. Testing Happy Path: Enroll -> UPI Pay -> Confirm -> Receipt ---\n";
    auto enrollment1 = enrollmentService.enroll(s1->getId(), ev1->getEventId());
    std::cout << "Enrolled Student #" << s1->getId() << " in Event #" << ev1->getEventId()
              << " | Enrollment ID: " << enrollment1->getEnrollmentId() << "\n";

    // Student 1 is a MemberStudent, so 500 base fee -> 20% discount = 400
    nlohmann::json upiDetails = {{"studentUpiId", "aarav@okaxis"}};
    auto payment1 = paymentService.initiatePayment(enrollment1->getEnrollmentId(), "UPI", upiDetails);
    std::cout << "Payment Initiated! Txn ID: " << payment1->getTransactionId()
              << " | Amount: INR " << payment1->getAmount()
              << " | Status: " << payment1->getStatus() << "\n";

    // Print UPI Deep link instruction
    std::cout << "UPI Deep Link Instruction:\n  --> " << payment1->getPaymentInstruction() << "\n";

    // Confirm Payment
    auto receipt1 = paymentService.confirmPayment(payment1->getTransactionId());
    std::cout << "Payment Confirmed! " << *receipt1 << "\n\n";

    // FRIEND FUNCTION: Print Receipt with private Payment access
    std::cout << "--- Testing Friend Function: printReceiptWithPayment(...) ---\n";
    printReceiptWithPayment(std::cout, *receipt1, *payment1);

    // 2. FREE EVENT PATH: Automatic Free Receipt
    std::cout << "\n--- 2. Testing Free Event Exemption ---\n";
    auto freeEnrollment = enrollmentService.enroll(s2->getId(), ev3->getEventId());
    auto freePayment = paymentService.initiatePayment(freeEnrollment->getEnrollmentId(), "Cash", {});
    auto freeReceipt = paymentService.getReceiptByEnrollmentId(freeEnrollment->getEnrollmentId());
    std::cout << "Free Event auto-confirmed! Amount: " << freePayment->getAmount()
              << " | Method: " << freeReceipt->getPaymentMethod()
              << " | Status: " << freePayment->getStatus() << "\n\n";

    // 3. TESTING DOMAIN EXCEPTIONS
    std::cout << "--- 3. Testing Domain Business Rule Exceptions ---\n";

    // Test A: AlreadyEnrolledException
    try {
        std::cout << "[Check A] Attempting duplicate enrollment for Student #101 in Event #1...\n";
        enrollmentService.enroll(s1->getId(), ev1->getEventId());
    } catch (const AlreadyEnrolledException& e) {
        std::cout << "  --> CAUGHT EXPECTED AlreadyEnrolledException: " << e.what() << "\n";
    }

    // Test B: TimeClashException (Student 1 already has an event at 2026-10-15 10:00)
    try {
        std::cout << "[Check B] Attempting clashing enrollment for Student #101 in Event #2 (same date & time)...\n";
        enrollmentService.enroll(s1->getId(), ev2->getEventId());
    } catch (const TimeClashException& e) {
        std::cout << "  --> CAUGHT EXPECTED TimeClashException: " << e.what() << "\n";
    }

    // Test C: EventFullException (Event 1 has 2 seats total. Student 1 took 1. Let Student 2 take 2nd, then Student 3 fails)
    std::cout << "[Check C] Filling last seat in Event #1 with Student #102...\n";
    enrollmentService.enroll(s2->getId(), ev1->getEventId());
    std::cout << "Seats left in Event #1: " << ev1->getSeatsLeft() << "/" << ev1->getTotalSeats() << "\n";
    try {
        std::cout << "Attempting to enroll Student #103 into full Event #1...\n";
        enrollmentService.enroll(s3->getId(), ev1->getEventId());
    } catch (const EventFullException& e) {
        std::cout << "  --> CAUGHT EXPECTED EventFullException: " << e.what() << "\n";
    }

    // Test D: PaymentFailedException (Invalid card number length)
    try {
        std::cout << "[Check D] Testing Card Payment with invalid 10-digit card number...\n";
        auto paidEnrollment = enrollmentService.enroll(s3->getId(), ev2->getEventId());
        nlohmann::json badCard = {{"cardNumber", "1234567890"}, {"cardHolderName", "Rohan"}};
        paymentService.initiatePayment(paidEnrollment->getEnrollmentId(), "Card", badCard);
    } catch (const PaymentFailedException& e) {
        std::cout << "  --> CAUGHT EXPECTED PaymentFailedException: " << e.what() << "\n";
    }

    // 4. OPERATOR OVERLOADING: Team operator+
    std::cout << "\n--- 4. Testing Team operator+ Overloading ---\n";
    Team team = (*s1) + (*s2); // Merges student 1 and student 2
    std::cout << "Formed team: '" << team.getTeamName() << "' with members: ";
    for (const auto& m : team.getMemberNames()) {
        std::cout << m << " ";
    }
    std::cout << "\n";

    team = team + (*s3); // Merges student 3 into existing team
    std::cout << "Expanded team: '" << team.getTeamName() << "' with members: ";
    for (const auto& m : team.getMemberNames()) {
        std::cout << m << " ";
    }
    std::cout << "\n";

    std::cout << "\n>>> Phase 2 Verification Completed Successfully! <<<\n";
    return 0;
}
