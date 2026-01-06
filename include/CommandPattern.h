#pragma once
#include "Models.h"
#include <string>
#include <memory>
#include <vector>

class Command {
public:
    virtual ~Command() = default;
    virtual bool execute() = 0;
    virtual bool undo() { return false; }
    virtual std::string getName() const = 0;
    virtual std::string getDescription() const = 0;
};

class PlaceOrderCommand : public Command {
private:
    Order order;
public:
    explicit PlaceOrderCommand(const Order& o) : order(o) {}
    bool execute() override;
    bool undo() override;
    std::string getName() const override;
    std::string getDescription() const override;
};

class CancelOrderCommand : public Command {
private:
    int orderId;
    std::string reason;
public:
    CancelOrderCommand(int id, const std::string& r = "") : orderId(id), reason(r) {}
    bool execute() override;
    bool undo() override;
    std::string getName() const override;
    std::string getDescription() const override;
};

class IssueRefundCommand : public Command {
private:
    int orderId;
    double amount;
    std::string reason;
public:
    IssueRefundCommand(int id, double amt, const std::string& r = "") 
        : orderId(id), amount(amt), reason(r) {}
    bool execute() override;
    bool undo() override;
    std::string getName() const override;
    std::string getDescription() const override;
};

class ConfirmOrderCommand : public Command {
private:
    int orderId;
public:
    explicit ConfirmOrderCommand(int id) : orderId(id) {}
    bool execute() override;
    bool undo() override;
    std::string getName() const override;
    std::string getDescription() const override;
};

class CommandInvoker {
private:
    static CommandInvoker* instance;
    static std::vector<std::shared_ptr<Command>> commandHistory;
    CommandInvoker() = default;
public:
    static CommandInvoker& getInstance();
    bool execute(std::shared_ptr<Command> cmd);
    bool undo();
    const std::vector<std::shared_ptr<Command>>& getHistory() const;
    std::shared_ptr<Command> getLastCommand() const;
    int getCommandCount() const;
    void clearHistory();
    void replayCommands();
};
