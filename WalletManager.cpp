#include "WalletManager.h"
#include <iostream>
#include <stdexcept>
#include <sstream>

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

bool WalletManager::addFundsToWallet(const std::string& walletId, double amount) {
    // Validate amount
    if (amount <= 0) {
        std::cerr << "Invalid amount. Amount must be greater than 0." << std::endl;
        return false;
    }
    
    // Get the wallet
    Wallet* wallet = dataManager.getWallet(walletId);
    if (!wallet) {
        std::cerr << "Wallet not found" << std::endl;
        return false;
    }
    
    // Add the funds to the wallet
    wallet->addPoints(amount);
    
    // Create a deposit transaction record (system wallet to user wallet)
    std::string systemWalletId = "SYSTEM"; // Special ID for system transactions
    std::string transactionId = dataManager.createTransaction(
        systemWalletId, walletId, amount, "Admin deposit"
    );
    
    Transaction* transaction = dataManager.getTransaction(transactionId);
    if (transaction) {
        // Mark as completed immediately
        transaction->setStatus(COMPLETED);
        dataManager.saveTransaction(*transaction);
        
        // Add to wallet transaction history
        wallet->addTransactionToHistory(transactionId);
        
        // Save wallet data
        dataManager.saveWallet(*wallet);
        return true;
    }
    
    return false;
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
    
    // Set initial transaction status
    transaction->setStatus(PENDING);
    
    bool success = false;
    
    if (senderWallet->deductPoints(amount)) {
        receiverWallet->addPoints(amount);
        transaction->setStatus(COMPLETED);
        success = true;
    } else {
        std::cerr << "Insufficient balance for transfer" << std::endl;
        transaction->setStatus(FAILED);
    }
    
    // Save transaction status immediately
    dataManager.saveTransaction(*transaction);
    
    if (success) {
        senderWallet->addTransactionToHistory(transactionId);
        receiverWallet->addTransactionToHistory(transactionId);
        
        dataManager.saveWallet(*senderWallet);
        dataManager.saveWallet(*receiverWallet);
    }
    
    return success;
}

bool WalletManager::initiateTransfer(const std::string& senderWalletId, 
                                  const std::string& receiverWalletId, 
                                  double amount,
                                  const std::string& description) {
    // Step 1: Find, open wallet A (sender)
    Wallet* senderWallet = dataManager.getWallet(senderWalletId);
    if (!senderWallet) {
        std::cerr << "Sender wallet not found" << std::endl;
        return false;
    }
    
    // Step 2: Find, open wallet B (receiver)
    Wallet* receiverWallet = dataManager.getWallet(receiverWalletId);
    if (!receiverWallet) {
        std::cerr << "Receiver wallet not found" << std::endl;
        return false;
    }
    
    // Check if sender has enough balance (preliminary check only)
    if (senderWallet->getBalance() < amount) {
        std::cerr << "Insufficient balance for transfer. Cannot proceed." << std::endl;
        return false;
    }
    
    // Generate transfer-specific OTP for the wallet owner
    std::string ownerUsername = senderWallet->getOwnerUsername();
    std::string otpPurpose = "Transfer points: " + senderWalletId + " to " + receiverWalletId + 
                             ", Amount: " + static_cast<std::ostringstream*>(&(std::ostringstream() << amount))->str();
    
    return authManager.generateOTP(ownerUsername, otpPurpose);
}

bool WalletManager::confirmTransfer(const std::string& senderWalletId, 
                                  const std::string& receiverWalletId,
                                  double amount, 
                                  const std::string& otpCode,
                                  const std::string& description) {
    // Step 1: Find, open wallet A (sender)
    Wallet* senderWallet = dataManager.getWallet(senderWalletId);
    if (!senderWallet) {
        std::cerr << "Sender wallet not found" << std::endl;
        return false;
    }
    
    // Step 2: Find, open wallet B (receiver)
    Wallet* receiverWallet = dataManager.getWallet(receiverWalletId);
    if (!receiverWallet) {
        std::cerr << "Receiver wallet not found" << std::endl;
        return false;
    }
    
    // Verify OTP
    std::string ownerUsername = senderWallet->getOwnerUsername();
    if (!authManager.verifyOTP(ownerUsername, otpCode)) {
        std::cerr << "Invalid OTP for transfer" << std::endl;
        return false;
    }
    
    // Create transaction record
    std::string transactionId = dataManager.createTransaction(
        senderWalletId, receiverWalletId, amount, description
    );
    
    Transaction* transaction = dataManager.getTransaction(transactionId);
    if (!transaction) {
        std::cerr << "Failed to create transaction" << std::endl;
        return false;
    }
    
    // Set initial transaction status
    transaction->setStatus(PENDING);
    
    // Begin atomic transaction process
    bool success = false;
    
    try {
        // Step 3.1: Check if senderWallet has enough balance
        if (senderWallet->getBalance() < amount) {
            std::cerr << "Insufficient balance for transfer. Cannot proceed." << std::endl;
            transaction->setStatus(FAILED);
            throw std::runtime_error("Low balance");
        }
        
        // Deduct points from sender
        if (!senderWallet->deductPoints(amount)) {
            transaction->setStatus(FAILED);
            throw std::runtime_error("Failed to deduct points");
        }
        
        // Step 3.2: Add points to receiver
        receiverWallet->addPoints(amount);
        
        // Transaction succeeded
        transaction->setStatus(COMPLETED);
        success = true;
    } 
    catch (const std::exception& e) {
        std::cerr << "Transaction failed: " << e.what() << std::endl;
        // The transaction failed, so we don't need to rollback anything
        // as deductPoints has built-in validation
        if (transaction->getStatus() != FAILED) {
            transaction->setStatus(FAILED);
        }
    }
    
    // Update transaction status
    dataManager.saveTransaction(*transaction);
    
    if (success) {
        // Add transaction to history and save data
        senderWallet->addTransactionToHistory(transactionId);
        receiverWallet->addTransactionToHistory(transactionId);
        
        dataManager.saveWallet(*senderWallet);
        dataManager.saveWallet(*receiverWallet);
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
