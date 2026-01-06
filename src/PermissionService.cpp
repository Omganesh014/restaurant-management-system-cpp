#include "PermissionService.h"
#include "Logger.h"
#include <algorithm>
#include <array>
#include <stdexcept>

bool PermissionService::canPerform(Action action) {
    // Central allowlist keeps behavior explicit and easy to tighten later.
    constexpr std::array<Action, 11> allowed = {
        Action::CREATE_ORDER,
        Action::MODIFY_ORDER,
        Action::CANCEL_ORDER,
        Action::ISSUE_REFUND,
        Action::VIEW_CUSTOMER_DATA,
        Action::MANAGE_MENU,
        Action::MANAGE_INVENTORY,
        Action::PROCESS_PAYMENT,
        Action::GENERATE_REPORT,
        Action::BACKUP_SYSTEM,
        Action::VIEW_AUDIT_LOG
    };

    const bool permitted = std::find(allowed.begin(), allowed.end(), action) != allowed.end();
    Logger::log(LogLevel::DEBUG, "Permission check: " + actionToString(action) +
                                   (permitted ? " -> ALLOWED" : " -> DENIED"));
    return permitted;
}

std::string PermissionService::actionToString(Action action) {
    switch (action) {
        case Action::CREATE_ORDER:         return "CREATE_ORDER";
        case Action::MODIFY_ORDER:         return "MODIFY_ORDER";
        case Action::CANCEL_ORDER:         return "CANCEL_ORDER";
        case Action::ISSUE_REFUND:         return "ISSUE_REFUND";
        case Action::VIEW_CUSTOMER_DATA:   return "VIEW_CUSTOMER_DATA";
        case Action::MANAGE_MENU:          return "MANAGE_MENU";
        case Action::MANAGE_INVENTORY:     return "MANAGE_INVENTORY";
        case Action::PROCESS_PAYMENT:      return "PROCESS_PAYMENT";
        case Action::GENERATE_REPORT:      return "GENERATE_REPORT";
        case Action::BACKUP_SYSTEM:        return "BACKUP_SYSTEM";
        case Action::VIEW_AUDIT_LOG:       return "VIEW_AUDIT_LOG";
        default:                           return "UNKNOWN_ACTION";
    }
}

void PermissionService::enforce(Action action) {
    if (!canPerform(action)) {
        std::string msg = "Permission denied for action: " + actionToString(action);
        Logger::log(LogLevel::WARNING, msg);
        throw std::runtime_error(msg);
    }
}
