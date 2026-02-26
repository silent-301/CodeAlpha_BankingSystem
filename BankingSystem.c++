#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <iomanip>

using namespace std;

class Transaction {
public:
    double amount;
    string type;
    int transactionId;

    Transaction(double amt, string t, int id) : amount(amt), type(t), transactionId(id) {}

    void displayTransaction() const {
        cout << "| " << left << setw(5) << transactionId 
             << "| " << setw(18) << type 
             << "| $" << setw(10) << fixed << setprecision(2) << amount << " |" << endl;
    }
};

class Account {
public:
    int accNum;
    string emailId, phoneNum, accUsername, accType;
    int accPin;
    double balance;
    vector<Transaction> transactions;

    Account() : accNum(0), accPin(0), balance(0.0) {}

    void saveToFile() const {
        string fileName = accUsername + ".txt";
        ofstream outFile(fileName);
        if (outFile.is_open()) {
            outFile << accNum << "\n" << emailId << "\n" << phoneNum << "\n" 
                    << accUsername << "\n" << accPin << "\n" << accType << "\n" << balance << endl;
            
            outFile << transactions.size() << endl;
            for (const auto& t : transactions) {
                outFile << t.amount << "|" << t.type << "|" << t.transactionId << endl;
            }
            outFile.close();
            
            ifstream masterIn("master_list.txt");
            string name; bool exists = false;
            while (masterIn >> name) { if (name == accUsername) exists = true; }
            masterIn.close();
            if (!exists) {
                ofstream masterOut("master_list.txt", ios::app);
                masterOut << accUsername << endl;
            }
        }
    }

    void displayAccountDetails() const {
        cout << "\n==========================================" << endl;
        cout << "           ACCOUNT INFORMATION            " << endl;
        cout << "==========================================" << endl;
        cout << " Account Number   : " << accNum << endl;
        cout << " Username         : " << accUsername << endl;
        cout << " Email-ID         : " << emailId << endl;
        cout << " Phone Number     : " << phoneNum << endl;
        cout << " Account Type     : " << accType << endl;
        cout << " Current Balance  : $" << fixed << setprecision(2) << balance << endl;
        cout << "==========================================" << endl;
    }

    void viewTransactionHistory() const {
        cout << "\n--- TRANSACTION HISTORY (ACC: " << accNum << ") ---" << endl;
        if (transactions.empty()) {
            cout << " No transactions found." << endl;
            return;
        }
        cout << "--------------------------------------------" << endl;
        cout << "| ID   | Type               | Amount      |" << endl;
        cout << "--------------------------------------------" << endl;
        for (const auto& t : transactions) t.displayTransaction();
        cout << "--------------------------------------------" << endl;
    }
};

class BankingSystem {
private:
    vector<Account> accounts;

public:
    
    void loadData() {
        ifstream master("master_list.txt");
        string uname;
        while (master >> uname) {
            ifstream userFile(uname + ".txt");
            if (userFile.is_open()) {
                Account acc;
                userFile >> acc.accNum; userFile.ignore();
                getline(userFile, acc.emailId);
                getline(userFile, acc.phoneNum);
                getline(userFile, acc.accUsername);
                userFile >> acc.accPin >> acc.accType >> acc.balance;
                
                int tSize; userFile >> tSize;
                for (int i = 0; i < tSize; i++) {
                    double amt; string type; int id;
                    userFile >> amt; userFile.ignore();
                    getline(userFile, type, '|');
                    userFile >> id;
                    acc.transactions.emplace_back(amt, type, id);
                }
                accounts.push_back(acc);
                userFile.close();
            }
        }
    }

    
    Account* findByAccNum(int num) {
        for (auto& acc : accounts) {
            if (acc.accNum == num) return &acc;
        }
        return nullptr;
    }

    
    Account* findByPin(int pin) {
        for (auto& acc : accounts) {
            if (acc.accPin == pin) return &acc;
        }
        return nullptr;
    }

    void addAccount(const Account& newAcc) {
        accounts.push_back(newAcc);
        newAcc.saveToFile();
        cout << "\n[SUCCESS] Account created and saved to database." << endl;
    }

    void updateAccount(int accNum) {
        Account* acc = findByAccNum(accNum);
        if (acc) {
            int choice;
            cout << "\nUpdating Account: " << accNum << endl;
            cout << "1. Change Email\n2. Change Phone\n3. Change Type\n4. Exit Update" << endl;
            cout << "Choice: "; cin >> choice;
            switch(choice) {
                case 1: cout << "New Email: "; cin >> acc->emailId; break;
                case 2: cout << "New Phone: "; cin >> acc->phoneNum; break;
                case 3: cout << "New Type: "; cin >> acc->accType; break;
                default: return;
            }
            acc->saveToFile();
            cout << "[SUCCESS] Information updated in file." << endl;
        } else {
            cout << "[ERROR] Account number not found." << endl;
        }
    }

    void deleteAccount(int accNum) {
        for (auto it = accounts.begin(); it != accounts.end(); ++it) {
            if (it->accNum == accNum) {
                string fname = it->accUsername + ".txt";
                remove(fname.c_str());
                accounts.erase(it);
                cout << "[SUCCESS] Account " << accNum << " deleted from system." << endl;
                return;
            }
        }
        cout << "[ERROR] Account not found." << endl;
    }

    void viewAllSummary() {
        cout << "\n----------------------------------------------------------" << endl;
        cout << left << setw(12) << "ACC NO" << setw(15) << "USERNAME" << setw(15) << "TYPE" << "BALANCE" << endl;
        cout << "----------------------------------------------------------" << endl;
        for (const auto& a : accounts) {
            cout << left << setw(12) << a.accNum << setw(15) << a.accUsername << setw(15) << a.accType << "$" << a.balance << endl;
        }
        cout << "----------------------------------------------------------" << endl;
    }

    bool customerLogin(string user, int pin) {
        for (const auto& a : accounts) {
            if (a.accUsername == user && a.accPin == pin) return true;
        }
        return false;
    }
};

int main() {
    BankingSystem bank;
    bank.loadData();
    char mainChoice;

    while (true) {
        cout << "\n############################################" << endl;
        cout << "#         SILENT BANKING TERMINAL          #" << endl;
        cout << "############################################" << endl;
        cout << " [A] Admin Menu   [C] Customer Menu   [E] Exit" << endl;
        cout << " Selection: "; cin >> mainChoice;

        if (mainChoice == 'E' || mainChoice == 'e') break;

        if (mainChoice == 'A' || mainChoice == 'a') {
            string pass; cout << " Enter Admin Password: "; cin >> pass;
            if (pass == "admin123") {
                int aChoice;
                do {
                    cout << "\n--- ADMIN DASHBOARD ---" << endl;
                    cout << "1. Create Account\n2. Search Account (by No.)\n3. Update Account (by No.)\n4. Delete Account (by No.)\n5. View All Users\n6. Total Accounts\n7. Logout" << endl;
                    cout << "Action: "; cin >> aChoice;

                    if (aChoice == 1) {
                        Account n;
                        cout << "Assign Acc Number: "; cin >> n.accNum;
                        cout << "Username: "; cin >> n.accUsername;
                        cout << "Email: "; cin >> n.emailId;
                        cout << "Phone: "; cin >> n.phoneNum;
                        cout << "Set PIN: "; cin >> n.accPin;
                        cout << "Account Type: "; cin >> n.accType;
                        cout << "Opening Balance: "; cin >> n.balance;
                        n.transactions.emplace_back(n.balance, "Initial Deposit", 1);
                        bank.addAccount(n);
                    } else if (aChoice == 2) {
                        int num; cout << "Enter Account Number: "; cin >> num;
                        Account* res = bank.findByAccNum(num);
                        if (res) res->displayAccountDetails();
                        else cout << "Not Found." << endl;
                    } else if (aChoice == 3) {
                        int num; cout << "Enter Account Number to Update: "; cin >> num;
                        bank.updateAccount(num);
                    } else if (aChoice == 4) {
                        int num; cout << "Enter Account Number to Delete: "; cin >> num;
                        bank.deleteAccount(num);
                    } else if (aChoice == 5) bank.viewAllSummary();
                    else if (aChoice == 6) cout << "Total Registered: " << "X" << endl; // Use vector size logic

                } while (aChoice != 7);
            }
        } else if (mainChoice == 'C' || mainChoice == 'c') {
            string u; int p;
            cout << " Username: "; cin >> u;
            cout << " PIN: "; cin >> p;
            if (bank.customerLogin(u, p)) {
                Account* current = bank.findByPin(p);
                int cChoice;
                do {
                    cout << "\n--- CUSTOMER PORTAL (" << u << ") ---" << endl;
                    cout << "1. Balance Details\n2. Deposit\n3. Withdraw\n4. History\n5. Logout" << endl;
                    cout << "Action: "; cin >> cChoice;

                    if (cChoice == 1) current->displayAccountDetails();
                    else if (cChoice == 2) {
                        double amt; cout << "Amount: "; cin >> amt;
                        current->balance += amt;
                        current->transactions.emplace_back(amt, "Deposit", current->transactions.size() + 1);
                        current->saveToFile();
                        cout << "Balance updated." << endl;
                    } else if (cChoice == 3) {
                        double amt; cout << "Amount: "; cin >> amt;
                        if (current->balance >= amt) {
                            current->balance -= amt;
                            current->transactions.emplace_back(amt, "Withdrawal", current->transactions.size() + 1);
                            current->saveToFile();
                        } else cout << "Insufficient funds." << endl;
                    } else if (cChoice == 4) current->viewTransactionHistory();
                } while (cChoice != 5);
            } else cout << "Invalid Login." << endl;
        }
    }
    return 0;
}