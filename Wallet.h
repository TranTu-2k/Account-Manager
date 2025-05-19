#ifndef WALLET_H
#define WALLET_H

#include <string>
#include <vector>
#include <ctime>

// Enum for transaction status
enum TransactionStatus {
    PENDING,
    COMPLETED,
    FAILED,
    CANCELLED
};

class Transaction {
private:
    std::string transactionId;
    std::string senderWalletId;
    std::string receiverWalletId;
    double amount;
    time_t timestamp;
    bool isSuccessful;
    std::string description;
    TransactionStatus status;

public:
    Transaction();
    
    Transaction(const std::string& transactionId,
                const std::string& senderWalletId,
                const std::string& receiverWalletId,
                double amount,
                const std::string& description = "");

    std::string getTransactionId() const;
    std::string getSenderWalletId() const;
    std::string getReceiverWalletId() const;
    double getAmount() const;
    time_t getTimestamp() const;
    bool getIsSuccessful() const;
    std::string getDescription() const;
    TransactionStatus getStatus() const;
    
    // Convert status to string representation
    std::string getStatusString() const;

    void setIsSuccessful(bool isSuccessful);
    void setStatus(TransactionStatus status);
};

class Wallet {
private:
    std::string walletId;
    std::string ownerUsername;
    double balance;
    std::vector<std::string> transactionHistory;

public:
    Wallet();
    Wallet(const std::string& walletId, const std::string& ownerUsername, double initialBalance = 0.0);

    std::string getWalletId() const;
    std::string getOwnerUsername() const;
    double getBalance() const;
    const std::vector<std::string>& getTransactionHistory() const;

    bool deductPoints(double amount);
    void addPoints(double amount);
    void addTransactionToHistory(const std::string& transactionId);
};

#endif 