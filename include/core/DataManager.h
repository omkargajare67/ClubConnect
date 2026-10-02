#ifndef DATA_MANAGER_H
#define DATA_MANAGER_H

#include <memory>
#include "core/Repository.h"
#include "users/User.h"
#include "events/Event.h"
#include "records/Club.h"
#include "records/Enrollment.h"
#include "records/Receipt.h"
#include "records/Certificate.h"
#include "payments/Payment.h"

// FACADE & PERSISTENCE ORCHESTRATION: Coordinates initial startup hydration and automatic disk syncing
// between in-memory generic repositories and JSON persistence files via FileManager singleton.
class DataManager {
private:
    std::shared_ptr<Repository<User>> userRepo;
    std::shared_ptr<Repository<Event>> eventRepo;
    std::shared_ptr<Repository<Club>> clubRepo;
    std::shared_ptr<Repository<Enrollment>> enrollmentRepo;
    std::shared_ptr<Repository<Receipt>> receiptRepo;
    std::shared_ptr<Repository<Payment>> paymentRepo;

public:
    DataManager(std::shared_ptr<Repository<User>> userRepo,
                std::shared_ptr<Repository<Event>> eventRepo,
                std::shared_ptr<Repository<Club>> clubRepo,
                std::shared_ptr<Repository<Enrollment>> enrollmentRepo,
                std::shared_ptr<Repository<Receipt>> receiptRepo,
                std::shared_ptr<Repository<Payment>> paymentRepo);

    // Loads all repositories from data/*.json on startup and resets static ID counters
    void loadAll();

    // Saves specific repositories to data/*.json
    void saveUsers();
    void saveEvents();
    void saveClubs();
    void saveEnrollments();
    void saveReceipts();
    void savePayments();
    void saveAll();
};

#endif // DATA_MANAGER_H
