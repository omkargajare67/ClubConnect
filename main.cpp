#include <iostream>
#include <memory>
#include "httplib.h"
#include "core/Repository.h"
#include "core/DataManager.h"
#include "services/AuthService.h"
#include "services/EventService.h"
#include "services/EnrollmentService.h"
#include "services/PaymentService.h"
#include "api/Routes.h"

int main() {
    std::cout << "========================================================\n";
    std::cout << "     ClubConnect - Campus Club Events & Enrollment     \n";
    std::cout << "       C++17 OOP Backend Server with cpp-httplib       \n";
    std::cout << "========================================================\n";

    // 1. Initialize generic in-memory repositories
    auto userRepo = std::make_shared<Repository<User>>();
    auto eventRepo = std::make_shared<Repository<Event>>();
    auto clubRepo = std::make_shared<Repository<Club>>();
    auto enrollmentRepo = std::make_shared<Repository<Enrollment>>();
    auto receiptRepo = std::make_shared<Repository<Receipt>>();
    auto paymentRepo = std::make_shared<Repository<Payment>>();

    // 2. Hydrate from disk via FileManager singleton
    DataManager dataManager(userRepo, eventRepo, clubRepo, enrollmentRepo, receiptRepo, paymentRepo);
    dataManager.loadAll();

    std::cout << "[DataManager] Loaded " << userRepo->size() << " users, "
              << clubRepo->size() << " clubs, "
              << eventRepo->size() << " events from disk.\n";

    // 3. Initialize domain services
    AuthService authService(userRepo);
    EnrollmentService enrollmentService(eventRepo, userRepo, enrollmentRepo);
    PaymentService paymentService(paymentRepo, receiptRepo, enrollmentRepo, eventRepo, userRepo);
    EventService eventService(eventRepo, enrollmentRepo, receiptRepo, userRepo, clubRepo);

    // 4. Configure HTTP server and API routes
    httplib::Server svr;
    Routes::setup(svr, authService, eventService, enrollmentService, paymentService, dataManager);

    const std::string host = "0.0.0.0";
    const int port = 8080;
    std::cout << "[Server] ClubConnect server listening on http://localhost:" << port << "\n";
    std::cout << "[Server] Ready to accept web requests...\n";

    if (!svr.listen(host, port)) {
        std::cerr << "[Server] Failed to bind to " << host << ":" << port << std::endl;
        return 1;
    }

    return 0;
}
