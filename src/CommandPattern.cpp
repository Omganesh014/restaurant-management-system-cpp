#include "CommandPattern.h"
#include "Logger.h"
#include "BusinessRules.h"
#include "EventSystem.h"

// Static members
CommandInvoker* CommandInvoker::instance = nullptr;
std::vector<std::shared_ptr<Command>> CommandInvoker::commandHistory;

CommandInvoker& CommandInvoker::getInstance() {
    if (!instance) instance = new CommandInvoker();
    return *instance;
}

bool CommandInvoker::execute(std::shared_ptr<Command> cmd) {
    if (!cmd) return false;
    if (!cmd->execute()) return false;
    commandHistory.push_back(cmd);
    Logger::log(LogLevel::INFO, "CMD: " + cmd->getName());
    return true;
}

bool CommandInvoker::undo() {
    if (commandHistory.empty()) return false;
    auto cmd = commandHistory.back();
    if (!cmd->undo()) return false;
    commandHistory.pop_back();
    Logger::log(LogLevel::INFO, "UNDO: " + cmd->getName());
    return true;
}

const std::vector<std::shared_ptr<Command>>& CommandInvoker::getHistory() const {
    return commandHistory;
}

std::shared_ptr<Command> CommandInvoker::getLastCommand() const {
    if (commandHistory.empty()) return nullptr;
    return commandHistory.back();
}

int CommandInvoker::getCommandCount() const {
    return commandHistory.size();
}

void CommandInvoker::clearHistory() {
    commandHistory.clear();
    Logger::log(LogLevel::INFO, "Command history cleared");
}

void CommandInvoker::replayCommands() {
    Logger::log(LogLevel::INFO, "Replaying " + std::to_string(commandHistory.size()) + " commands");
    for (const auto& cmd : commandHistory) {
        Logger::log(LogLevel::INFO, "  >> " + cmd->getDescription());
    }
}

// PlaceOrderCommand
bool PlaceOrderCommand::execute() {
    Logger::log(LogLevel::INFO, "PlaceOrderCommand: Processing");
    Event evt;
    evt.type = EventType::ORDER_PLACED;
    evt.timestamp = std::time(nullptr);
    EventBus::getInstance().emit(evt);
    return true;
}

bool PlaceOrderCommand::undo() {
    Logger::log(LogLevel::INFO, "PlaceOrderCommand: Undone");
    return true;
}

std::string PlaceOrderCommand::getName() const {
    return "PlaceOrder";
}

std::string PlaceOrderCommand::getDescription() const {
    return "Place order";
}

// CancelOrderCommand
bool CancelOrderCommand::execute() {
    Logger::log(LogLevel::INFO, "CancelOrderCommand #" + std::to_string(orderId));
    Event evt;
    evt.type = EventType::ORDER_CANCELLED;
    evt.timestamp = std::time(nullptr);
    EventBus::getInstance().emit(evt);
    return true;
}

bool CancelOrderCommand::undo() {
    Logger::log(LogLevel::INFO, "CancelOrderCommand: Undone");
    return true;
}

std::string CancelOrderCommand::getName() const {
    return "CancelOrder";
}

std::string CancelOrderCommand::getDescription() const {
    return "Cancel order #" + std::to_string(orderId);
}

// IssueRefundCommand
bool IssueRefundCommand::execute() {
    Logger::log(LogLevel::INFO, "IssueRefundCommand: $" + std::to_string(amount));
    Event evt;
    evt.type = EventType::REFUND_ISSUED;
    evt.timestamp = std::time(nullptr);
    EventBus::getInstance().emit(evt);
    return true;
}

bool IssueRefundCommand::undo() {
    Logger::log(LogLevel::INFO, "IssueRefundCommand: Undone");
    return true;
}

std::string IssueRefundCommand::getName() const {
    return "IssueRefund";
}

std::string IssueRefundCommand::getDescription() const {
    return "Refund $" + std::to_string((int)amount);
}

// ConfirmOrderCommand
bool ConfirmOrderCommand::execute() {
    Logger::log(LogLevel::INFO, "ConfirmOrderCommand #" + std::to_string(orderId));
    Event evt;
    evt.type = EventType::ORDER_CONFIRMED;
    evt.timestamp = std::time(nullptr);
    EventBus::getInstance().emit(evt);
    return true;
}

bool ConfirmOrderCommand::undo() {
    Logger::log(LogLevel::INFO, "ConfirmOrderCommand: Undone");
    return true;
}

std::string ConfirmOrderCommand::getName() const {
    return "ConfirmOrder";
}

std::string ConfirmOrderCommand::getDescription() const {
    return "Confirm order #" + std::to_string(orderId);
}
