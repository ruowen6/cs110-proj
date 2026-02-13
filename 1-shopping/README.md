# 1-shopping — Chain Stores (Shopping)

A C++ console application implementing the COMP.CS.110 "Chain stores (Shopping)" assignment. The program reads product and store data from a text input file, validates and stores the data in memory, and provides an interactive command-line interface to inspect chains, stores, products, selections and to query cheapest prices.

**Contents of this folder**
- `main.cpp` — entry point for the console program.
- `shopping.pro` — Qt project file (optional; can be opened with Qt Creator / qmake).

**High-level features**
- Loads the dataset once at startup and validates the input file format.
- Stores data using standard C++ containers (`std::map`, custom `struct Product`, etc.).
- Interactive CLI supporting at least the following commands: `chains`, `stores`, `selection`, `cheapest`, `products`, `quit`.

## 1) Background / Purpose

This project is an exercise in file I/O, data parsing, and container-based data modelling in C++. It demonstrates how to read structured data, validate its format, represent hierarchical relationships (chains → stores → products) and implement an interactive query interface. The implementation follows the COMP.CS.110 assignment requirements for the "Chain stores (Shopping)" task.

## 2) How to Use

### Prerequisites
- A C++ compiler with C++17 support (e.g., `g++`, `clang++`).
- Optional: Qt and `qmake` / Qt Creator if you prefer to open the `.pro` file.

### Build (simple, using g++)
Open a terminal and run:

```bash
cd 1-shopping
g++ -std=c++17 -O2 -Wall -Wextra -pedantic main.cpp -o shopping
```

This produces a `shopping` executable.

### Build with Qt (`.pro`)
If you have Qt installed you can open `shopping.pro` in Qt Creator.

## 3) References & Credits
- Based on the COMP.CS.110 Programming 2 assignment "Chain stores (Shopping)".
- Course materials and instructor-provided examples are the authoritative reference for input formatting.