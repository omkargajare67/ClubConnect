#ifndef USER_FACTORY_H
#define USER_FACTORY_H

#include <memory>
#include "json.hpp"
#include "users/User.h"

// FACTORY PATTERN: Instantiates polymorphic User derivatives from persisted user JSON data.
class UserFactory {
public:
    static std::shared_ptr<User> createUser(const nlohmann::json& j);
};

#endif // USER_FACTORY_H
