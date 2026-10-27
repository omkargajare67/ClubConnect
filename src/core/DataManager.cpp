#include "core/DataManager.h"
#include "core/FileManager.h"
#include "users/UserFactory.h"
#include "events/EventFactory.h"
#include "payments/PaymentFactory.h"
#include "services/EnrollmentService.h"
#include "services/PaymentService.h"
#include <algorithm>
#include <iostream>

DataManager::DataManager(std::shared_ptr<Repository<User>> userRepo,
                         std::shared_ptr<Repository<Event>> eventRepo,
                         std::shared_ptr<Repository<Club>> clubRepo,
                         std::shared_ptr<Repository<Enrollment>> enrollmentRepo,
                         std::shared_ptr<Repository<Receipt>> receiptRepo,
                         std::shared_ptr<Repository<Payment>> paymentRepo)
    : userRepo(userRepo), eventRepo(eventRepo), clubRepo(clubRepo),
      enrollmentRepo(enrollmentRepo), receiptRepo(receiptRepo), paymentRepo(paymentRepo) {}

void DataManager::loadAll() {
    FileManager& fm = FileManager::getInstance();

    // 1. Load Clubs
    clubRepo->clear();
    nlohmann::json clubsJson = fm.load("clubs.json");
    for (const auto& item : clubsJson) {
        clubRepo->add(std::make_shared<Club>(Club::fromJson(item)));
    }

    // 2. Load Users
    userRepo->clear();
    nlohmann::json usersJson = fm.load("users.json");
    for (const auto& item : usersJson) {
        try {
            userRepo->add(UserFactory::createUser(item));
        } catch (const std::exception& e) {
            std::cerr << "[DataManager] Error loading user: " << e.what() << std::endl;
        }
    }

    // 3. Load Events
    eventRepo->clear();
    int maxEventId = 0;
    nlohmann::json eventsJson = fm.load("events.json");
    for (const auto& item : eventsJson) {
        try {
            auto ev = EventFactory::createEvent(item);
            eventRepo->add(ev);
            if (ev->getEventId() > maxEventId) {
                maxEventId = ev->getEventId();
            }
        } catch (const std::exception& e) {
            std::cerr << "[DataManager] Error loading event: " << e.what() << std::endl;
        }
    }
    // RESET STATIC COUNTER: Ensure new events get ID > max loaded ID
    Event::nextEventId = maxEventId + 1;

    // 4. Load Enrollments
    enrollmentRepo->clear();
    int maxEnrollmentId = 0;
    nlohmann::json enrollmentsJson = fm.load("enrollments.json");
    for (const auto& item : enrollmentsJson) {
        auto en = std::make_shared<Enrollment>(Enrollment::fromJson(item));
        enrollmentRepo->add(en);
        if (en->getEnrollmentId() > maxEnrollmentId) {
            maxEnrollmentId = en->getEnrollmentId();
        }
    }
    EnrollmentService::nextEnrollmentId = maxEnrollmentId + 1;

    // 5. Load Payments
    paymentRepo->clear();
    int maxTxnId = 5000;
    nlohmann::json paymentsJson = fm.load("payments.json");
    for (const auto& item : paymentsJson) {
        try {
            auto p = PaymentFactory::createPayment(item);
            paymentRepo->add(p);
            std::string tid = p->getTransactionId();
            if (tid.rfind("TXN-", 0) == 0) {
                try {
                    int num = std::stoi(tid.substr(4));
                    if (num > maxTxnId) maxTxnId = num;
                } catch (...) {}
            }
        } catch (const std::exception& e) {
            std::cerr << "[DataManager] Error loading payment: " << e.what() << std::endl;
        }
    }
    PaymentService::nextTxnCounter = maxTxnId + 1;

    // 6. Load Receipts
    receiptRepo->clear();
    int maxReceiptNo = 1000;
    nlohmann::json receiptsJson = fm.load("receipts.json");
    for (const auto& item : receiptsJson) {
        auto rcpt = std::make_shared<Receipt>(Receipt::fromJson(item));
        receiptRepo->add(rcpt);
        if (rcpt->getReceiptNo() > maxReceiptNo) {
            maxReceiptNo = rcpt->getReceiptNo();
        }
    }
    Receipt::nextReceiptNo = maxReceiptNo + 1;
}

void DataManager::saveUsers() {
    nlohmann::json arr = nlohmann::json::array();
    for (const auto& u : userRepo->getAll()) {
        nlohmann::json j = u->toJson();
        j["password"] = u->getPasswordHash();
        j["isHashed"] = true;
        arr.push_back(j);
    }
    FileManager::getInstance().save("users.json", arr);
}

void DataManager::saveEvents() {
    nlohmann::json arr = nlohmann::json::array();
    for (const auto& e : eventRepo->getAll()) {
        arr.push_back(e->toJson());
    }
    FileManager::getInstance().save("events.json", arr);
}

void DataManager::saveClubs() {
    nlohmann::json arr = nlohmann::json::array();
    for (const auto& c : clubRepo->getAll()) {
        arr.push_back(c->toJson());
    }
    FileManager::getInstance().save("clubs.json", arr);
}

void DataManager::saveEnrollments() {
    nlohmann::json arr = nlohmann::json::array();
    for (const auto& en : enrollmentRepo->getAll()) {
        arr.push_back(en->toJson());
    }
    FileManager::getInstance().save("enrollments.json", arr);
}

void DataManager::saveReceipts() {
    nlohmann::json arr = nlohmann::json::array();
    for (const auto& r : receiptRepo->getAll()) {
        arr.push_back(r->toJson());
    }
    FileManager::getInstance().save("receipts.json", arr);
}

void DataManager::savePayments() {
    nlohmann::json arr = nlohmann::json::array();
    for (const auto& p : paymentRepo->getAll()) {
        arr.push_back(p->toJson());
    }
    FileManager::getInstance().save("payments.json", arr);
}

void DataManager::saveAll() {
    saveUsers();
    saveEvents();
    saveClubs();
    saveEnrollments();
    saveReceipts();
    savePayments();
}
