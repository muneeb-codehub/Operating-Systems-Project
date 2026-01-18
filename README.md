# Operating Systems Banking Simulation

**Author:** Muneeb  
**Project:** Banking System with OS Concepts Implementation

## Overview

This project simulates a banking system while demonstrating core Operating System concepts including Process Management, CPU Scheduling, Memory Management, Disk I/O Scheduling, File Management, and Inter-Process Communication.

## Features Implemented

### 1. Account Management
- Create new bank accounts with initial balance
- Deposit and withdraw operations with file persistence
- Balance inquiry system

### 2. Process & Thread Management
- **Process Control Block (PCB)** with PID, status, transaction tracking
- **Process Table** for managing all active processes
- **Multithreading** for concurrent transaction processing
- Process state transitions: NEW → READY → RUNNING → COMPLETED

### 3. CPU Scheduling
- **Round Robin Scheduling** with configurable time quantum (2 units)
- Gantt chart visualization
- **Performance Metrics:**
  - Average Waiting Time
  - Average Turnaround Time
  - CPU Utilization Percentage

### 4. Memory Management
- **LRU (Least Recently Used) Page Replacement Algorithm**
- Simulated memory pages (max 3 pages in memory)
- Page fault handling and replacement visualization

### 5. Disk I/O Management
- **FCFS (First Come First Serve)** disk scheduling
- **SCAN (Elevator Algorithm)** disk scheduling
- Seek time calculation and performance analysis

### 6. File Management
- **File Allocation Table (FAT)** implementation
- Dynamic block allocation for files
- File-to-block mapping visualization

### 7. Inter-Process Communication (IPC)
- **Synchronous messaging** (blocking with acknowledgment)
- **Asynchronous messaging** (non-blocking)
- **Process-to-Process communication** with dedicated queues
- Global message queue management
- Process completion notifications

## Expected Output

### Menu Interface
```
============================================================
===      BANKING SYSTEM - OS SIMULATION MENU          ===
============================================================

[ACCOUNT OPERATIONS]
1. Create Account
2. Deposit Money
3. Withdraw Money
4. Check Balance

[PROCESS & THREAD MANAGEMENT]
5. Process Transactions Concurrently (Multithreading)
6. View Process Table

[CPU SCHEDULING]
7. Simulate Round Robin Scheduling with Metrics

[MEMORY MANAGEMENT]
8. Simulate Memory Management (LRU Paging)

[DISK I/O & FILE MANAGEMENT]
9. Simulate Disk Scheduling (FCFS)
10. Simulate Disk Scheduling (SCAN)
11. Manage File Allocation Table (FAT)

[INTER-PROCESS COMMUNICATION]
12. Send IPC Message (Synchronous)
13. Send IPC Message (Asynchronous)
14. Process-to-Process Communication
15. View IPC Status

16. Exit
============================================================
```

### Sample Output: Process Table (Option 6)
```
=== PROCESS TABLE ===
   PID    Transaction      Status  Wait Time
--------------------------------------------------
     1             T1   COMPLETED          0
     2             T2   COMPLETED          2
     3             T3   COMPLETED          4
     4             T4   COMPLETED          6
```

### Sample Output: CPU Scheduling Metrics (Option 7)
```
=== ROUND ROBIN CPU SCHEDULING (Time Quantum = 2) ===

Gantt Chart:
       PID    Transaction     Start Time       End Time      Wait Time
----------------------------------------------------------------------
         1             T1              0              2              0
         2             T2              2              4              2
         3             T3              4              6              4
         4             T4              6              8              6

=== CPU SCHEDULING METRICS ===
Total Processes: 4
Average Waiting Time: 3.00 units
Average Turnaround Time: 5.00 units
CPU Utilization: 100.00%
Total CPU Time: 8 units
```

### Sample Output: Memory Management LRU (Option 8)
```
[LRU PAGING DEMO]
Accessed memory page: 1
Accessed memory page: 2
Accessed memory page: 3
Accessed memory page: 4
Removed page: 1
Accessed memory page: 1

Memory Map:
Page 2: Transaction Log - Page 2
Page 3: User Data - Page 3
Page 4: Audit Trail - Page 4
```

### Sample Output: Disk Scheduling FCFS (Option 9)
```
=== DISK SCHEDULING: FCFS (First Come First Serve) ===
Request Sequence: 98 183 37 122 14 124 65 67 

Seek Sequence:
      From        To      Seek Time
-----------------------------------
         0        98             98
        98       183             85
       183        37            146
        37       122             85
       122        14            108
        14       124            110
       124        65             59
        65        67              2

Total Seek Time: 693
Average Seek Time: 86.625
```

### Sample Output: Disk Scheduling SCAN (Option 10)
```
=== DISK SCHEDULING: SCAN (Elevator Algorithm) ===
Initial Head Position: 53
Disk Size: 200

Seek Sequence: 53 -> 65 -> 67 -> 98 -> 122 -> 124 -> 183 -> 199 -> 37 -> 14 -> 

Total Seek Time: 331
Average Seek Time: 41.375
```

### Sample Output: File Allocation Table (Option 11)
```
[FILE ALLOCATION TABLE MANAGEMENT]
[FAT] Allocated 5 blocks for file: transaction_log.txt
[FAT] Blocks: 0 1 2 3 4 
[FAT] Allocated 3 blocks for file: account_data.dat
[FAT] Blocks: 5 6 7 
[FAT] Allocated 7 blocks for file: audit_trail.log
[FAT] Blocks: 8 9 10 11 12 13 14 

=== FILE ALLOCATION TABLE ===
            Filename         Allocated Blocks
--------------------------------------------------
transaction_log.txt    0 1 2 3 4 
  account_data.dat    5 6 7 
   audit_trail.log    8 9 10 11 12 13 14 
```

### Sample Output: IPC Process-to-Process (Option 14)
```
Enter source PID: 1
Enter target PID: 2
Enter message: Transaction data ready for processing
[IPC] Process-to-Process Message Sent: [PID 1 -> PID 2]: Transaction data ready for processing
[IPC] Process 2 Received: [PID 1 -> PID 2]: Transaction data ready for processing
```

### Sample Output: IPC Status (Option 15)
```
=== IPC STATUS ===
Global Queue Size: 2
Process Queues: 3 active
  PID 1: 0 messages
  PID 2: 1 messages
  PID 3: 2 messages
```

## Technical Details

### Data Structures Used
- **STL Containers:** map, vector, deque, queue
- **Synchronization:** mutex, lock_guard
- **Threading:** std::thread
- **File I/O:** fstream for persistent storage

### Algorithms Implemented
- Round Robin CPU Scheduling
- LRU Page Replacement
- FCFS Disk Scheduling
- SCAN Disk Scheduling
- File Allocation Table Management

## Compilation & Execution

### Windows (Visual Studio)
```bash
cl /EHsc "OS PROJECT_AMU.cpp" /Fe"OS_Banking_System.exe"
OS_Banking_System.exe
```

### Windows (MinGW)
```bash
g++ -std=c++17 "OS PROJECT_AMU.cpp" -o OS_Banking_System.exe
OS_Banking_System.exe
```

### Linux/Mac
```bash
g++ -std=c++17 "OS PROJECT_AMU.cpp" -o OS_Banking_System -pthread
./OS_Banking_System
```

## Features Demonstrated

✅ Process Control Block (PCB) Implementation  
✅ Process Table Management  
✅ Multithreading with Mutex Synchronization  
✅ Round Robin Scheduling with Performance Metrics  
✅ LRU Page Replacement Algorithm  
✅ FCFS and SCAN Disk Scheduling  
✅ File Allocation Table (FAT)  
✅ Synchronous/Asynchronous IPC  
✅ Process-to-Process Communication  
✅ File Persistence (accounts.txt)  

## Project Structure

```
Operating-Systems-Project/
├── OS PROJECT_AMU.cpp    # Main source file
└── README.md             # Documentation
```

## Author Information

**Developer:** Muneeb  
**Project Type:** Operating Systems Simulation  
**Language:** C++17  
**Purpose:** Academic demonstration of OS concepts in a banking context

---

*This project demonstrates the integration of multiple operating system concepts in a practical banking simulation environment.*
