#ifndef WALLET_MANAGER_H
#define WALLET_MANAGER_H

#include <string>
#include "Wallet.h"
#include "AuthManager.h"
#include "DataManager.h"

class WalletManager {
private:
    DataManager& dataManager;
    AuthManager& authManager;

public:
    WalletManager(DataManager& dataManager, AuthManager& authManager);

    std::string createWallet(const std::string& ownerUsername);
    double getBalance(const std::string& walletId);
    
    // Add new method for adding funds to wallet
    bool addFundsToWallet(const std::string& walletId, double amount);
    
    bool transferPoints(const std::string& senderWalletId, 
                       const std::string& receiverWalletId, 
                       double amount,
                       const std::string& otpCode,
                       const std::string& description = "");
    
    // New two-phase OTP transfer workflow
    bool initiateTransfer(const std::string& senderWalletId, 
                        const std::string& receiverWalletId, 
                        double amount,
                        const std::string& description = "");
                        
    bool confirmTransfer(const std::string& senderWalletId, 
                       const std::string& receiverWalletId,
                       double amount, 
                       const std::string& otpCode,
                       const std::string& description = "");
    
    std::vector<Transaction> getTransactionHistory(const std::string& walletId) const;
    
    Wallet* getCurrentUserWallet();
};

#endif 