#ifndef ROUTES_H
#define ROUTES_H

#include "httplib.h"
#include "services/AuthService.h"
#include "services/EventService.h"
#include "services/EnrollmentService.h"
#include "services/PaymentService.h"
#include "core/DataManager.h"

// LAYERED ARCHITECTURE: Web API routing boundary.
// Translates HTTP requests to Service method invocations and converts domain exceptions into standard HTTP status codes.
// Contains zero business logic.
class Routes {
public:
    static void setup(httplib::Server& svr,
                      AuthService& authService,
                      EventService& eventService,
                      EnrollmentService& enrollmentService,
                      PaymentService& paymentService,
                      DataManager& dataManager);
};

#endif // ROUTES_H
