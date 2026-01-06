#pragma once
#include <string>
#include <ctime>
#include "OrderFSM.h"

struct Customer {
    int id;
    std::string name;
    std::string phone;
    std::string email;
    int loyaltyPoints;
};

struct MenuItem {
    int id;
    std::string name;
    std::string category;
    double price;
};

struct Order {
    int orderId;
    int customerId;
    double total;
    int priority;
    std::time_t timestamp;
    OrderState state;

    bool updateState(OrderState next) {
        if (OrderFSM::canTransition(state, next)) {
            state = next;
            return true;
        }
        return false;
    }
};
