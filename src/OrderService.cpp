#include "Models.h"
#include "Logger.h"
#include <vector>

static std::vector<Order> orders;

void placeOrder(const Order& o) {
    orders.push_back(o);
    Logger::log(LogLevel::INFO, "Order placed");
}
