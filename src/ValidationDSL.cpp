#include "ValidationDSL.h"
#include "Logger.h"
#include <fstream>
#include <sstream>
#include <algorithm>

std::vector<ValidationRule> ValidationDSL::rules;

void ValidationDSL::loadRulesFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        Logger::log(LogLevel::WARNING, 
            "Could not load validation rules from: " + filename);
        return;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        // Skip empty lines and comments
        if (line.empty() || line[0] == '#') {
            continue;
        }
        
        // Parse RULE: fieldname operator value
        if (line.substr(0, 5) == "RULE:") {
            std::istringstream iss(line.substr(5));
            std::string fieldName, opStr;
            double value;
            
            if (iss >> fieldName >> opStr >> value) {
                ValidationOperator op = parseOperator(opStr);
                addRule(fieldName, op, value, line);
            }
        }
    }
    
    file.close();
    Logger::log(LogLevel::INFO, 
        "Loaded " + std::to_string(rules.size()) + " validation rules");
}

void ValidationDSL::addRule(const std::string& fieldName,
                           ValidationOperator op,
                           double value,
                           const std::string& description) {
    ValidationRule rule;
    rule.fieldName = fieldName;
    rule.op = op;
    rule.value = value;
    rule.description = description.empty() ? fieldName : description;
    rule.enabled = true;
    
    rules.push_back(rule);
    Logger::log(LogLevel::INFO, 
        "Added validation rule: " + fieldName + " " + 
        operatorToString(op) + " " + std::to_string(value));
}

bool ValidationDSL::validate(const std::string& ruleName, double value) {
    auto it = std::find_if(rules.begin(), rules.end(),
                          [&ruleName](const ValidationRule& r) {
                              return r.fieldName == ruleName;
                          });
    
    if (it == rules.end()) {
        Logger::log(LogLevel::WARNING, 
            "Validation rule not found: " + ruleName);
        return true;  // Default to pass if rule not found
    }
    
    if (!it->enabled) {
        return true;  // Skip disabled rules
    }
    
    bool result = false;
    switch (it->op) {
        case ValidationOperator::GREATER_THAN:
            result = value > it->value;
            break;
        case ValidationOperator::LESS_THAN:
            result = value < it->value;
            break;
        case ValidationOperator::EQUAL:
            result = value == it->value;
            break;
        case ValidationOperator::NOT_EQUAL:
            result = value != it->value;
            break;
        case ValidationOperator::GREATER_EQUAL:
            result = value >= it->value;
            break;
        case ValidationOperator::LESS_EQUAL:
            result = value <= it->value;
            break;
    }
    
    if (!result) {
        Logger::log(LogLevel::WARNING,
            "Validation failed for " + ruleName + 
            ": " + std::to_string(value) + " not " +
            operatorToString(it->op) + " " + std::to_string(it->value));
    }
    
    return result;
}

bool ValidationDSL::validateCategory(const std::string& category,
                                    const std::map<std::string, double>& values) {
    auto categoryRules = getRulesByCategory(category);
    
    if (categoryRules.empty()) {
        return true;  // No rules for this category
    }
    
    for (const auto& rule : categoryRules) {
        auto it = values.find(rule.fieldName);
        if (it != values.end()) {
            if (!validate(rule.fieldName, it->second)) {
                return false;
            }
        }
    }
    
    return true;
}

const std::vector<ValidationRule>& ValidationDSL::getRules() {
    return rules;
}

std::vector<ValidationRule> ValidationDSL::getRulesByCategory(const std::string& category) {
    std::vector<ValidationRule> categoryRules;
    
    for (const auto& rule : rules) {
        // Extract category from fieldName (e.g., "Order.amount" -> "Order")
        size_t dotPos = rule.fieldName.find('.');
        if (dotPos != std::string::npos) {
            std::string ruleCategory = rule.fieldName.substr(0, dotPos);
            if (ruleCategory == category) {
                categoryRules.push_back(rule);
            }
        }
    }
    
    return categoryRules;
}

ValidationOperator ValidationDSL::parseOperator(const std::string& op) {
    if (op == ">") return ValidationOperator::GREATER_THAN;
    if (op == "<") return ValidationOperator::LESS_THAN;
    if (op == "==") return ValidationOperator::EQUAL;
    if (op == "!=") return ValidationOperator::NOT_EQUAL;
    if (op == ">=") return ValidationOperator::GREATER_EQUAL;
    if (op == "<=") return ValidationOperator::LESS_EQUAL;
    
    Logger::log(LogLevel::WARNING, "Unknown operator: " + op);
    return ValidationOperator::GREATER_THAN;
}

std::string ValidationDSL::operatorToString(ValidationOperator op) {
    switch (op) {
        case ValidationOperator::GREATER_THAN:
            return ">";
        case ValidationOperator::LESS_THAN:
            return "<";
        case ValidationOperator::EQUAL:
            return "==";
        case ValidationOperator::NOT_EQUAL:
            return "!=";
        case ValidationOperator::GREATER_EQUAL:
            return ">=";
        case ValidationOperator::LESS_EQUAL:
            return "<=";
    }
    return "UNKNOWN";
}

void ValidationDSL::clearRules() {
    rules.clear();
    Logger::log(LogLevel::INFO, "Validation rules cleared");
}
