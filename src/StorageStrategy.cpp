#include "StorageStrategy.h"
#include "Logger.h"
#include <fstream>
#include <sstream>
#include <filesystem>

namespace fs = std::filesystem;

// ============ CSVStorageStrategy Implementation ============

bool CSVStorageStrategy::saveCustomer(const CustomerRecord& customer) {
    Logger::log("STORAGE: Saving customer " + customer.id + " (CSV)");
    
    try {
        std::ofstream file("data/customers.txt", std::ios::app);
        if (!file.is_open()) return false;
        
        file << customer.id << "," << customer.name << "," 
             << customer.email << "," << (customer.isActive ? "1" : "0") << "\n";
        file.close();
        return true;
    } catch (...) {
        return false;
    }
}

CustomerRecord CSVStorageStrategy::loadCustomer(const std::string& id) {
    Logger::log("STORAGE: Loading customer " + id + " (CSV)");
    
    CustomerRecord customer;
    try {
        std::ifstream file("data/customers.txt");
        if (!file.is_open()) return customer;
        
        std::string line;
        while (std::getline(file, line)) {
            std::stringstream ss(line);
            std::string cid, name, email, active;
            
            if (std::getline(ss, cid, ',') && cid == id) {
                std::getline(ss, name, ',');
                std::getline(ss, email, ',');
                std::getline(ss, active, ',');
                
                customer.id = cid;
                customer.name = name;
                customer.email = email;
                customer.isActive = (active == "1");
                return customer;
            }
        }
        file.close();
    } catch (...) {
        Logger::log("Error loading customer from CSV");
    }
    
    return customer;
}

std::vector<CustomerRecord> CSVStorageStrategy::loadAllCustomers() {
    Logger::log("STORAGE: Loading all customers (CSV)");
    
    std::vector<CustomerRecord> customers;
    try {
        std::ifstream file("data/customers.txt");
        if (!file.is_open()) return customers;
        
        std::string line;
        while (std::getline(file, line)) {
            std::stringstream ss(line);
            std::string cid, name, email, active;
            
            if (std::getline(ss, cid, ',')) {
                std::getline(ss, name, ',');
                std::getline(ss, email, ',');
                std::getline(ss, active, ',');
                
                CustomerRecord customer;
                customer.id = cid;
                customer.name = name;
                customer.email = email;
                customer.isActive = (active == "1");
                customers.push_back(customer);
            }
        }
        file.close();
    } catch (...) {
        Logger::log("Error loading customers from CSV");
    }
    
    return customers;
}

bool CSVStorageStrategy::deleteCustomer(const std::string& id) {
    Logger::log("STORAGE: Deleting customer " + id + " (CSV)");
    
    // For CSV, soft delete would mark as inactive
    // Hard delete would remove the line (not typical for this pattern)
    return true;
}

bool CSVStorageStrategy::saveMenuItem(const MenuItem& item) {
    Logger::log("STORAGE: Saving menu item " + item.id + " (CSV)");
    
    try {
        std::ofstream file("data/menu_items.txt", std::ios::app);
        if (!file.is_open()) return false;
        
        file << item.id << "," << item.name << "," << item.price << ","
             << item.quantityAvailable << "\n";
        file.close();
        return true;
    } catch (...) {
        return false;
    }
}

MenuItem CSVStorageStrategy::loadMenuItem(const std::string& id) {
    Logger::log("STORAGE: Loading menu item " + id + " (CSV)");
    
    MenuItem item;
    try {
        std::ifstream file("data/menu_items.txt");
        if (!file.is_open()) return item;
        
        std::string line;
        while (std::getline(file, line)) {
            std::stringstream ss(line);
            std::string iid, name, price, qty;
            
            if (std::getline(ss, iid, ',') && iid == id) {
                std::getline(ss, name, ',');
                std::getline(ss, price, ',');
                std::getline(ss, qty, ',');
                
                item.id = iid;
                item.name = name;
                item.price = std::stod(price);
                item.quantityAvailable = std::stoi(qty);
                return item;
            }
        }
        file.close();
    } catch (...) {
        Logger::log("Error loading menu item from CSV");
    }
    
    return item;
}

std::vector<MenuItem> CSVStorageStrategy::loadAllMenuItems() {
    Logger::log("STORAGE: Loading all menu items (CSV)");
    
    std::vector<MenuItem> items;
    try {
        std::ifstream file("data/menu_items.txt");
        if (!file.is_open()) return items;
        
        std::string line;
        while (std::getline(file, line)) {
            std::stringstream ss(line);
            std::string iid, name, price, qty;
            
            if (std::getline(ss, iid, ',')) {
                std::getline(ss, name, ',');
                std::getline(ss, price, ',');
                std::getline(ss, qty, ',');
                
                MenuItem item;
                item.id = iid;
                item.name = name;
                item.price = std::stod(price);
                item.quantityAvailable = std::stoi(qty);
                items.push_back(item);
            }
        }
        file.close();
    } catch (...) {
        Logger::log("Error loading menu items from CSV");
    }
    
    return items;
}

bool CSVStorageStrategy::deleteMenuItem(const std::string& id) {
    Logger::log("STORAGE: Deleting menu item " + id + " (CSV)");
    return true;
}

bool CSVStorageStrategy::saveOrder(const Order& order) {
    Logger::log("STORAGE: Saving order " + order.id + " (CSV)");
    
    try {
        std::ofstream file("data/orders.txt", std::ios::app);
        if (!file.is_open()) return false;
        
        file << order.id << "," << order.customerId << "," 
             << order.total << "," << static_cast<int>(order.status) << "\n";
        file.close();
        return true;
    } catch (...) {
        return false;
    }
}

Order CSVStorageStrategy::loadOrder(const std::string& id) {
    Logger::log("STORAGE: Loading order " + id + " (CSV)");
    
    Order order;
    try {
        std::ifstream file("data/orders.txt");
        if (!file.is_open()) return order;
        
        std::string line;
        while (std::getline(file, line)) {
            std::stringstream ss(line);
            std::string oid, cid, total, status;
            
            if (std::getline(ss, oid, ',') && oid == id) {
                std::getline(ss, cid, ',');
                std::getline(ss, total, ',');
                std::getline(ss, status, ',');
                
                order.id = oid;
                order.customerId = cid;
                order.total = std::stod(total);
                order.status = static_cast<OrderStatus>(std::stoi(status));
                return order;
            }
        }
        file.close();
    } catch (...) {
        Logger::log("Error loading order from CSV");
    }
    
    return order;
}

std::vector<Order> CSVStorageStrategy::loadAllOrders() {
    Logger::log("STORAGE: Loading all orders (CSV)");
    
    std::vector<Order> orders;
    try {
        std::ifstream file("data/orders.txt");
        if (!file.is_open()) return orders;
        
        std::string line;
        while (std::getline(file, line)) {
            if (line.empty()) continue;
            
            std::stringstream ss(line);
            std::string oid, cid, total, status;
            
            if (std::getline(ss, oid, ',')) {
                std::getline(ss, cid, ',');
                std::getline(ss, total, ',');
                std::getline(ss, status, ',');
                
                Order order;
                order.id = oid;
                order.customerId = cid;
                order.total = std::stod(total);
                order.status = static_cast<OrderStatus>(std::stoi(status));
                orders.push_back(order);
            }
        }
        file.close();
    } catch (...) {
        Logger::log("Error loading orders from CSV");
    }
    
    return orders;
}

bool CSVStorageStrategy::deleteOrder(const std::string& id) {
    Logger::log("STORAGE: Deleting order " + id + " (CSV)");
    return true;
}

bool CSVStorageStrategy::isHealthy() {
    try {
        // Test write access
        std::ofstream test("data/.storage_health_check.txt");
        if (!test.is_open()) return false;
        test << "ok";
        test.close();
        
        // Test read access
        std::ifstream verify("data/.storage_health_check.txt");
        if (!verify.is_open()) return false;
        verify.close();
        
        fs::remove("data/.storage_health_check.txt");
        return true;
    } catch (...) {
        return false;
    }
}

// ============ StorageManager Implementation ============

StorageManager::StorageManager() 
    : strategy(std::make_unique<CSVStorageStrategy>()) {}

StorageManager& StorageManager::instance() {
    static StorageManager sm;
    return sm;
}

void StorageManager::setStrategy(std::unique_ptr<StorageStrategy> newStrategy) {
    if (newStrategy) {
        strategy = std::move(newStrategy);
        Logger::log("Storage strategy changed to: " + strategy->getName());
    }
}

StorageStrategy& StorageManager::getStrategy() {
    return *strategy;
}

std::string StorageManager::getStorageType() const {
    return strategy->getName();
}
