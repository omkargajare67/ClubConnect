# ClubConnect - Campus Club Events & Enrollment Portal

A robust, production-grade Object-Oriented Programming (OOP) platform built in modern **C++17** with a responsive **HTML5/Vanilla CSS/JavaScript + Bootstrap 5** web frontend. Engineered for CS engineering students to easily demonstrate and explain core OOP principles to professors.

---

## Access the Portal
When running locally, open your web browser at:
👉 **`http://localhost:8080`**

---

## Demo Login Credentials

| Role | Name | Email | Password | Details |
|---|---|---|---|---|
| **SuperAdmin** | Dr. Arvind Sharma | `superadmin@campus.edu` | `superpassword` | Global event approval / rejection, financial reports & analytics |
| **ClubAdmin** | Prof. Rajesh Nair | `admin.coding@campus.edu` | `codingpass` | Manages **"Coding Club"** events & attendance only |
| **ClubAdmin** | Prof. Sneha Kulkarni | `admin.robotics@campus.edu` | `robopass` | Manages **"Robotics Society"** events & attendance only |
| **ClubAdmin** | Prof. Vikram Sarabhai | `admin.ai@campus.edu` | `aipass` | Manages **"AI & Machine Learning Club"** events & attendance only |
| **ClubAdmin** | Prof. Meera Sen | `admin.cultural@campus.edu` | `culturalpass` | Manages **"Cultural & Fine Arts Society"** events & attendance only |
| **ClubAdmin** | Coach Sandeep Patil | `admin.sports@campus.edu` | `sportspass` | Manages **"Sports & Athletics Council"** events & attendance only |
| **ClubAdmin** | Prof. Tanya Verma | `admin.design@campus.edu` | `designpass` | Manages **"Design & Media Guild"** events & attendance only |
| **ClubAdmin** | Prof. Arjun Dave | `admin.cyber@campus.edu` | `cyberpass` | Manages **"Cyber Security & Ethical Hacking"** events & attendance only |
| **ClubAdmin** | Prof. Homi Bhabha | `admin.aero@campus.edu` | `aeropass` | Manages **"Aerospace & Astronomy Club"** events & attendance only |
| **MemberStudent** | Aarav Sharma | `aarav@campus.edu` | `pass123` | CS Dept &bull; **20% membership discount** applied automatically |
| **MemberStudent** | Neha Gupta | `neha@campus.edu` | `pass123` | EC Dept &bull; **20% membership discount** applied automatically |
| **GuestStudent** | Diya Patel | `diya@other.edu` | `pass123` | External Student &bull; Full fee (100%) |
| **GuestStudent** | Kabir Mehta | `kabir@other.edu` | `pass123` | External Student &bull; Full fee (100%) |


---

## Compilation & Execution

### Windows (MinGW-w64 / MSYS2 UCRT64)
```bash
g++ -std=c++17 -Wall -Wextra main.cpp src/*/*.cpp -Iinclude -Ithird_party -o clubconnect -pthread -lws2_32 -lmswsock
./clubconnect
```

### Linux / macOS
```bash
g++ -std=c++17 -Wall -Wextra main.cpp src/*/*.cpp -Iinclude -Ithird_party -o clubconnect -pthread
./clubconnect
```

### Running Automated Test Suites
```bash
# Verify end-to-end API routes and error status codes (200, 201, 401, 402, 403, 404, 409):
./test_api.sh

# Verify full Definition of Done student/admin workflow:
./test_dod.sh
```

---

## Layered Architecture

ClubConnect strictly adheres to a unidirectional 5-layer separation of concerns. Each layer only calls the layer directly below it:

```
[ Browser / Frontend Client ] (Plain HTML5, CSS3, Vanilla JS, Bootstrap 5)
            │
            ▼
[ API Routes ] (src/api/Routes.cpp)
            │   - Thin HTTP handlers (max ~15 lines each)
            │   - Parses JSON & extracts tokens
            │   - Converts domain exceptions to HTTP status codes (401, 402, 403, 404, 409)
            ▼
[ Application Services ] (src/services/*.cpp)
            │   - AuthService, EnrollmentService, PaymentService, EventService
            │   - Enforces business rules & thread safety via std::mutex
            ▼
[ OOP Core Entities ] (src/users/*.cpp, src/events/*.cpp, src/payments/*.cpp, src/records/*.cpp)
            │   - Domain models, polymorphism, operators, encapsulation
            │   - Zero web server dependencies
            ▼
[ Persistence & Data Layer ] (src/core/FileManager.cpp, src/core/DataManager.cpp)
            │   - Thread-safe Singleton FileManager touching data/*.json
            ▼
[ JSON Storage ] (data/users.json, events.json, clubs.json, enrollments.json, payments.json, receipts.json)
```

---

## OOP Concepts Mapping Table

| OOP Concept | File / Location | How It Is Demonstrated in ClubConnect |
|---|---|---|
| **Abstraction** | `include/users/User.h`<br>`include/events/Event.h`<br>`include/payments/Payment.h` | Pure abstract base classes (`User`, `Event`, `Payment`) defining interfaces (`getRole()`, `toJson()`, `processPayment()`) with virtual destructors and no concrete instances. |
| **Encapsulation** | `Event::seatsLeft`<br>`Payment::amount`<br>`User::passwordHash`<br>`Receipt::amountPaid` | Sensitive variables are strictly `private`. Seats can only decrement via `enroll()`. Financial transaction status can only transition inside `processPayment()`. Passwords are only stored as hashes. |
| **Inheritance** | `Student : User`<br>`MemberStudent : Student`<br>`Workshop : Event`<br>`UPIPayment : Payment` | Hierarchies for user roles, student membership tiers, specialized events (`Workshop`, `Hackathon`, `Seminar`, `CulturalEvent`), and payment channels (`UPIPayment`, `CardPayment`, `CashPayment`). |
| **Polymorphism (Dynamic Binding)** | `Event::displayDetails()`<br>`Event::toJson()`<br>`Student::calculateFee()`<br>`Payment::processPayment()` | Runtime dynamic dispatch via `virtual` and `override`. `MemberStudent` overrides `calculateFee()` with a 20% discount; `Workshop` and `Hackathon` generate unique certificate types. |
| **Operator Overloading** | `Event::operator<`<br>`Event::operator<<`<br>`Receipt::operator<<`<br>`Team::operator+` | `Event::operator<` sorts events chronologically by date. `Event::operator<<` and `Receipt::operator<<` format output streams. `Team::operator+` merges two `Student` objects into a collaborative hackathon team. |
| **Function Overloading** | `EventSearch::searchEvent(...)` in `include/events/EventSearch.h` | Three distinct overloaded query signatures: search by event title (`string`), by host entity (`Club`), and by budget ceiling (`double maxFee`). |
| **Templates / Generics** | `include/core/Repository.h` | Generic `Repository<T>` storing `std::shared_ptr<T>`, providing thread-safe CRUD operations, lambda search (`find`), and filtering (`filter`) without code duplication. |
| **Friend Functions** | `Receipt.h`<br>`Team.h` | `friend void printReceiptWithPayment(...)` grants `Receipt` privileged access to inspect private `Payment` details for audit vouchers. `friend Team operator+(const Student&, const Student&)` merges students. |
| **Singleton Pattern** | `include/core/FileManager.h` | `FileManager::getInstance()` ensures a single point of serialized disk access guarded by mutexes. |
| **Factory Pattern** | `EventFactory`<br>`UserFactory`<br>`PaymentFactory` | Static factory methods dynamically instantiate correct polymorphic subclasses from raw JSON records. |
| **Exception Handling** | `include/core/Exceptions.h` | Custom domain exception hierarchy deriving from `std::exception`: `EventFullException` (409), `AlreadyEnrolledException` (409), `TimeClashException` (409), `PaymentFailedException` (402). |

---

## Business Rules Implemented
1. **Seat Capacity & Race Conditions**: `seatsLeft` changes only via `Event::enroll()`. Multi-threaded enrollment in `EnrollmentService` uses `std::lock_guard<std::mutex>` so two concurrent requests cannot take the last seat.
2. **Duplicate & Clash Prevention**: Students cannot enroll in the same event twice (`AlreadyEnrolledException`), nor can they enroll in events occurring at the exact same date and time (`TimeClashException`).
3. **No Cancellation / No Refund**: Enrollment is final. The frontend renders a confirmation modal before payment: *"This enrollment cannot be changed. Proceed?"*
4. **Free Event Exemption**: Events with `fee = 0` bypass payment channels directly, immediately issuing a free voucher receipt.
5. **Attendance-Bound Certificates**: Certificates are issued **only** to students marked present by the club administrator.
6. **Simulated Payment Gateway**: Card numbers must be 16 digits (server stores only the last 4 digits). UPI generates authentic deep link QR codes (`upi://pay?pa=clubfund@upi...`). Payment confirmation is simulated with clear academic notices.

---

## UML Class Diagram
The full PlantUML source code is available at [`docs/class_diagram.puml`](file:///docs/class_diagram.puml). Render using PlantUML extension in VS Code or online at [plantuml.com](http://www.plantuml.com).
