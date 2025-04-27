#include "DataManager.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <sys/stat.h>

// Hàm tạo thư mục tương thích với C++98
bool createDirectory(const std::string& path) {
    #ifdef _WIN32
    return system(("mkdir " + path + " 2> nul").c_str()) == 0;
    #else
    return system(("mkdir -p " + path + " 2> /dev/null").c_str()) == 0;
    #endif
}

// Hàm kiểm tra file tồn tại tương thích với C++98
bool fileExists(const std::string& filename) {
    std::ifstream file(filename.c_str());
    return file.good();
}

// Hàm sao chép file tương thích với C++98
bool copyFile(const std::string& src, const std::string& dest) {
    std::ifstream source(src.c_str(), std::ios::binary);
    if (!source) return false;
    
    std::ofstream destination(dest.c_str(), std::ios::binary);
    if (!destination) return false;
    
    destination << source.rdbuf();
    return source && destination;
}

DataManager::DataManager() :
    USER_DATA_FILE("data/users.txt"),
    WALLET_DATA_FILE("data/wallets.txt"),
    TRANSACTION_DATA_FILE("data/transactions.txt"),
    BACKUP_DIR("data/backups/") {
    loadData();
}

DataManager::~DataManager() {
    saveData();
}

std::string DataManager::generateUniqueId() const {
    // Sử dụng rand() thay vì random device để tương thích với C++98
    srand(static_cast<unsigned int>(time(NULL)));
    
    const char* hex_chars = "0123456789abcdef";
    
    std::string uuid;
    for (int i = 0; i < 32; ++i) {
        if (i == 8 || i == 12 || i == 16 || i == 20) {
            uuid += "-";
        }
        uuid += hex_chars[rand() % 16];
    }
    
    return uuid;
}

bool DataManager::createBackup() {
    time_t now = time(NULL);
    tm* now_tm = localtime(&now);
    char timestamp[20];
    strftime(timestamp, sizeof(timestamp), "%Y%m%d_%H%M%S", now_tm);
    
    createDirectory(BACKUP_DIR);
    
    std::string userBackup = BACKUP_DIR + "users_" + timestamp + ".txt";
    std::string walletBackup = BACKUP_DIR + "wallets_" + timestamp + ".txt";
    std::string transactionBackup = BACKUP_DIR + "transactions_" + timestamp + ".txt";
    
    try {
        copyFile(USER_DATA_FILE, userBackup);
        copyFile(WALLET_DATA_FILE, walletBackup);
        copyFile(TRANSACTION_DATA_FILE, transactionBackup);
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Backup failed: " << e.what() << std::endl;
        return false;
    }
}

bool DataManager::restoreFromBackup(const std::string& backupTimestamp) {
    std::string userBackup = BACKUP_DIR + "users_" + backupTimestamp + ".txt";
    std::string walletBackup = BACKUP_DIR + "wallets_" + backupTimestamp + ".txt";
    std::string transactionBackup = BACKUP_DIR + "transactions_" + backupTimestamp + ".txt";
    
    if (!fileExists(userBackup) || 
        !fileExists(walletBackup) || 
        !fileExists(transactionBackup)) {
        std::cerr << "Backup files not found" << std::endl;
        return false;
    }
    
    createBackup();
    
    try {
        copyFile(userBackup, USER_DATA_FILE);
        copyFile(walletBackup, WALLET_DATA_FILE);
        copyFile(transactionBackup, TRANSACTION_DATA_FILE);
        
        loadData();
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Restoration failed: " << e.what() << std::endl;
        return false;
    }
}

bool DataManager::saveUser(const User& user) {
    users[user.getUsername()] = user;
    return true;
}

bool DataManager::deleteUser(const std::string& username) {
    std::map<std::string, User>::iterator it = users.find(username);
    if (it != users.end()) {
        users.erase(it);
        return true;
    }
    return false;
}

User* DataManager::getUser(const std::string& username) {
    std::map<std::string, User>::iterator it = users.find(username);
    if (it != users.end()) {
        return &(it->second);
    }
    return NULL;
}

std::vector<User> DataManager::getAllUsers() const {
    std::vector<User> userList;
    for (std::map<std::string, User>::const_iterator it = users.begin(); it != users.end(); ++it) {
        userList.push_back(it->second);
    }
    return userList;
}

bool DataManager::userExists(const std::string& username) const {
    return users.find(username) != users.end();
}

std::string DataManager::createWallet(const std::string& ownerUsername) {
    std::string walletId = generateUniqueId();
    
    Wallet wallet(walletId, ownerUsername);
    
    wallets[walletId] = wallet;
    
    return walletId;
}

Wallet* DataManager::getWallet(const std::string& walletId) {
    std::map<std::string, Wallet>::iterator it = wallets.find(walletId);
    if (it != wallets.end()) {
        return &(it->second);
    }
    return NULL;
}

Wallet* DataManager::getWalletByOwner(const std::string& username) {
    for (std::map<std::string, Wallet>::iterator it = wallets.begin(); it != wallets.end(); ++it) {
        if (it->second.getOwnerUsername() == username) {
            return &(it->second);
        }
    }
    return NULL;
}

bool DataManager::saveWallet(const Wallet& wallet) {
    wallets[wallet.getWalletId()] = wallet;
    return true;
}

std::string DataManager::createTransaction(const std::string& senderWalletId, 
                                         const std::string& receiverWalletId,
                                         double amount,
                                         const std::string& description) {
    std::string transactionId = generateUniqueId();
    
    Transaction transaction(transactionId, senderWalletId, receiverWalletId, amount, description);
    
    transactions[transactionId] = transaction;
    
    return transactionId;
}

Transaction* DataManager::getTransaction(const std::string& transactionId) {
    std::map<std::string, Transaction>::iterator it = transactions.find(transactionId);
    if (it != transactions.end()) {
        return &(it->second);
    }
    return NULL;
}

std::vector<Transaction> DataManager::getTransactionsByWallet(const std::string& walletId) const {
    std::vector<Transaction> walletTransactions;
    
    for (std::map<std::string, Transaction>::const_iterator it = transactions.begin(); it != transactions.end(); ++it) {
        const Transaction& transaction = it->second;
        if (transaction.getSenderWalletId() == walletId || 
            transaction.getReceiverWalletId() == walletId) {
            walletTransactions.push_back(transaction);
        }
    }
    
    return walletTransactions;
}

bool DataManager::saveTransaction(const Transaction& transaction) {
    transactions[transaction.getTransactionId()] = transaction;
    return true;
}

bool DataManager::loadData() {
    users.clear();
    wallets.clear();
    transactions.clear();
    
    createDirectory("data");
    
    try {
        std::ifstream userFile(USER_DATA_FILE.c_str());
        if (userFile.is_open()) {
            std::string line;
            while (std::getline(userFile, line)) {
                std::stringstream ss(line);
                std::string username, passwordHash, fullName, email, phoneNumber, roleStr;
                std::string isAutoGenStr, isFirstLoginStr, creationDateStr, lastLoginDateStr;
                
                std::getline(ss, username, ',');
                std::getline(ss, passwordHash, ',');
                std::getline(ss, fullName, ',');
                std::getline(ss, email, ',');
                std::getline(ss, phoneNumber, ',');
                std::getline(ss, roleStr, ',');
                std::getline(ss, isAutoGenStr, ',');
                std::getline(ss, isFirstLoginStr, ',');
                std::getline(ss, creationDateStr, ',');
                std::getline(ss, lastLoginDateStr, ',');
                
                UserRole role = (roleStr == "1") ? ADMIN : REGULAR;
                
                User user(username, passwordHash, fullName, email, phoneNumber, role);
                user.setIsAutoGeneratedPassword(isAutoGenStr == "1");
                user.setIsFirstLogin(isFirstLoginStr == "1");
                
                // Sử dụng atol thay vì stoll
                time_t creationDate = atol(creationDateStr.c_str());
                time_t lastLoginDate = atol(lastLoginDateStr.c_str());
                
                user.setLastLoginDate(lastLoginDate);
                
                users[username] = user;
            }
            userFile.close();
        }
        
        std::ifstream walletFile(WALLET_DATA_FILE.c_str());
        if (walletFile.is_open()) {
            std::string line;
            while (std::getline(walletFile, line)) {
                std::stringstream ss(line);
                std::string walletId, ownerUsername, balanceStr;
                
                std::getline(ss, walletId, ',');
                std::getline(ss, ownerUsername, ',');
                std::getline(ss, balanceStr, ',');
                
                // Sử dụng atof thay vì stod
                double balance = atof(balanceStr.c_str());
                
                Wallet wallet(walletId, ownerUsername, balance);
                
                std::string transactionId;
                while (std::getline(ss, transactionId, ',')) {
                    if (!transactionId.empty()) {
                        wallet.addTransactionToHistory(transactionId);
                    }
                }
                
                wallets[walletId] = wallet;
            }
            walletFile.close();
        }
        
        std::ifstream transactionFile(TRANSACTION_DATA_FILE.c_str());
        if (transactionFile.is_open()) {
            std::string line;
            while (std::getline(transactionFile, line)) {
                std::stringstream ss(line);
                std::string transactionId, senderWalletId, receiverWalletId, amountStr;
                std::string timestampStr, isSuccessfulStr, description;
                
                std::getline(ss, transactionId, ',');
                std::getline(ss, senderWalletId, ',');
                std::getline(ss, receiverWalletId, ',');
                std::getline(ss, amountStr, ',');
                std::getline(ss, timestampStr, ',');
                std::getline(ss, isSuccessfulStr, ',');
                std::getline(ss, description);
                
                // Sử dụng atof thay vì stod
                double amount = atof(amountStr.c_str());
                
                Transaction transaction(transactionId, senderWalletId, receiverWalletId, amount, description);
                transaction.setIsSuccessful(isSuccessfulStr == "1");
                
                transactions[transactionId] = transaction;
            }
            transactionFile.close();
        }
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error loading data: " << e.what() << std::endl;
        return false;
    }
}

bool DataManager::saveData() {
    try {
        createBackup();
        
        std::ofstream userFile(USER_DATA_FILE.c_str());
        if (userFile.is_open()) {
            for (std::map<std::string, User>::const_iterator it = users.begin(); it != users.end(); ++it) {
                const User& user = it->second;
                userFile << user.getUsername() << ","
                         << user.getPasswordHash() << ","
                         << user.getFullName() << ","
                         << user.getEmail() << ","
                         << user.getPhoneNumber() << ","
                         << (user.getRole() == ADMIN ? "1" : "0") << ","
                         << (user.getIsAutoGeneratedPassword() ? "1" : "0") << ","
                         << (user.getIsFirstLogin() ? "1" : "0") << ","
                         << user.getCreationDate() << ","
                         << user.getLastLoginDate() << std::endl;
            }
            userFile.close();
        }
        
        std::ofstream walletFile(WALLET_DATA_FILE.c_str());
        if (walletFile.is_open()) {
            for (std::map<std::string, Wallet>::const_iterator it = wallets.begin(); it != wallets.end(); ++it) {
                const Wallet& wallet = it->second;
                walletFile << wallet.getWalletId() << ","
                          << wallet.getOwnerUsername() << ","
                          << wallet.getBalance();
                
                const std::vector<std::string>& history = wallet.getTransactionHistory();
                for (size_t i = 0; i < history.size(); ++i) {
                    walletFile << "," << history[i];
                }
                walletFile << std::endl;
            }
            walletFile.close();
        }
        
        std::ofstream transactionFile(TRANSACTION_DATA_FILE.c_str());
        if (transactionFile.is_open()) {
            for (std::map<std::string, Transaction>::const_iterator it = transactions.begin(); it != transactions.end(); ++it) {
                const Transaction& transaction = it->second;
                transactionFile << transaction.getTransactionId() << ","
                               << transaction.getSenderWalletId() << ","
                               << transaction.getReceiverWalletId() << ","
                               << transaction.getAmount() << ","
                               << transaction.getTimestamp() << ","
                               << (transaction.getIsSuccessful() ? "1" : "0") << ","
                               << transaction.getDescription() << std::endl;
            }
            transactionFile.close();
        }
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error saving data: " << e.what() << std::endl;
        return false;
    }
} 
