# 📈 Stock Market Simulator (Wall Street Engine)

## 🧾 Overview
This C++ program simulates a **multi-stock trading platform** that processes buy/sell orders and executes trades in real-time. It maintains per-stock order books using priority queues, supports detailed trade analytics, and introduces a fun feature: a **time traveler** who determines the optimal buy-sell points for maximum profit.

This project mimics how modern trading engines operate with precision and modularity.

## 📂 File Structure
```
market.cpp       → Full source code
README.md        → Documentation and usage
P2random.h       → (Optional) Custom random order generator
```

## ⚙️ Features

- 💸 **Order Matching Engine**:
  - Uses max-heaps for buyers, min-heaps for sellers
  - Matches orders based on price and sequence

- 🧠 **CLI Argument Options**:
  - `--verbose`: Output each trade
  - `--median`: Print median trade price per timestamp
  - `--trader_info`: Summary of each trader's performance
  - `--time_travelers`: Find optimal buy/sell times per stock

- 📊 **Analytics**:
  - Per-trader net profit/loss
  - Median price per timestamp
  - Time traveler optimization

- 📥 **Input Modes**:
  - `PR` mode with `P2random.h` to simulate orders
  - Manual order entry via `stdin`

## 🧠 Key Concepts Used

- **Heap-based priority queues** (`std::priority_queue`)
- **Enums & Structs** to model time-state, orders, traders, and stocks
- **CLI parsing** with `getopt_long()`
- **Double heap method** for real-time median tracking
- **Greedy time-travel strategy** for maximum stock arbitrage

## 🛠 Tech Stack

- Language: **C++11**
- STL Containers: `vector`, `queue`, `priority_queue`
- Libraries: `getopt.h`, `P2random.h` (if PR mode enabled)

## ▶️ How to Build & Run

### ✅ Compile
```bash
g++ -std=c++11 -o market market.cpp
```

### ▶️ Run
```bash
./market --verbose --median --trader_info --time_travelers < input.txt
```

- Supports **any combination** of flags
- Input format varies by mode (`PR` or manual)

## 🧪 Sample Input Format (Manual)
```
# input header
input_mode MANUAL
number_of_traders 3
number_of_stocks 2

# orders (timestamp, action, trader_id, stock_id, price, quantity)
0 BUY T0 S0 $100 Q10
0 SELL T1 S0 $90 Q10
1 SELL T2 S1 $80 Q5
...
```

Or if using `PR` mode:
```
input_mode PR
seed 1337
num_orders 1000
arrival_rate 2
```

## 📌 Sample Output (Verbose Mode)
```
Processing orders...
Trader 0 purchased 10 shares of Stock 0 from Trader 1 for $90/share
Median match price of Stock 0 at time 0 is $90
---Trader Info---
Trader 0 bought 10 and sold 0 for a net transfer of $-900
Trader 1 bought 0 and sold 10 for a net transfer of $900
...
---Time Travelers---
A time traveler would buy Stock 0 at time 0 for $90 and sell it at time 3 for $110
```

## 📈 Future Improvements
- Add GUI for live market visualization
- Load real historical data from CSV/JSON
- Simulate multiple trading strategies (momentum, arbitrage, etc.)
- Add transaction fee modeling and latency simulation

## 📄 License
MIT License — use or modify freely.
