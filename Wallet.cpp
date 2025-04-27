#include "Wallet.h"
#include <ctime>

Transaction::Transaction() :
    transactionId(""),
    senderWalletId(""),
    receiverWalletId(""),
    amount(0.0),
    timestamp(time(NULL)),
    isSuccessful(false),
    description("") {}

Transaction::Transaction(const std::string& transactionId,
                       const std::string& senderWalletId,
                       const std::string& receiverWalletId,
                       double amount,
                       const std::string& description) :
    transactionId(transactionId),
    senderWalletId(senderWalletId),
    receiverWalletId(receiverWalletId),
    amount(amount),
    timestamp(time(NULL)),
    isSuccessful(false),
    description(description) {}

std::string Transaction::getTransactionId() const {
    return transactionId;
}

std::string Transaction::getSenderWalletId() const {
    return senderWalletId;
}

std::string Transaction::getReceiverWalletId() const {
    return receiverWalletId;
}

double Transaction::getAmount() const {
    return amount;
}

time_t Transaction::getTimestamp() const {
    return timestamp;
}

bool Transaction::getIsSuccessful() const {
    return isSuccessful;
}

std::string Transaction::getDescription() const {
    return description;
}

void Transaction::setIsSuccessful(bool isSuccessful) {
    this->isSuccessful = isSuccessful;
}

Wallet::Wallet() : 
    walletId(""),
    ownerUsername(""),
    balance(0.0) {}

Wallet::Wallet(const std::string& walletId, const std::string& ownerUsername, double initialBalance) :
    walletId(walletId),
    ownerUsername(ownerUsername),
    balance(initialBalance) {}

std::string Wallet::getWalletId() const {
    return walletId;
}

std::string Wallet::getOwnerUsername() const {
    return ownerUsername;
}

double Wallet::getBalance() const {
    return balance;
}

const std::vector<std::string>& Wallet::getTransactionHistory() const {
    return transactionHistory;
}

bool Wallet::deductPoints(double amount) {
    if (amount > balance) {
        return false;
    }
    
    balance -= amount;
    return true;
}

void Wallet::addPoints(double amount) {
    balance += amount;
}

void Wallet::addTransactionToHistory(const std::string& transactionId) {
    transactionHistory.push_back(transactionId);
}
