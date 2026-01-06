#pragma once
#include <string>

enum class OrderState {
    CREATED,
    CONFIRMED,
    PREPARING,
    READY,
    SERVED,
    CANCELLED,
    REFUNDED
};

class OrderFSM {
public:
    static bool canTransition(OrderState from, OrderState to);
    static std::string toString(OrderState s);
};

// Implementation (still in header OR separate cpp if you want)
inline bool OrderFSM::canTransition(OrderState from, OrderState to) {
    switch (from) {
        case OrderState::CREATED:
            return to == OrderState::CONFIRMED || to == OrderState::CANCELLED;
        case OrderState::CONFIRMED:
            return to == OrderState::PREPARING || to == OrderState::CANCELLED;
        case OrderState::PREPARING:
            return to == OrderState::READY;
        case OrderState::READY:
            return to == OrderState::SERVED || to == OrderState::CANCELLED;
        case OrderState::SERVED:
            return to == OrderState::REFUNDED;
        default:
            return false;
    }
}

inline std::string OrderFSM::toString(OrderState s) {
    static const char* names[] = {
        "CREATED","CONFIRMED","PREPARING","READY",
        "SERVED","CANCELLED","REFUNDED"
    };
    return names[(int)s];
}
