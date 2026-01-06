#pragma once
#include <string>

/**
 * Role-Agnostic Permission System
 * Even without full RBAC, enforce permission checks
 * Allows easy plugging in of role-based rules later
 */
enum class Action {
    CREATE_ORDER,
    MODIFY_ORDER,
    CANCEL_ORDER,
    ISSUE_REFUND,
    VIEW_CUSTOMER_DATA,
    MANAGE_MENU,
    MANAGE_INVENTORY,
    PROCESS_PAYMENT,
    GENERATE_REPORT,
    BACKUP_SYSTEM,
    VIEW_AUDIT_LOG
};

class PermissionService {
public:
    /**
     * Check if an action can be performed
     * Currently returns true for all (future: plug in role checks)
     */
    static bool canPerform(Action action);
    
    /**
     * Get human-readable action name
     */
    static std::string actionToString(Action action);
    
    /**
     * Enforce permission check (throws if denied)
     */
    static void enforce(Action action);
};
