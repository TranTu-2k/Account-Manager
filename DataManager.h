#ifndef DATA_MANAGER_H
#define DATA_MANAGER_H

#include <string>
#include <vector>
#include <map>
#include "User.h"
#include "Wallet.h"

class DataManager {
private:
    const std::string USER_DATA_FILE;
    const std::string WALLET_DATA_FILE;
    const std::string TRANSACTION_DATA_FILE;
    const std::string BACKUP_DIR;
    
    std::map<std::string, User> users;
    std::map<std::string, Wallet> wallets;
    std::map<std::string, Transaction> transactions;
    
    bool createBackup();
    bool restoreFromBackup(const std::string& backupTimestamp);
    std::string generateUniqueId() const;

public:
    DataManager();
    ~DataManager();
    
    bool saveUser(const User& user);
    bool deleteUser(const std::string& username);
    User* getUser(const std::string& username);
    std::vector<User> getAllUsers() const;
    bool userExists(const std::string& username) const;
    
    std::string createWallet(const std::string& ownerUsername);
    Wallet* getWallet(const std::string& walletId);
    Wallet* getWalletByOwner(const std::string& username);
    bool saveWallet(const Wallet& wallet);
    
    std::string createTransaction(const std::string& senderWalletId, 
                                 const std::string& receiverWalletId,
                                 double amount,
                                 const std::string& description = "");
    Transaction* getTransaction(const std::string& transactionId);
    std::vector<Transaction> getTransactionsByWallet(const std::string& walletId) const;
    bool saveTransaction(const Transaction& transaction);
    
    bool loadData();
    bool saveData();
};

#endif