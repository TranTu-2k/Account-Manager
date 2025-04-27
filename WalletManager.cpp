#include "WalletManager.h"
#include <iostream>

WalletManager::WalletManager(DataManager& dataManager, AuthManager& authManager)
    : dataManager(dataManager), authManager(authManager) {}

std::string WalletManager::createWallet(const std::string& ownerUsername) {
    return dataManager.createWallet(ownerUsername);
}

double WalletManager::getBalance(const std::string& walletId) {
    Wallet* wallet = dataManager.getWallet(walletId);
    if (wallet) {
        return wallet->getBalance();
    }
    return 0.0;
}

bool WalletManager::transferPoints(const std::string& senderWalletId, 
                                 const std::string& receiverWalletId, 
                                 double amount,
                                 const std::string& otpCode,
                                 const std::string& description) {
    Wallet* senderWallet = dataManager.getWallet(senderWalletId);
    if (!senderWallet) {
        std::cerr << "Sender wallet not found" << std::endl;
        return false;
    }
    
    std::string ownerUsername = senderWallet->getOwnerUsername();
    if (!authManager.verifyOTP(ownerUsername, otpCode)) {
        std::cerr << "Invalid OTP for transfer" << std::endl;
        return false;
    }
    
    Wallet* receiverWallet = dataManager.getWallet(receiverWalletId);
    if (!receiverWallet) {
        std::cerr << "Receiver wallet not found" << std::endl;
        return false;
    }
    
    std::string transactionId = dataManager.createTransaction(
        senderWalletId, receiverWalletId, amount, description
    );
    
    Transaction* transaction = dataManager.getTransaction(transactionId);
    if (!transaction) {
        std::cerr << "Failed to create transaction" << std::endl;
        return false;
    }
    
    bool success = false;
    
    if (senderWallet->deductPoints(amount)) {
        receiverWallet->addPoints(amount);
        success = true;
    } else {
        std::cerr << "Insufficient balance for transfer" << std::endl;
    }
    
    transaction->setIsSuccessful(success);
    
    if (success) {
        senderWallet->addTransactionToHistory(transactionId);
        receiverWallet->addTransactionToHistory(transactionId);
        
        dataManager.saveWallet(*senderWallet);
        dataManager.saveWallet(*receiverWallet);
        dataManager.saveTransaction(*transaction);
    }
    
    return success;
}

std::vector<Transaction> WalletManager::getTransactionHistory(const std::string& walletId) const {
    return dataManager.getTransactionsByWallet(walletId);
}

Wallet* WalletManager::getCurrentUserWallet() {
    std::string username = authManager.getCurrentUser();
    if (username.empty()) {
        return NULL;
    }
    
    return dataManager.getWalletByOwner(username);
} 
