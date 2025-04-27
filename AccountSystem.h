#ifndef ACCOUNT_SYSTEM_H
#define ACCOUNT_SYSTEM_H

#include "User.h"
#include "Wallet.h"
#include "AuthManager.h"
#include "DataManager.h"
#include "WalletManager.h"

class AccountSystem {
private:
    AuthManager authManager;
    DataManager dataManager;
    WalletManager walletManager;

public:
    AccountSystem();

    void start();
    void shutdown();

    bool registerUser(const std::string& username, 
                     const std::string& password, 
                     const std::string& fullName,
                     const std::string& email,
                     const std::string& phoneNumber);
                     
    bool registerUserByAdmin(const std::string& username, 
                            const std::string& fullName,
                            const std::string& email,
                            const std::string& phoneNumber);
                            
    bool updateUserProfile(const std::string& username,
                          const std::string& fullName,
                          const std::string& email,
                          const std::string& phoneNumber,
                          const std::string& otpCode);
                          
    bool login(const std::string& username, const std::string& password);
    void logout();
    bool changePassword(const std::string& oldPassword, const std::string& newPassword);
    bool resetPassword(const std::string& username);

    bool generateOTP(const std::string& username, const std::string& purpose);
    bool verifyOTP(const std::string& username, const std::string& otpCode);

    std::string createWallet(const std::string& ownerUsername);
    double getWalletBalance(const std::string& walletId);
    Wallet* getCurrentUserWallet();
    std::vector<Transaction> getTransactionHistory(const std::string& walletId);
    bool transferPoints(const std::string& receiverWalletId,
                       double amount,
                       const std::string& otpCode,
                       const std::string& description = "");

    std::vector<User> getAllUsers();
    bool isAdmin() const;
    bool isLoggedIn() const;
    std::string getCurrentUser() const;
    
    // Getter for DataManager
    DataManager& getDataManager() { return dataManager; }
};

#endif 