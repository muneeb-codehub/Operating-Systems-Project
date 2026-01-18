#include <iostream>
#include <vector>
#include <string>
#include <tuple>
#include <thread>
#include <map>
#include <deque>
#include <fstream>
#include <algorithm>
#include <mutex>
#include <iomanip>
#include <chrono>
#include <queue>

using namespace std;

// --- Process Control Block ---
struct ProcessControlBlock {
    int pid;
    string status; // "NEW", "READY", "RUNNING", "WAITING", "COMPLETED"
    string transaction_id;
    int arrival_time;
    int burst_time;
    int waiting_time;
    int turnaround_time;
    
    ProcessControlBlock(int p, string tid) 
        : pid(p), transaction_id(tid), status("NEW"), 
          arrival_time(0), burst_time(1), waiting_time(0), turnaround_time(0) {}
};

// --- Process Table ---
class ProcessTable {
private:
    vector<ProcessControlBlock> processes;
    mutex table_mutex;
    int next_pid;
    
public:
    ProcessTable() : next_pid(1) {}
    
    int createProcess(const string& transaction_id) {
        lock_guard<mutex> lock(table_mutex);
        ProcessControlBlock pcb(next_pid++, transaction_id);
        pcb.status = "READY";
        processes.push_back(pcb);
        cout << "[PROCESS TABLE] Created Process PID: " << pcb.pid << " for Transaction: " << transaction_id << endl;
        return pcb.pid;
    }
    
    void updateStatus(int pid, const string& new_status) {
        lock_guard<mutex> lock(table_mutex);
        for (auto& pcb : processes) {
            if (pcb.pid == pid) {
                pcb.status = new_status;
                cout << "[PROCESS TABLE] PID " << pid << " status: " << new_status << endl;
                break;
            }
        }
    }
    
    void setWaitingTime(int pid, int wtime) {
        lock_guard<mutex> lock(table_mutex);
        for (auto& pcb : processes) {
            if (pcb.pid == pid) {
                pcb.waiting_time = wtime;
                break;
            }
        }
    }
    
    void displayProcessTable() {
        lock_guard<mutex> lock(table_mutex);
        cout << "\n=== PROCESS TABLE ===" << endl;
        cout << setw(6) << "PID" << setw(15) << "Transaction" << setw(12) << "Status" 
             << setw(10) << "Wait Time" << endl;
        cout << string(50, '-') << endl;
        for (const auto& pcb : processes) {
            cout << setw(6) << pcb.pid << setw(15) << pcb.transaction_id 
                 << setw(12) << pcb.status << setw(10) << pcb.waiting_time << endl;
        }
    }
    
    vector<ProcessControlBlock> getProcesses() {
        lock_guard<mutex> lock(table_mutex);
        return processes;
    }
};

// --- Account Management ---
class AccountManager {
private:
    map<string, double> accounts;
    mutex account_mutex;
    const string ACCOUNT_FILE = "accounts.txt";

    // Helper function to load accounts from the file
    void loadAccountsFromFile() {
        ifstream infile(ACCOUNT_FILE);
        if (!infile.is_open()) {
            return; // File does not exist, nothing to load
        }

        accounts.clear();
        string account_id;
        double balance;
        while (infile >> account_id >> balance) {
            accounts[account_id] = balance;
        }
        infile.close();
    }

    // Helper function to save all accounts to the file
    void saveAccountsToFile() {
        ofstream outfile(ACCOUNT_FILE, ios::trunc);
        for (const auto& entry : accounts) {
            outfile << entry.first << " " << entry.second << endl;
        }
        outfile.close();
    }

public:
    AccountManager() {
        loadAccountsFromFile(); // Load accounts when the system starts
    }

    void createAccount(const string& account_id, double initial_balance) {
        lock_guard<mutex> lock(account_mutex);

        if (accounts.find(account_id) != accounts.end()) {
            cout << "Account ID already exists!" << endl;
            return;
        }

        accounts[account_id] = initial_balance;

        // Append the new account to the file
        ofstream outfile(ACCOUNT_FILE, ios::app);
        if (outfile.is_open()) {
            outfile << account_id << " " << initial_balance << endl;
            outfile.close();
        }

        cout << "Account created for " << account_id << " with balance: " << initial_balance << endl;
    }

    void deposit(const string& account_id, double amount) {
        lock_guard<mutex> lock(account_mutex);
        if (accounts.find(account_id) != accounts.end()) {
            accounts[account_id] += amount;
            saveAccountsToFile(); // Update the file with the new balance
            cout << "Deposited " << amount << " to account " << account_id << endl;
        } else {
            cout << "Account " << account_id << " not found!" << endl;
        }
    }

    void withdraw(const string& account_id, double amount) {
        lock_guard<mutex> lock(account_mutex);
        if (accounts.find(account_id) != accounts.end()) {
            if (accounts[account_id] >= amount) {
                accounts[account_id] -= amount;
                saveAccountsToFile(); // Update the file with the new balance
                cout << "Withdrew " << amount << " from account " << account_id << endl;
            } else {
                cout << "Insufficient balance in account " << account_id << endl;
            }
        } else {
            cout << "Account " << account_id << " not found!" << endl;
        }
    }

    void checkBalance(const string& account_id) {
        lock_guard<mutex> lock(account_mutex);
        if (accounts.find(account_id) != accounts.end()) {
            cout << "Balance for account " << account_id << ": " << accounts[account_id] << endl;
        } else {
            cout << "Account " << account_id << " not found!" << endl;
        }
    }
};

// --- Transaction Management ---
class TransactionManager {
private:
    AccountManager* account_manager;
    ProcessTable* process_table;

public:
    TransactionManager(AccountManager* am, ProcessTable* pt) 
        : account_manager(am), process_table(pt) {}

    void createTransaction(const string& id, const string& action, const string& account_id, double amount = 0, int pid = 0) {
        if (pid > 0 && process_table) {
            process_table->updateStatus(pid, "RUNNING");
        }
        
        cout << "Transaction " << id << " started" << endl;
        if (action == "deposit") {
            account_manager->deposit(account_id, amount);
        } else if (action == "withdraw") {
            account_manager->withdraw(account_id, amount);
        } else if (action == "balance") {
            account_manager->checkBalance(account_id);
        }
        cout << "Transaction " << id << " completed" << endl;
        
        if (pid > 0 && process_table) {
            process_table->updateStatus(pid, "COMPLETED");
        }
    }
};

// --- CPU Scheduling ---
class CPUScheduler {
private:
    TransactionManager* transaction_manager;
    ProcessTable* process_table;
    const int TIME_QUANTUM = 2;

public:
    CPUScheduler(TransactionManager* tm, ProcessTable* pt) 
        : transaction_manager(tm), process_table(pt) {}

    void simulateRoundRobin(const vector<tuple<string, string, string, double>>& transactions) {
        cout << "\n=== ROUND ROBIN CPU SCHEDULING (Time Quantum = " << TIME_QUANTUM << ") ===\n";
        cout << "\nGantt Chart:\n";
        cout << setw(10) << "PID" << setw(15) << "Transaction" << setw(15) << "Start Time" 
             << setw(15) << "End Time" << setw(15) << "Wait Time" << endl;
        cout << string(70, '-') << endl;

        int time = 0;
        int total_waiting_time = 0;
        int total_turnaround_time = 0;
        vector<int> pids;
        
        for (const auto& transaction : transactions) {
            string id, action, account_id;
            double amount;
            tie(id, action, account_id, amount) = transaction;

            // Create process
            int pid = process_table->createProcess(id);
            pids.push_back(pid);
            
            int start_time = time;
            int waiting_time = start_time;
            
            cout << setw(10) << pid << setw(15) << id << setw(15) << start_time 
                 << setw(15) << (start_time + TIME_QUANTUM) << setw(15) << waiting_time << endl;

            process_table->setWaitingTime(pid, waiting_time);
            
            thread t(&TransactionManager::createTransaction, transaction_manager, 
                    id, action, account_id, amount, pid);
            t.join();
            
            time += TIME_QUANTUM;
            total_waiting_time += waiting_time;
            total_turnaround_time += (time);
        }
        
        // Calculate metrics
        double avg_waiting_time = (double)total_waiting_time / transactions.size();
        double avg_turnaround_time = (double)total_turnaround_time / transactions.size();
        double cpu_utilization = ((double)transactions.size() * TIME_QUANTUM / time) * 100;
        
        cout << "\n=== CPU SCHEDULING METRICS ===" << endl;
        cout << "Total Processes: " << transactions.size() << endl;
        cout << "Average Waiting Time: " << fixed << setprecision(2) << avg_waiting_time << " units" << endl;
        cout << "Average Turnaround Time: " << avg_turnaround_time << " units" << endl;
        cout << "CPU Utilization: " << cpu_utilization << "%" << endl;
        cout << "Total CPU Time: " << time << " units" << endl;
        
        process_table->displayProcessTable();
    }
};

// --- Memory Management ---
class MemoryManager {
private:
    map<int, string> memory_map;
    deque<int> page_order;
    const int MAX_PAGES = 3;

public:
    void accessMemoryPage(int page_id, const string& data) {
        auto it = find(page_order.begin(), page_order.end(), page_id);
        if (it != page_order.end()) {
            page_order.erase(it);
        }

        page_order.push_back(page_id);
        memory_map[page_id] = data;

        if (page_order.size() > MAX_PAGES) {
            int removed_page = page_order.front();
            page_order.pop_front();
            memory_map.erase(removed_page);
            cout << "Removed page: " << removed_page << endl;
        }

        cout << "Accessed memory page: " << page_id << endl;
    }

    void displayMemoryMap() {
        cout << "\nMemory Map:" << endl;
        for (const auto& page : memory_map) {
            cout << "Page " << page.first << ": " << page.second << endl;
        }
    }
};

// --- File Allocation Table ---
class FileAllocationTable {
private:
    map<string, vector<int>> file_blocks;
    int next_block;
    
public:
    FileAllocationTable() : next_block(0) {}
    
    void allocateFile(const string& filename, int size) {
        vector<int> blocks;
        for (int i = 0; i < size; i++) {
            blocks.push_back(next_block++);
        }
        file_blocks[filename] = blocks;
        cout << "[FAT] Allocated " << size << " blocks for file: " << filename << endl;
        cout << "[FAT] Blocks: ";
        for (int block : blocks) {
            cout << block << " ";
        }
        cout << endl;
    }
    
    void displayFAT() {
        cout << "\n=== FILE ALLOCATION TABLE ===" << endl;
        cout << setw(20) << "Filename" << setw(30) << "Allocated Blocks" << endl;
        cout << string(50, '-') << endl;
        for (const auto& entry : file_blocks) {
            cout << setw(20) << entry.first << "    ";
            for (int block : entry.second) {
                cout << block << " ";
            }
            cout << endl;
        }
    }
};

// --- Disk I/O Management with Scheduling ---
class DiskManager {
private:
    vector<int> disk_blocks;
    FileAllocationTable fat;

public:
    void simulateDiskAccessFCFS(const vector<int>& block_requests) {
        cout << "\n=== DISK SCHEDULING: FCFS (First Come First Serve) ===" << endl;
        cout << "Request Sequence: ";
        for (int block : block_requests) {
            cout << block << " ";
        }
        cout << endl;
        
        int head = 0;
        int total_seek_time = 0;
        
        cout << "\nSeek Sequence:\n";
        cout << setw(10) << "From" << setw(10) << "To" << setw(15) << "Seek Time" << endl;
        cout << string(35, '-') << endl;
        
        for (int block : block_requests) {
            int seek_time = abs(block - head);
            total_seek_time += seek_time;
            cout << setw(10) << head << setw(10) << block << setw(15) << seek_time << endl;
            head = block;
            disk_blocks.push_back(block);
        }
        
        cout << "\nTotal Seek Time: " << total_seek_time << endl;
        cout << "Average Seek Time: " << (double)total_seek_time / block_requests.size() << endl;
    }
    
    void simulateDiskAccessSCAN(vector<int> block_requests, int initial_head, int disk_size) {
        cout << "\n=== DISK SCHEDULING: SCAN (Elevator Algorithm) ===" << endl;
        sort(block_requests.begin(), block_requests.end());
        
        vector<int> left, right;
        for (int req : block_requests) {
            if (req < initial_head)
                left.push_back(req);
            else
                right.push_back(req);
        }
        
        reverse(left.begin(), left.end());
        
        cout << "Initial Head Position: " << initial_head << endl;
        cout << "Disk Size: " << disk_size << endl;
        cout << "\nSeek Sequence: " << initial_head << " -> ";
        
        int head = initial_head;
        int total_seek_time = 0;
        
        // Move right
        for (int req : right) {
            total_seek_time += abs(req - head);
            head = req;
            cout << head << " -> ";
        }
        
        // Move to end
        total_seek_time += abs(disk_size - 1 - head);
        head = disk_size - 1;
        cout << head << " -> ";
        
        // Move left
        for (int req : left) {
            total_seek_time += abs(req - head);
            head = req;
            cout << head << " -> ";
        }
        
        cout << "\n\nTotal Seek Time: " << total_seek_time << endl;
        cout << "Average Seek Time: " << (double)total_seek_time / block_requests.size() << endl;
    }
    
    FileAllocationTable* getFAT() {
        return &fat;
    }
};

// --- Inter-Process Communication ---
class IPCManager {
private:
    map<int, queue<string>> process_queues; // Per-process message queues
    deque<string> global_queue;
    mutex ipc_mutex;

public:
    // Send message to specific process (Process-to-Process)
    void sendMessageToProcess(int source_pid, int target_pid, const string& message) {
        lock_guard<mutex> lock(ipc_mutex);
        string formatted_msg = "[PID " + to_string(source_pid) + " -> PID " 
                              + to_string(target_pid) + "]: " + message;
        process_queues[target_pid].push(formatted_msg);
        cout << "[IPC] Process-to-Process Message Sent: " << formatted_msg << endl;
    }
    
    // Receive message for specific process
    void receiveMessageForProcess(int pid) {
        lock_guard<mutex> lock(ipc_mutex);
        if (!process_queues[pid].empty()) {
            string message = process_queues[pid].front();
            process_queues[pid].pop();
            cout << "[IPC] Process " << pid << " Received: " << message << endl;
        } else {
            cout << "[IPC] No messages for Process " << pid << endl;
        }
    }
    
    // Synchronous message send (wait for acknowledgment)
    void sendMessageSync(const string& message) {
        lock_guard<mutex> lock(ipc_mutex);
        global_queue.push_back(message);
        cout << "[IPC-SYNC] Message sent (blocking): " << message << endl;
        // Simulate waiting for acknowledgment
        this_thread::sleep_for(chrono::milliseconds(100));
        cout << "[IPC-SYNC] Acknowledgment received" << endl;
    }
    
    // Asynchronous message send (no wait)
    void sendMessageAsync(const string& message) {
        lock_guard<mutex> lock(ipc_mutex);
        global_queue.push_back(message);
        cout << "[IPC-ASYNC] Message sent (non-blocking): " << message << endl;
        // No waiting, returns immediately
    }

    void receiveMessage() {
        lock_guard<mutex> lock(ipc_mutex);
        if (!global_queue.empty()) {
            string message = global_queue.front();
            global_queue.pop_front();
            cout << "[IPC] Message received: " << message << endl;
        } else {
            cout << "[IPC] No messages in global queue" << endl;
        }
    }
    
    // Notify process completion
    void notifyProcessCompletion(int pid) {
        lock_guard<mutex> lock(ipc_mutex);
        string notification = "Process " + to_string(pid) + " has completed";
        global_queue.push_back(notification);
        cout << "[IPC] Notification: " << notification << endl;
    }
    
    void displayIPCStatus() {
        lock_guard<mutex> lock(ipc_mutex);
        cout << "\n=== IPC STATUS ===" << endl;
        cout << "Global Queue Size: " << global_queue.size() << endl;
        cout << "Process Queues: " << process_queues.size() << " active" << endl;
        for (const auto& pq : process_queues) {
            cout << "  PID " << pq.first << ": " << pq.second.size() << " messages" << endl;
        }
    }
};

// --- Banking System ---
class BankingSystem {
private:
    AccountManager account_manager;
    ProcessTable process_table;
    TransactionManager transaction_manager;
    CPUScheduler cpu_scheduler;
    MemoryManager memory_manager;
    DiskManager disk_manager;
    IPCManager ipc_manager;

public:
    BankingSystem()
        : transaction_manager(&account_manager, &process_table),
          cpu_scheduler(&transaction_manager, &process_table) {}

    void menu() {
        while (true) {
            cout << "\n" << string(60, '=') << endl;
            cout << "===      BANKING SYSTEM - OS SIMULATION MENU          ===" << endl;
            cout << string(60, '=') << endl;
            cout << "\n[ACCOUNT OPERATIONS]" << endl;
            cout << "1. Create Account" << endl;
            cout << "2. Deposit Money" << endl;
            cout << "3. Withdraw Money" << endl;
            cout << "4. Check Balance" << endl;
            cout << "\n[PROCESS & THREAD MANAGEMENT]" << endl;
            cout << "5. Process Transactions Concurrently (Multithreading)" << endl;
            cout << "6. View Process Table" << endl;
            cout << "\n[CPU SCHEDULING]" << endl;
            cout << "7. Simulate Round Robin Scheduling with Metrics" << endl;
            cout << "\n[MEMORY MANAGEMENT]" << endl;
            cout << "8. Simulate Memory Management (LRU Paging)" << endl;
            cout << "\n[DISK I/O & FILE MANAGEMENT]" << endl;
            cout << "9. Simulate Disk Scheduling (FCFS)" << endl;
            cout << "10. Simulate Disk Scheduling (SCAN)" << endl;
            cout << "11. Manage File Allocation Table (FAT)" << endl;
            cout << "\n[INTER-PROCESS COMMUNICATION]" << endl;
            cout << "12. Send IPC Message (Synchronous)" << endl;
            cout << "13. Send IPC Message (Asynchronous)" << endl;
            cout << "14. Process-to-Process Communication" << endl;
            cout << "15. View IPC Status" << endl;
            cout << "\n16. Exit" << endl;
            cout << string(60, '=') << endl;

            int choice;
            cin >> choice;

            if (choice == 1) {
                string customer_id;
                double initial_balance;
                cout << "Enter Customer ID: ";
                cin >> customer_id;
                cout << "Enter Initial Balance: ";
                cin >> initial_balance;
                account_manager.createAccount(customer_id, initial_balance);
            } else if (choice == 2) {
                string account_id;
                double amount;
                cout << "Enter Account ID: ";
                cin >> account_id;
                cout << "Enter Amount to Deposit: ";
                cin >> amount;
                account_manager.deposit(account_id, amount);
            } else if (choice == 3) {
                string account_id;
                double amount;
                cout << "Enter Account ID: ";
                cin >> account_id;
                cout << "Enter Amount to Withdraw: ";
                cin >> amount;
                account_manager.withdraw(account_id, amount);
            } else if (choice == 4) {
                string account_id;
                cout << "Enter Account ID: ";
                cin >> account_id;
                account_manager.checkBalance(account_id);
            } else if (choice == 5) {
                cout << "\n[MULTITHREADING DEMO] Processing transactions concurrently...\n";
                vector<thread> threads;
                int pid1 = process_table.createProcess("T1");
                int pid2 = process_table.createProcess("T2");
                
                threads.emplace_back(&TransactionManager::createTransaction, &transaction_manager, "T1", "deposit", "111", 1000.0, pid1);
                threads.emplace_back(&TransactionManager::createTransaction, &transaction_manager, "T2", "withdraw", "111", 500.0, pid2);
                
                for (auto& t : threads) {
                    t.join();
                }
                cout << "All transactions completed!\n";
                ipc_manager.notifyProcessCompletion(pid1);
                ipc_manager.notifyProcessCompletion(pid2);
                
            } else if (choice == 6) {
                process_table.displayProcessTable();
                
            } else if (choice == 7) {
                vector<tuple<string, string, string, double>> transactions = {
                    {"T1", "deposit", "A1", 500},
                    {"T2", "withdraw", "A2", 200},
                    {"T3", "balance", "A1", 0},
                    {"T4", "deposit", "A2", 300}};
                cpu_scheduler.simulateRoundRobin(transactions);
                
            } else if (choice == 8) {
                cout << "\n[LRU PAGING DEMO]\n";
                memory_manager.accessMemoryPage(1, "Account Data - Page 1");
                memory_manager.accessMemoryPage(2, "Transaction Log - Page 2");
                memory_manager.accessMemoryPage(3, "User Data - Page 3");
                memory_manager.accessMemoryPage(4, "Audit Trail - Page 4");
                memory_manager.accessMemoryPage(1, "Account Data - Page 1 (Re-access)");
                memory_manager.displayMemoryMap();
                
            } else if (choice == 9) {
                vector<int> block_requests = {98, 183, 37, 122, 14, 124, 65, 67};
                disk_manager.simulateDiskAccessFCFS(block_requests);
                
            } else if (choice == 10) {
                vector<int> block_requests = {98, 183, 37, 122, 14, 124, 65, 67};
                int initial_head = 53;
                int disk_size = 200;
                disk_manager.simulateDiskAccessSCAN(block_requests, initial_head, disk_size);
                
            } else if (choice == 11) {
                FileAllocationTable* fat = disk_manager.getFAT();
                cout << "\n[FILE ALLOCATION TABLE MANAGEMENT]\n";
                fat->allocateFile("transaction_log.txt", 5);
                fat->allocateFile("account_data.dat", 3);
                fat->allocateFile("audit_trail.log", 7);
                fat->displayFAT();
                
            } else if (choice == 12) {
                string message;
                cout << "Enter message for synchronous send: ";
                cin.ignore();
                getline(cin, message);
                ipc_manager.sendMessageSync(message);
                ipc_manager.receiveMessage();
                
            } else if (choice == 13) {
                string message;
                cout << "Enter message for asynchronous send: ";
                cin.ignore();
                getline(cin, message);
                ipc_manager.sendMessageAsync(message);
                cout << "[INFO] Returned immediately (non-blocking)\n";
                
            } else if (choice == 14) {
                int source_pid, target_pid;
                string message;
                cout << "Enter source PID: ";
                cin >> source_pid;
                cout << "Enter target PID: ";
                cin >> target_pid;
                cout << "Enter message: ";
                cin.ignore();
                getline(cin, message);
                ipc_manager.sendMessageToProcess(source_pid, target_pid, message);
                ipc_manager.receiveMessageForProcess(target_pid);
                
            } else if (choice == 15) {
                ipc_manager.displayIPCStatus();
                
            } else if (choice == 16) {
                cout << "\n" << string(60, '=') << endl;
                cout << "Exiting Banking System. Goodbye!" << endl;
                cout << string(60, '=') << endl;
                break;
            } else {
                cout << "\n[ERROR] Invalid choice! Please try again.\n";
            }
        }
    }
};

// --- Main Function ---
int main() {
    BankingSystem banking_system;
    banking_system.menu();
    return 0;
}

