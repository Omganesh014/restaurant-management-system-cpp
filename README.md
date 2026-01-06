# Restaurant Management System (C++)

## 👤 Author

Omganesh Matiwade

Computer Science & Engineering

Focus: C++, Data Structures, System Design

A comprehensive Restaurant Management System implemented in C++ with a strong focus on
data structures, algorithms, and system design principles.

> This project was initially developed as a single-file academic submission and later
refactored into a modular multi-file architecture for maintainability and scalability.

## Features
- Customer, Order, Inventory, Billing, and Reservation management
- Priority-based order processing using Heaps
- Inventory management using Hash Tables
- Customer lookup using AVL Trees
- Delivery optimization using Graph algorithms (BFS, DFS, Dijkstra, Prim)
- FSM-based order lifecycle management
- File persistence and analytics

# Restaurant Management System (Full Stack | C++)

## 📌 Overview
The **Restaurant Management System** is a **full-stack software application** designed to manage real-world restaurant operations using a **high-performance C++ backend** and a **frontend interface** for interaction and visualization.

The system models customers, orders, kitchen workflows, inventory, billing, reservations, delivery routing, promotions, feedback, and analytics. It emphasizes **data structures, algorithms, and system-level design** rather than simple CRUD operations.

This project demonstrates a clear transition from **academic programming** to **production-oriented engineering**, focusing on correctness, scalability, and maintainability.

---

## ⚙️ Backend Architecture (C++)
ProjectRoot


│

├── include/

│   ├── Common.h           // Shared enums, constants

│   ├── Logger.h           // Logging system

│   ├── Models.h           // Domain entities

│   ├── DataStructures.h   // AVL, Heap, Hash, LRU Cache

│   └── Algorithms.h       // Sorting, Searching, Graph algorithms

│

├── src/

│   ├── Logger.cpp

│   ├── OrderService.cpp

│   ├── InventoryService.cpp

│   ├── AnalyticsEngine.cpp

│   └── DeliveryManager.cpp

│

└── main.cpp               // Application entry point

---

## 🧠 Data Structures Used

AVL Tree – Customer lookup (O(log n))

Max Heap – Priority-based order processing

Hash Table – Inventory management

LRU Cache – Frequently accessed data

Linked List – Kitchen order queue

Circular Queue – Billing workflow

Graph (Adjacency List / Matrix) – Delivery routing

## 📐 Algorithms Implemented

Sorting: Merge Sort, Quick Sort, Heap Sort

Searching: Binary Search, KMP, Rabin-Karp

Graph Algorithms: BFS, DFS, Dijkstra, Prim’s MST

Greedy Algorithms: Coin Change, Scheduling heuristics

Optimized Implementations: Priority-queue-based Dijkstra & Prim

## 📊 Analytics & Reporting

Daily revenue and sales analysis

Customer lifetime value calculation

Feedback sentiment analysis

Inventory optimization and alerts

Operational efficiency dashboard

## 💾 Persistence & Reliability

File-based storage using CSV / TXT

Backup and restore functionality

Audit trail and transaction history

Structured logging and error handling

Memory-safe cleanup using RAII principles

## 🛠️ Tech Stack
Backend

Language: C++

Standard: C++17

Paradigms: OOP, FSM, Modular Design

Focus: Performance, correctness, memory safety

Frontend

Current: CLI

Extensible: Web / GUI / API interfaces

## 🚀 Build & Run

g++ -std=c++17 -Wall -O2 src/*.cpp main.cpp -o restaurant_system

./restaurant_system

## 🎯 Learning Outcomes

Designed a full-stack system with a native C++ backend

Applied data structures to real-world business problems

Implemented state-safe workflows using FSM

Practiced clean architecture and modular refactoring

Gained experience in system-level software design

## 📌 Note

The project was initially implemented as a single-file academic submission and later refactored into a modular full-stack architecture to reflect real-world production practices.


## 🏗️ System Architecture

┌────────────────────┐
│    Frontend UI     │
│ (CLI / GUI / Web)  │
└─────────▲──────────┘
          │
          │ User Actions / Requests
          ▼
┌────────────────────┐
│  Backend (C++)     │
│ Business Logic     │
│ FSM + Services     │
└─────────▲──────────┘
          │
          │ Data Access
          ▼
┌────────────────────┐
│ Persistence Layer  │
│ CSV / TXT Files   │
└────────────────────┘

---

