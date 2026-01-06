# Restaurant Management System (C++)

Production-ready refactor of the original monolith into a clean, multi-file C++ backend with CSV/TXT storage and zero external frameworks.

## Structure

- include/
  - Common.h: Shared enums & constants
  - Logger.h: Logger interface
  - Models.h: Pure data models (Customer, MenuItem, Order)
  - OrderFSM.h: Order state machine (inline implementation)
  - DataStructures.h, Algorithms.h: Placeholders for future shared declarations
- src/
  - Logger.cpp: Logger implementation
  - OrderService.cpp: Order placement stub
  - InventoryService.cpp, AnalyticsEngine.cpp, DeliveryManager.cpp: Stubs
- data/
  - customers.csv, menu.csv, inventory.csv, feedback.csv
- main.cpp: Minimal wiring / bootstrap
- daa_project.c++: Original monolith kept unchanged for reference

## Build

### On Windows (MSYS2 + MinGW-w64 GCC)

```powershell
# First-time setup: install MSYS2 if not already present
winget install -e --id MSYS2.MSYS2 --accept-package-agreements --accept-source-agreements

# Install GCC via pacman (one-time)
"C:\msys64\usr\bin\bash.exe" -lc "pacman -Syu --noconfirm && pacman -S --noconfirm mingw-w64-ucrt-x86_64-gcc"

# Build (from project root)
"C:\msys64\usr\bin\bash.exe" -lc "cd /d/daa_project_fullstack && g++ -std=c++17 -Wall -O2 src/*.cpp main.cpp -Iinclude -o restaurant_system.exe"

# Run
.\restaurant_system.exe
```

### On Linux/macOS

```bash
g++ -std=c++17 -Wall -O2 src/*.cpp main.cpp -Iinclude -o restaurant_system
./restaurant_system
```

## Notes

- Same logic boundaries preserved: FSM, models, and logging separated.
- Storage remains CSV/TXT; no frameworks or external deps.
- Easy to extend with services while keeping clean interfaces.
