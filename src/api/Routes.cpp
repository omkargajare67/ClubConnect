#include "api/Routes.h"
#include "core/Exceptions.h"
#include "users/ClubAdmin.h"
#include <iostream>

// Helper: Extracts Bearer authorization token
static std::string extractToken(const httplib::Request& req) {
    if (req.has_header("Authorization")) {
        std::string auth = req.get_header_value("Authorization");
        if (auth.rfind("Bearer ", 0) == 0) {
            return auth.substr(7);
        }
        return auth;
    }
    return "";
}

// Helper: Centralized role and authentication guard
static bool requireAuth(const httplib::Request& req, httplib::Response& res,
                        AuthService& auth, const std::string& requiredRole,
                        std::shared_ptr<User>& outUser) {
    std::string token = extractToken(req);
    outUser = auth.validateToken(token);
    if (!outUser) {
        res.status = 401;
        res.set_content(nlohmann::json({{"error", "Unauthorized: Invalid or missing token."}}).dump(), "application/json");
        return false;
    }
    if (!requiredRole.empty() && outUser->getRole() != requiredRole) {
        res.status = 403;
        res.set_content(nlohmann::json({{"error", "Forbidden: Access denied for role " + outUser->getRole()}}).dump(), "application/json");
        return false;
    }
    return true;
}

// Helper: Maps domain exceptions directly to standard HTTP status codes
static void handleRouteException(httplib::Response& res, const std::exception& e) {
    if (dynamic_cast<const EventFullException*>(&e)) {
        res.status = 409;
    } else if (dynamic_cast<const AlreadyEnrolledException*>(&e)) {
        res.status = 409;
    } else if (dynamic_cast<const TimeClashException*>(&e)) {
        res.status = 409;
    } else if (dynamic_cast<const PaymentFailedException*>(&e)) {
        res.status = 402;
    } else {
        std::string msg = e.what();
        res.status = (msg.find("not found") != std::string::npos) ? 404 : 400;
    }
    res.set_content(nlohmann::json({{"error", e.what()}}).dump(), "application/json");
}

void Routes::setup(httplib::Server& svr,
                   AuthService& authService,
                   EventService& eventService,
                   EnrollmentService& enrollmentService,
                   PaymentService& paymentService,
                   DataManager& dataManager) {

    // CORS Preflight & Headers
    svr.set_pre_routing_handler([](const httplib::Request& req, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
        res.set_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
        if (req.method == "OPTIONS") {
            res.status = 204;
            return httplib::Server::HandlerResponse::Handled;
        }
        return httplib::Server::HandlerResponse::Unhandled;
    });

    // 1. POST /api/register
    svr.Post("/api/register", [&](const httplib::Request& req, httplib::Response& res) {
        try {
            auto body = nlohmann::json::parse(req.body);
            auto user = authService.registerStudent(body);
            std::string token = authService.login(user->getEmail(), body.value("password", "pass123"));
            dataManager.saveUsers();
            res.status = 201;
            res.set_content(nlohmann::json({{"token", token}, {"role", user->getRole()}, {"name", user->getName()}, {"id", user->getId()}}).dump(), "application/json");
        } catch (const std::exception& e) { handleRouteException(res, e); }
    });

    // 2. POST /api/login
    svr.Post("/api/login", [&](const httplib::Request& req, httplib::Response& res) {
        try {
            auto body = nlohmann::json::parse(req.body);
            std::string token = authService.login(body.value("email", ""), body.value("password", ""));
            auto user = authService.validateToken(token);
            res.set_content(nlohmann::json({{"token", token}, {"role", user->getRole()}, {"name", user->getName()}, {"id", user->getId()}, {"email", user->getEmail()}}).dump(), "application/json");
        } catch (const std::exception& e) {
            res.status = 401;
            res.set_content(nlohmann::json({{"error", e.what()}}).dump(), "application/json");
        }
    });

    // 3. GET /api/events (Approved only; search & sort)
    svr.Get("/api/events", [&](const httplib::Request& req, httplib::Response& res) {
        std::string name = req.has_param("name") ? req.get_param_value("name") : "";
        std::string club = req.has_param("club") ? req.get_param_value("club") : "";
        std::string sort = req.has_param("sort") ? req.get_param_value("sort") : "date";
        double maxFee = req.has_param("maxFee") ? std::stod(req.get_param_value("maxFee")) : -1.0;

        auto events = eventService.getApprovedEvents(name, club, maxFee, sort);
        nlohmann::json arr = nlohmann::json::array();
        for (const auto& ev : events) arr.push_back(ev->toJson());
        res.set_content(arr.dump(), "application/json");
    });

    // 4. POST /api/events (ClubAdmin: creates PENDING)
    svr.Post("/api/events", [&](const httplib::Request& req, httplib::Response& res) {
        std::shared_ptr<User> user;
        if (!requireAuth(req, res, authService, "ClubAdmin", user)) return;
        try {
            auto admin = std::dynamic_pointer_cast<ClubAdmin>(user);
            auto newEvent = eventService.createEvent(nlohmann::json::parse(req.body), admin ? admin->getClubName() : "");
            dataManager.saveEvents();
            res.status = 201;
            res.set_content(nlohmann::json({{"success", true}, {"event", newEvent->toJson()}}).dump(), "application/json");
        } catch (const std::exception& e) { handleRouteException(res, e); }
    });

    // 5. GET /api/events/pending (SuperAdmin)
    svr.Get("/api/events/pending", [&](const httplib::Request& req, httplib::Response& res) {
        std::shared_ptr<User> user;
        if (!requireAuth(req, res, authService, "SuperAdmin", user)) return;
        auto pending = eventService.getPendingEvents();
        nlohmann::json arr = nlohmann::json::array();
        for (const auto& ev : pending) arr.push_back(ev->toJson());
        res.set_content(arr.dump(), "application/json");
    });

    // 6. POST /api/events/:id/approve (SuperAdmin)
    svr.Post(R"(/api/events/(\d+)/approve)", [&](const httplib::Request& req, httplib::Response& res) {
        std::shared_ptr<User> user;
        if (!requireAuth(req, res, authService, "SuperAdmin", user)) return;
        int eventId = std::stoi(req.matches[1]);
        eventService.setEventStatus(eventId, "APPROVED");
        dataManager.saveEvents();
        res.set_content(nlohmann::json({{"success", true}, {"status", "APPROVED"}}).dump(), "application/json");
    });

    // 7. POST /api/events/:id/reject (SuperAdmin)
    svr.Post(R"(/api/events/(\d+)/reject)", [&](const httplib::Request& req, httplib::Response& res) {
        std::shared_ptr<User> user;
        if (!requireAuth(req, res, authService, "SuperAdmin", user)) return;
        int eventId = std::stoi(req.matches[1]);
        eventService.setEventStatus(eventId, "REJECTED");
        dataManager.saveEvents();
        res.set_content(nlohmann::json({{"success", true}, {"status", "REJECTED"}}).dump(), "application/json");
    });

    // 8. POST /api/enroll (Student)
    svr.Post("/api/enroll", [&](const httplib::Request& req, httplib::Response& res) {
        std::shared_ptr<User> user;
        if (!requireAuth(req, res, authService, "Student", user)) return;
        try {
            auto body = nlohmann::json::parse(req.body);
            auto enrollment = enrollmentService.enroll(user->getId(), body.value("eventId", 0));
            dataManager.saveEnrollments();
            dataManager.saveEvents();
            res.status = 201;
            res.set_content(enrollment->toJson().dump(), "application/json");
        } catch (const std::exception& e) { handleRouteException(res, e); }
    });

    // 9. POST /api/pay/initiate (Student)
    svr.Post("/api/pay/initiate", [&](const httplib::Request& req, httplib::Response& res) {
        std::shared_ptr<User> user;
        if (!requireAuth(req, res, authService, "Student", user)) return;
        try {
            auto body = nlohmann::json::parse(req.body);
            auto payment = paymentService.initiatePayment(body.value("enrollmentId", 0), body.value("mode", "UPI"), body.value("details", nlohmann::json::object()));
            dataManager.savePayments();
            dataManager.saveEnrollments();
            dataManager.saveReceipts();
            res.set_content(nlohmann::json({{"paymentId", payment->getTransactionId()}, {"instruction", payment->getPaymentInstruction()}, {"amount", payment->getAmount()}, {"status", payment->getStatus()}}).dump(), "application/json");
        } catch (const std::exception& e) { handleRouteException(res, e); }
    });

    // 10. POST /api/pay/confirm (Student)
    svr.Post("/api/pay/confirm", [&](const httplib::Request& req, httplib::Response& res) {
        std::shared_ptr<User> user;
        if (!requireAuth(req, res, authService, "Student", user)) return;
        try {
            auto body = nlohmann::json::parse(req.body);
            auto receipt = paymentService.confirmPayment(body.value("paymentId", ""));
            dataManager.savePayments();
            dataManager.saveReceipts();
            dataManager.saveEnrollments();
            res.set_content(nlohmann::json({{"receiptId", receipt->getReceiptNo()}, {"receiptCode", receipt->getReceiptCode()}, {"receipt", receipt->toJson()}}).dump(), "application/json");
        } catch (const std::exception& e) { handleRouteException(res, e); }
    });

    // 11. GET /api/my/enrollments (Student)
    svr.Get("/api/my/enrollments", [&](const httplib::Request& req, httplib::Response& res) {
        std::shared_ptr<User> user;
        if (!requireAuth(req, res, authService, "Student", user)) return;
        auto list = enrollmentService.getStudentEnrollments(user->getId());
        nlohmann::json arr = nlohmann::json::array();
        for (const auto& en : list) {
            nlohmann::json j = en->toJson();
            auto rcpt = paymentService.getReceiptByEnrollmentId(en->getEnrollmentId());
            j["hasReceipt"] = (rcpt != nullptr);
            if (rcpt) j["receiptNo"] = rcpt->getReceiptNo();
            arr.push_back(j);
        }
        res.set_content(arr.dump(), "application/json");
    });

    // 12. GET /api/receipt/:id
    svr.Get(R"(/api/receipt/(\d+))", [&](const httplib::Request& req, httplib::Response& res) {
        int rId = std::stoi(req.matches[1]);
        auto rcpt = paymentService.getReceiptById(rId);
        if (!rcpt) {
            res.status = 404;
            res.set_content(nlohmann::json({{"error", "Receipt not found."}}).dump(), "application/json");
            return;
        }
        res.set_content(rcpt->toJson().dump(), "application/json");
    });

    // 13. POST /api/events/:id/attendance (ClubAdmin)
    svr.Post(R"(/api/events/(\d+)/attendance)", [&](const httplib::Request& req, httplib::Response& res) {
        std::shared_ptr<User> user;
        if (!requireAuth(req, res, authService, "ClubAdmin", user)) return;
        try {
            int eventId = std::stoi(req.matches[1]);
            auto body = nlohmann::json::parse(req.body);
            std::vector<int> studentIds = body.value("studentIds", std::vector<int>{});
            int count = eventService.markAttendance(eventId, studentIds);
            dataManager.saveEnrollments();
            res.set_content(nlohmann::json({{"success", true}, {"markedCount", count}}).dump(), "application/json");
        } catch (const std::exception& e) { handleRouteException(res, e); }
    });

    // 14. GET /api/certificate/:enrollmentId
    svr.Get(R"(/api/certificate/(\d+))", [&](const httplib::Request& req, httplib::Response& res) {
        try {
            int enId = std::stoi(req.matches[1]);
            std::string certText = eventService.generateCertificate(enId);
            res.set_content(nlohmann::json({{"certificate", certText}}).dump(), "application/json");
        } catch (const std::exception& e) { handleRouteException(res, e); }
    });

    // 15. GET /api/reports (SuperAdmin)
    svr.Get("/api/reports", [&](const httplib::Request& req, httplib::Response& res) {
        std::shared_ptr<User> user;
        if (!requireAuth(req, res, authService, "SuperAdmin", user)) return;
        res.set_content(eventService.getReports().dump(), "application/json");
    });

    // Static Web Assets Mount
    svr.set_mount_point("/", "web");
}
