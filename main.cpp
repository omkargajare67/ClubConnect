#include <iostream>
#include <memory>
#include <vector>
#include <algorithm>
#include "core/Repository.h"
#include "core/FileManager.h"
#include "core/DataManager.h"
#include "events/Event.h"
#include "events/Workshop.h"
#include "events/Hackathon.h"
#include "events/EventSearch.h"
#include "users/User.h"
#include "records/Club.h"

int main() {
    std::cout << "========================================================\n";
    std::cout << "       ClubConnect - Phase 3 Verification Demo          \n";
    std::cout << "========================================================\n\n";

    // Initialize generic repositories
    auto userRepo = std::make_shared<Repository<User>>();
    auto eventRepo = std::make_shared<Repository<Event>>();
    auto clubRepo = std::make_shared<Repository<Club>>();
    auto enrollmentRepo = std::make_shared<Repository<Enrollment>>();
    auto receiptRepo = std::make_shared<Repository<Receipt>>();
    auto paymentRepo = std::make_shared<Repository<Payment>>();

    DataManager dataManager(userRepo, eventRepo, clubRepo, enrollmentRepo, receiptRepo, paymentRepo);

    // 1. PERSISTENCE & COUNTER RESET: Load seed data from disk
    std::cout << "--- 1. Loading Seed Data from data/*.json via FileManager Singleton ---\n";
    dataManager.loadAll();

    std::cout << "Loaded " << userRepo->size() << " users.\n";
    std::cout << "Loaded " << clubRepo->size() << " clubs.\n";
    std::cout << "Loaded " << eventRepo->size() << " events.\n";
    std::cout << "Next Event ID after counter reset: " << Event::nextEventId << "\n";
    std::cout << "Next Receipt No after counter reset: " << Receipt::nextReceiptNo << "\n\n";

    auto events = eventRepo->getAll();

    // 2. FUNCTION OVERLOADING: Testing EventSearch with 3 distinct signatures
    std::cout << "--- 2. Testing Function Overloading: searchEvent(...) ---\n";
    
    // Overload 1: by string keyword
    std::cout << "[Overload 1: Keyword 'Workshop']\n";
    auto keywordMatches = EventSearch::searchEvent(events, "Workshop");
    for (const auto& ev : keywordMatches) {
        std::cout << "  Found: " << *ev << "\n";
    }

    // Overload 2: by Club object
    auto codingClubPtr = clubRepo->find([](const std::shared_ptr<Club>& c) {
        return c->getName() == "Coding Club";
    });
    if (codingClubPtr) {
        std::cout << "\n[Overload 2: By Club Object ('" << codingClubPtr->getName() << "')]\n";
        auto clubMatches = EventSearch::searchEvent(events, *codingClubPtr);
        for (const auto& ev : clubMatches) {
            std::cout << "  Found: " << *ev << "\n";
        }
    }

    // Overload 3: by budget / max fee
    std::cout << "\n[Overload 3: By Max Fee <= INR 200.00]\n";
    auto budgetMatches = EventSearch::searchEvent(events, 200.0);
    for (const auto& ev : budgetMatches) {
        std::cout << "  Found: " << *ev << "\n";
    }

    // 3. OPERATOR OVERLOADING & LAMBDAS: Sorting Events
    std::cout << "\n--- 3. Testing Event Sorting: operator< (Date) vs Lambda (Fee) ---\n";
    
    // Sort by Date using operator<
    std::vector<std::shared_ptr<Event>> sortedByDate = events;
    std::sort(sortedByDate.begin(), sortedByDate.end(), [](const std::shared_ptr<Event>& a, const std::shared_ptr<Event>& b) {
        return (*a) < (*b); // Invokes Event::operator<
    });
    std::cout << "Sorted by Date (using operator<):\n";
    for (const auto& ev : sortedByDate) {
        std::cout << "  " << ev->getDate() << " " << ev->getTime() << " | " << ev->getName() << "\n";
    }

    // Sort by Fee using a Lambda comparator
    std::vector<std::shared_ptr<Event>> sortedByFee = events;
    std::sort(sortedByFee.begin(), sortedByFee.end(), [](const std::shared_ptr<Event>& a, const std::shared_ptr<Event>& b) {
        return a->getFee() < b->getFee(); // Lambda comparator
    });
    std::cout << "\nSorted by Fee (using Lambda):\n";
    for (const auto& ev : sortedByFee) {
        std::cout << "  INR " << ev->getFee() << " | " << ev->getName() << "\n";
    }

    // 4. PERSISTENCE SURVIVAL TEST (SIMULATE RESTART)
    std::cout << "\n--- 4. Testing Persistence: Adding Item, Saving, and Reloading ---\n";
    int newEventId = Event::nextEventId++;
    auto persistentEvent = std::make_shared<Workshop>(
        newEventId, "Persistence Verification Workshop", "Testing disk persistence across simulated server restarts",
        "Coding Club", "2026-11-20", "15:00", "Lab 204", 150.0, 25, "Dr. Persistence", 25, "APPROVED"
    );
    eventRepo->add(persistentEvent);
    std::cout << "Added new event #" << newEventId << " to repository. Saving to disk...\n";
    dataManager.saveEvents();

    std::cout << "Simulating server shutdown: clearing in-memory repositories...\n";
    eventRepo->clear();
    std::cout << "In-memory events count after clear: " << eventRepo->size() << "\n";

    std::cout << "Simulating server restart: reloading from data/events.json...\n";
    dataManager.loadAll();
    std::cout << "In-memory events count after restart: " << eventRepo->size() << "\n";

    auto reloadedEvent = eventRepo->find([newEventId](const std::shared_ptr<Event>& e) {
        return e->getEventId() == newEventId;
    });

    if (reloadedEvent) {
        std::cout << "SUCCESS: Newly added event #" << newEventId << " survived server restart!\n";
        std::cout << "Reloaded Details: " << *reloadedEvent << "\n";
        // Clean up test event so seed data remains clean for subsequent phases
        eventRepo->remove([newEventId](const std::shared_ptr<Event>& e) {
            return e->getEventId() == newEventId;
        });
        dataManager.saveEvents();
        std::cout << "Cleaned up temporary test event. Seed events restored to " << eventRepo->size() << ".\n";
    } else {
        std::cerr << "FAILURE: Event did not survive restart!\n";
        return 1;
    }

    std::cout << "\n>>> Phase 3 Verification Completed Successfully! <<<\n";
    return 0;
}
