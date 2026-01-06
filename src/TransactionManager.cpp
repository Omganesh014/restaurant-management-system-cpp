#include "TransactionManager.h"
#include "Logger.h"

// ============ Transaction Implementation ============

Transaction::Transaction() 
    : state(State::READY), 
      startTime(std::chrono::system_clock::now()) {}

void Transaction::begin() {
    state = State::ACTIVE;
    Logger::log("Transaction started");
}

void Transaction::execute(std::function<void()> operation, std::function<void()> rollbackOp) {
    if (state != State::ACTIVE) {
        throw std::runtime_error("Cannot execute operation: transaction not active");
    }
    
    try {
        operation();
        operations.push_back({operation, rollbackOp, true});
        Logger::log("Transaction operation completed");
    } catch (const std::exception& e) {
        state = State::FAILED;
        errorMessage = std::string("Operation failed: ") + e.what();
        Logger::log("Transaction operation failed: " + errorMessage);
        throw;
    }
}

void Transaction::commit() {
    if (state == State::ACTIVE) {
        state = State::COMMITTED;
        Logger::log("Transaction committed with " + std::to_string(operations.size()) + " operations");
    } else if (state == State::FAILED) {
        applyRollbacks();
        Logger::log("Transaction failed, rollback applied");
    }
}

void Transaction::rollback() {
    if (state == State::ACTIVE || state == State::FAILED) {
        applyRollbacks();
        state = State::ROLLED_BACK;
        Logger::log("Transaction rolled back");
    }
}

void Transaction::applyRollbacks() {
    Logger::log("Applying " + std::to_string(operations.size()) + " rollback operations");
    
    // Rollback in reverse order (LIFO)
    for (int i = operations.size() - 1; i >= 0; --i) {
        if (operations[i].rollback) {
            try {
                operations[i].rollback();
            } catch (const std::exception& e) {
                Logger::log("Warning: Rollback operation " + std::to_string(i) + " failed: " + e.what());
            }
        }
    }
}

Transaction::State Transaction::getState() const {
    return state;
}

std::string Transaction::getStateString() const {
    switch (state) {
        case State::READY: return "READY";
        case State::ACTIVE: return "ACTIVE";
        case State::COMMITTED: return "COMMITTED";
        case State::ROLLED_BACK: return "ROLLED_BACK";
        case State::FAILED: return "FAILED";
        default: return "UNKNOWN";
    }
}

bool Transaction::isSuccessful() const {
    return state == State::COMMITTED;
}

const std::string& Transaction::getErrorMessage() const {
    return errorMessage;
}

std::chrono::system_clock::time_point Transaction::getStartTime() const {
    return startTime;
}

size_t Transaction::getOperationCount() const {
    return operations.size();
}

// ============ TransactionManager Implementation ============

TransactionManager& TransactionManager::instance() {
    static TransactionManager tm;
    return tm;
}

Transaction* TransactionManager::createTransaction() {
    auto tx = new Transaction();
    activeTransactions.push_back(tx);
    return tx;
}

void TransactionManager::removeTransaction(Transaction* tx) {
    if (tx->isSuccessful()) {
        totalCommitted++;
    } else {
        totalFailed++;
    }
    
    activeTransactions.erase(
        std::remove(activeTransactions.begin(), activeTransactions.end(), tx),
        activeTransactions.end()
    );
    delete tx;
}

size_t TransactionManager::getActiveTransactionCount() const {
    return activeTransactions.size();
}

size_t TransactionManager::getTotalCommittedTransactions() const {
    return totalCommitted;
}

size_t TransactionManager::getTotalFailedTransactions() const {
    return totalFailed;
}
