# ClubConnect - Campus Club Events & Enrollment Portal

A layered Object-Oriented Programming (OOP) project in modern C++17 with a responsive web interface. Built for demonstrating core OOP concepts cleanly and elegantly to CS engineering professors and peers.

## Compilation & Execution

### Build Command (Windows MinGW / MSYS2 UCRT64)
```bash
g++ -std=c++17 -Wall -Wextra main.cpp src/*/*.cpp -Iinclude -Ithird_party -o clubconnect -pthread -lws2_32 -lmswsock
```

### Build Command (Linux / macOS)
```bash
g++ -std=c++17 -Wall -Wextra main.cpp src/*/*.cpp -Iinclude -Ithird_party -o clubconnect -pthread
```

### Run
```bash
./clubconnect
```
The web portal will be accessible at: `http://localhost:8080`

---

## OOP Concepts Mapping

| OOP Concept | File / Location | Description & Purpose |
|---|---|---|
| **Abstraction** | `include/users/User.h`, `include/events/Event.h`, `include/payments/Payment.h` | Pure abstract base classes defining interfaces (`getRole()`, `showMenu()`, `toJson()`, `processPayment()`) with virtual destructors. |
| **Encapsulation** | All classes in `include/` | Private fields with strict const getters and guarded mutating methods (e.g. `seatsLeft` only modified via `enroll()`). |
| **Inheritance** | `Student`, `MemberStudent`, `GuestStudent`, `ClubAdmin`, `SuperAdmin`, `Workshop`, `Hackathon`, `Seminar`, `CulturalEvent` | Hierarchies representing user roles, discount models, and event types. |
| **Polymorphism** | `displayDetails()`, `toJson()`, `calculateFee()`, `generateCertificate()`, `processPayment()` | Runtime dynamic dispatch via `virtual` / `override` keywords. |
| **Operator Overloading** | `Event::operator<`, `Event::operator<<`, `Receipt::operator<<`, `Team::operator+` | Custom operators for sorting events by date, printing entities to stream, and merging students into teams. |
| **Function Overloading** | `EventRepository` / `searchEvent(...)` | Overloaded search functions by name (`std::string`), by club (`Club`), and by budget (`double maxFee`). |
| **Templates** | `include/core/Repository.h` | Generic `Repository<T>` storing and querying `std::shared_ptr<T>`. |
| **Friend Functions** | `include/records/Receipt.h` | `friend void printReceiptDetails(...)` accessing private payment details. |
| **Design Patterns** | `FileManager` (Singleton), `EventFactory` (Factory Method) | Singleton for thread-safe centralized persistence; Factory for instantiating dynamic Event subclasses from JSON. |
| **Exception Handling** | `include/core/Exceptions.h` | Custom exception hierarchy deriving from `std::exception` (`EventFullException`, `AlreadyEnrolledException`, `TimeClashException`, `PaymentFailedException`). |

---

## Demo Credentials
*(Populated in Phase 3)*

## Architecture Layers
Browser -> Routes (`api/Routes.cpp`) -> Services -> OOP core classes -> FileManager -> `data/*.json`
