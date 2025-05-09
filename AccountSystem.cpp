#include "AccountSystem.h"
#include <iostream>

AccountSystem::AccountSystem() 
    : dataManager(), authManager(dataManager), walletManager(dataManager, authManager) {
}

void AccountSystem::start() {
    std::cout << "Starting Account Management System..." << std::endl;
    
    if (!dataManager.loadData()) {
        std::cout << "Warning: Failed to load existing data." << std::endl;
    }
    
    std::cout << "System started successfully." << std::endl;
}

void AccountSystem::shutdown() {
    std::cout << "Shutting down Account Management System..." << std::endl;
    
    if (!dataManager.saveData()) {
        std::cout << "Warning: Failed to save data." << std::endl;
    }
    
    std::cout << "System shutdown complete." << std::endl;
}

bool AccountSystem::registerUser(const std::string& username, 
                               const std::string& password, 
                               const std::string& fullName,
                               const std::string& email,
                               const std::string& phoneNumber) {
    if (dataManager.userExists(username)) {
        std::cout << "Username already exists." << std::endl;
        return false;
    }
    
    bool success = authManager.registerUser(username, password, fullName, email, phoneNumber);
    
    if (success) {
        std::string walletId = walletManager.createWallet(username);
        std::cout << "Wallet created for user: " << username << " with ID: " << walletId << std::endl;
        dataManager.saveData();
    }
    
    return success;
}

bool AccountSystem::registerUserByAdmin(const std::string& username, 
                                      const std::string& fullName,
                                      const std::string& email,
                                      const std::string& phoneNumber) {
    if (!isAdmin()) {
        std::cout << "Only administrators can register new users." << std::endl;
        return false;
    }
    
    if (dataManager.userExists(username)) {
        std::cout << "Username already exists." << std::endl;
        return false;
    }
    
    bool success = authManager.registerUserByAdmin(username, fullName, email, phoneNumber);
    
    if (success) {
        std::string walletId = walletManager.createWallet(username);
        std::cout << "Wallet created for user: " << username << " with ID: " << walletId << std::endl;
    }
    
    return success;
}

bool AccountSystem::updateUserProfile(const std::string& username,
                                    const std::string& fullName,
                                    const std::string& email,
                                    const std::string& phoneNumber,
                                    const std::string& otpCode) {
    User* user = dataManager.getUser(username);
    if (!user) {
        std::cout << "User not found." << std::endl;
        return false;
    }
    
    if (authManager.getCurrentUser() != username && !isAdmin()) {
        std::cout << "Permission denied." << std::endl;
        return false;
    }
    
    if (!authManager.verifyOTP(username, otpCode)) {
        std::cout << "Invalid OTP." << std::endl;
        return false;
    }
    
    user->setFullName(fullName);
    user->setEmail(email);
    user->setPhoneNumber(phoneNumber);
    
    return dataManager.saveUser(*user);
}

bool AccountSystem::login(const std::string& username, const std::string& password) {
    if (!dataManager.userExists(username)) {
        std::cout << "User not found: " << username << std::endl;
        return false;
    }
    
    User* user = dataManager.getUser(username);
    if (!user) {
        std::cout << "Error retrieving user data." << std::endl;
        return false;
    }
    
    bool success = authManager.login(username, password);
    
    if (success) {
        if (user->getIsFirstLogin() || user->getIsAutoGeneratedPassword()) {
            std::cout << "Please change your password before continuing." << std::endl;
        }
        
        user->setLastLoginDate(time(NULL));
        dataManager.saveUser(*user);
        
        std::cout << "Login successful for user: " << username << std::endl;
    } else {
        std::cout << "Login failed: Incorrect password for " << username << std::endl;
    }
    
    return success;
}

void AccountSystem::logout() {
    authManager.logout();
}

bool AccountSystem::changePassword(const std::string& oldPassword, const std::string& newPassword) {
    if (!isLoggedIn()) {
        std::cout << "Not logged in." << std::endl;
        return false;
    }
    
    std::string username = authManager.getCurrentUser();
    
    bool success = authManager.changePassword(username, oldPassword, newPassword);
    
    if (success) {
        User* user = dataManager.getUser(username);
        if (user) {
            user->setIsAutoGeneratedPassword(false);
            user->setIsFirstLogin(false);
            dataManager.saveUser(*user);
        }
    }
    
    return success;
}

bool AccountSystem::resetPassword(const std::string& username) {
    if (authManager.getCurrentUser() != username && !isAdmin()) {
        std::cout << "Permission denied." << std::endl;
        return false;
    }
    
    bool success = authManager.resetPassword(username);
    
    if (success) {
        User* user = dataManager.getUser(username);
        if (user) {
            user->setIsAutoGeneratedPassword(true);
            user->setIsFirstLogin(true);
            dataManager.saveUser(*user);
        }
    }
    
    return success;
}

bool AccountSystem::generateOTP(const std::string& username, const std::string& purpose) {
    if (!dataManager.userExists(username)) {
        std::cout << "User not found." << std::endl;
        return false;
    }
    
    return authManager.generateOTP(username, purpose);
}

bool AccountSystem::verifyOTP(const std::string& username, const std::string& otpCode) {
    return authManager.verifyOTP(username, otpCode);
}

std::string AccountSystem::createWallet(const std::string& ownerUsername) {
    if (!dataManager.userExists(ownerUsername)) {
        std::cout << "User not found." << std::endl;
        return "";
    }
    
    return walletManager.createWallet(ownerUsername);
}

double AccountSystem::getWalletBalance(const std::string& walletId) {
    return walletManager.getBalance(walletId);
}

Wallet* AccountSystem::getCurrentUserWallet() {
    if (!isLoggedIn()) {
        return NULL;
    }
    
    std::string username = getCurrentUser();
    return dataManager.getWalletByOwner(username);
}

std::vector<Transaction> AccountSystem::getTransactionHistory(const std::string& walletId) {
    return walletManager.getTransactionHistory(walletId);
}

bool AccountSystem::transferPoints(const std::string& receiverWalletId,
                                 double amount,
                                 const std::string& otpCode,
                                 const std::string& description) {
    if (!isLoggedIn()) {
        std::cout << "Not logged in." << std::endl;
        return false;
    }
    
    Wallet* senderWallet = walletManager.getCurrentUserWallet();
    if (!senderWallet) {
        std::cout << "Sender wallet not found." << std::endl;
        return false;
    }
    
    return walletManager.transferPoints(
        senderWallet->getWalletId(),
        receiverWalletId,
        amount,
        otpCode,
        description
    );
}

std::vector<User> AccountSystem::getAllUsers() {
    if (!isAdmin()) {
        std::cout << "Only administrators can view all users." << std::endl;
        return std::vector<User>();
    }
    
    return dataManager.getAllUsers();
}

bool AccountSystem::isAdmin() const {
    return authManager.isAdmin();
}

bool AccountSystem::isLoggedIn() const {
    return authManager.isLoggedIn();
}

std::string AccountSystem::getCurrentUser() const {
    return authManager.getCurrentUser();
} 
