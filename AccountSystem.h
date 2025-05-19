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
    bool changePassword(const std::string& oldPassword, const std::string& newPassword, const std::string& otpCode);
    bool resetPassword(const std::string& username, const std::string& otpCode);

    // Simple OTP methods
    bool generateOTP(const std::string& username, const std::string& purpose);
    bool verifyOTP(const std::string& username, const std::string& otpCode);

    // TOTP (Two-Factor Authentication) methods
    bool setupTOTP(const std::string& username);
    bool verifyTOTP(const std::string& username, const std::string& totpCode);
    bool disableTOTP(const std::string& username);
    bool isTOTPEnabled(const std::string& username);

    std::string createWallet(const std::string& ownerUsername);
    double getWalletBalance(const std::string& walletId);
    Wallet* getCurrentUserWallet();
    std::vector<Transaction> getTransactionHistory(const std::string& walletId);
    
    // Transaction status related methods
    Transaction* getTransaction(const std::string& transactionId);
    std::string getTransactionStatusString(const std::string& transactionId);
    
    // Transaction reporting methods
    void displayTransactionSummary(const std::string& walletId);
    void displayTransactionDetails(const std::string& transactionId);
    
    // Filter transactions by status
    std::vector<Transaction> getTransactionsByStatus(const std::string& walletId, TransactionStatus status);
    
    // Admin function to add funds to any wallet
    bool adminAddFundsToWallet(const std::string& walletId, double amount, const std::string& otpCode);
    
    // Original single-step transfer method
    bool transferPoints(const std::string& receiverWalletId,
                       double amount,
                       const std::string& otpCode,
                       const std::string& description = "");
                       
    // New two-phase OTP transfer methods
    bool initiateTransfer(const std::string& receiverWalletId, 
                         double amount,
                         const std::string& description = "");
                         
    bool confirmTransfer(const std::string& receiverWalletId,
                        double amount,
                        const std::string& otpCode,
                        const std::string& description = "");

    std::vector<User> getAllUsers();
    std::vector<Wallet> getAllWallets();
    bool isAdmin() const;
    bool isLoggedIn() const;
    std::string getCurrentUser() const;
    
    // Getter for DataManager - non-const version
    DataManager& getDataManager() { return dataManager; }
    
    // Getter for DataManager - const version (to fix compiler error)
    const DataManager& getDataManager() const { return dataManager; }
};

#endif 