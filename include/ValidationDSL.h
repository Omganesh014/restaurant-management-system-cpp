#pragma once
#include <string>
#include <vector>
#include <map>

/**
 * Validation DSL (Domain Specific Language)
 * Parse and evaluate rules at runtime
 * No hardcoding of validations
 * Example: RULE: Order.amount > 0
 */

enum class ValidationOperator {
    GREATER_THAN,
    LESS_THAN,
    EQUAL,
    NOT_EQUAL,
    GREATER_EQUAL,
    LESS_EQUAL
};

struct ValidationRule {
    std::string fieldName;      // e.g., "Order.amount"
    ValidationOperator op;
    double value;
    std::string description;
    bool enabled = true;
};

/**
 * Validation DSL Parser and Evaluator
 */
class ValidationDSL {
private:
    static std::vector<ValidationRule> rules;
    
public:
    /**
     * Load validation rules from config file
     * Format: RULE: fieldname operator value
     * Example: RULE: Order.amount > 0
     */
    static void loadRulesFromFile(const std::string& filename);
    
    /**
     * Add a rule programmatically
     */
    static void addRule(const std::string& fieldName, 
                       ValidationOperator op, 
                       double value,
                       const std::string& description = "");
    
    /**
     * Validate a value against a rule
     */
    static bool validate(const std::string& ruleName, double value);
    
    /**
     * Validate all rules for a category
     */
    static bool validateCategory(const std::string& category, 
                                const std::map<std::string, double>& values);
    
    /**
     * Get all rules
     */
    static const std::vector<ValidationRule>& getRules();
    
    /**
     * Get rules by category (e.g., "Order", "Inventory")
     */
    static std::vector<ValidationRule> getRulesByCategory(const std::string& category);
    
    /**
     * Parse operator string to enum
     */
    static ValidationOperator parseOperator(const std::string& op);
    
    /**
     * Get operator as string
     */
    static std::string operatorToString(ValidationOperator op);
    
    /**
     * Clear all rules
     */
    static void clearRules();
};
