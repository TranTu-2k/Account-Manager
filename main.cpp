#include <iostream>
#include <string>
#include <limits>
#include "AccountSystem.h"

void clearScreen() {
    #ifdef _WIN32
    system("cls");
    #else
    system("clear");
    #endif
}

void clearInputBuffer() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

void showMainMenu(const AccountSystem& system) {
    std::cout << "\n===== Account Management System =====\n";
    
    if (system.isLoggedIn()) {
        std::cout << "Logged in as: " << system.getCurrentUser();
        if (system.isAdmin()) {
            std::cout << " (Administrator)";
        }
        std::cout << std::endl;
        
        std::cout << "\n----- User Menu -----\n";
        std::cout << "1. View Profile\n";
        std::cout << "2. Change Password\n";
        std::cout << "3. Wallet Operations\n";
        if (system.isAdmin()) {
            std::cout << "\n----- Admin Menu -----\n";
            std::cout << "4. View All Users\n";
            std::cout << "5. Register New User\n";
            std::cout << "6. Reset User Password\n";
        }
        std::cout << "\n0. Logout\n";
    } else {
        std::cout << "\n----- Login Menu -----\n";
        std::cout << "1. Login\n";
        std::cout << "2. Register\n";
        std::cout << "\n0. Exit\n";
    }
    
    std::cout << "\nChoice: ";
}

void showWalletMenu() {
    std::cout << "\n===== Wallet Operations =====\n";
    std::cout << "1. View Balance\n";
    std::cout << "2. View Transaction History\n";
    std::cout << "3. Transfer Points\n";
    std::cout << "\n0. Back to Main Menu\n";
    std::cout << "\nChoice: ";
}

void registerUser(AccountSystem& system) {
    std::string username, password, fullName, email, phoneNumber;
    
    std::cout << "\n===== User Registration =====\n";
    
    std::cout << "Enter Username: ";
    std::cin >> username;
    
    std::cout << "Enter Password: ";
    std::cin >> password;
    
    clearInputBuffer();
    
    std::cout << "Enter Full Name: ";
    std::getline(std::cin, fullName);
    
    std::cout << "Enter Email: ";
    std::getline(std::cin, email);
    
    std::cout << "Enter Phone Number: ";
    std::getline(std::cin, phoneNumber);
    
    if (system.registerUser(username, password, fullName, email, phoneNumber)) {
        std::cout << "\nRegistration successful! You can now login.\n";
    } else {
        std::cout << "\nRegistration failed. Please try again.\n";
    }
}

void registerUserByAdmin(AccountSystem& system) {
    std::string username, fullName, email, phoneNumber;
    
    std::cout << "\n===== Register New User (Admin) =====\n";
    
    std::cout << "Enter Username: ";
    std::cin >> username;
    
    clearInputBuffer();
    
    std::cout << "Enter Full Name: ";
    std::getline(std::cin, fullName);
    
    std::cout << "Enter Email: ";
    std::getline(std::cin, email);
    
    std::cout << "Enter Phone Number: ";
    std::getline(std::cin, phoneNumber);
    
    if (system.registerUserByAdmin(username, fullName, email, phoneNumber)) {
        std::cout << "\nUser registered successfully with auto-generated password.\n";
    } else {
        std::cout << "\nRegistration failed. Please try again.\n";
    }
}

void login(AccountSystem& system) {
    std::string username, password;
    
    std::cout << "\n===== Login =====\n";
    
    std::cout << "Enter Username: ";
    std::cin >> username;
    
    std::cout << "Enter Password: ";
    std::cin >> password;
    
    if (system.login(username, password)) {
        std::cout << "\nLogin successful!\n";
    } else {
        std::cout << "\nLogin failed. Please check your credentials.\n";
    }
}

void changePassword(AccountSystem& system) {
    std::string oldPassword, newPassword;
    
    std::cout << "\n===== Change Password =====\n";
    
    std::cout << "Enter Old Password: ";
    std::cin >> oldPassword;
    
    std::cout << "Enter New Password: ";
    std::cin >> newPassword;
    
    if (system.changePassword(oldPassword, newPassword)) {
        std::cout << "\nPassword changed successfully!\n";
    } else {
        std::cout << "\nPassword change failed. Please check your old password.\n";
    }
}

void viewProfile(AccountSystem& system) {
    std::string username = system.getCurrentUser();
    
    User* user = system.getDataManager().getUser(username);
    if (!user) {
        std::cout << "\nError retrieving user profile.\n";
        return;
    }
    
    std::cout << "\n===== User Profile =====\n";
    std::cout << "Username: " << user->getUsername() << std::endl;
    std::cout << "Full Name: " << user->getFullName() << std::endl;
    std::cout << "Email: " << user->getEmail() << std::endl;
    std::cout << "Phone Number: " << user->getPhoneNumber() << std::endl;
    std::cout << "Role: " << (user->isAdmin() ? "Administrator" : "Regular User") << std::endl;
    
    // Display wallet information
    Wallet* wallet = system.getCurrentUserWallet();
    if (wallet) {
        std::cout << "\n----- Wallet Information -----\n";
        std::cout << "Wallet ID: " << wallet->getWalletId() << std::endl;
        std::cout << "Balance: " << wallet->getBalance() << " points" << std::endl;
    }
}

void viewAllUsers(AccountSystem& system) {
    std::vector<User> users = system.getAllUsers();
    
    std::cout << "\n===== All Users =====\n";
    if (users.empty()) {
        std::cout << "No users found.\n";
        return;
    }
    
    for (std::vector<User>::const_iterator user = users.begin(); user != users.end(); ++user) {
        std::cout << "Username: " << user->getUsername() << "\n";
        std::cout << "Full Name: " << user->getFullName() << "\n";
        std::cout << "Email: " << user->getEmail() << "\n";
        std::cout << "Role: " << (user->isAdmin() ? "Administrator" : "Regular User") << "\n";
        std::cout << "----------------------------\n";
    }
}

void resetUserPassword(AccountSystem& system) {
    std::string username;
    
    std::cout << "\n===== Reset User Password =====\n";
    
    std::cout << "Enter Username: ";
    std::cin >> username;
    
    if (system.resetPassword(username)) {
        std::cout << "\nPassword reset successful.\n";
    } else {
        std::cout << "\nPassword reset failed.\n";
    }
}

void viewWalletBalance(AccountSystem& system) {
    Wallet* wallet = system.getCurrentUserWallet();
    
    if (wallet) {
        std::cout << "\n===== Wallet Balance =====\n";
        std::cout << "Wallet ID: " << wallet->getWalletId() << "\n";
        std::cout << "Balance: " << wallet->getBalance() << " points\n";
    } else {
        std::cout << "\nWallet not found or not logged in.\n";
    }
}

void viewTransactionHistory(AccountSystem& system) {
    Wallet* wallet = system.getCurrentUserWallet();
    
    if (!wallet) {
        std::cout << "\nWallet not found or not logged in.\n";
        return;
    }
    
    std::vector<Transaction> transactions = system.getTransactionHistory(wallet->getWalletId());
    
    std::cout << "\n===== Transaction History =====\n";
    if (transactions.empty()) {
        std::cout << "No transactions found.\n";
        return;
    }
    
    for (std::vector<Transaction>::const_iterator transaction = transactions.begin(); transaction != transactions.end(); ++transaction) {
        std::cout << "Transaction ID: " << transaction->getTransactionId() << "\n";
        std::cout << "Type: " << (transaction->getSenderWalletId() == wallet->getWalletId() ? "Sent" : "Received") << "\n";
        std::cout << "Amount: " << transaction->getAmount() << " points\n";
        std::cout << "Status: " << (transaction->getIsSuccessful() ? "Successful" : "Failed") << "\n";
        std::cout << "Description: " << transaction->getDescription() << "\n";
        std::cout << "----------------------------\n";
    }
}

void transferPoints(AccountSystem& system) {
    std::string receiverWalletId, description, otpCode;
    double amount;
    
    std::cout << "\n===== Transfer Points =====\n";
    
    std::cout << "Enter Receiver Wallet ID: ";
    std::cin >> receiverWalletId;
    
    std::cout << "Enter Amount: ";
    std::cin >> amount;
    
    clearInputBuffer();
    
    std::cout << "Enter Description: ";
    std::getline(std::cin, description);
    
    std::string username = system.getCurrentUser();
    system.generateOTP(username, "Transfer points");
    
    std::cout << "Enter OTP Code (check console for demonstration): ";
    std::cin >> otpCode;
    
    if (system.transferPoints(receiverWalletId, amount, otpCode, description)) {
        std::cout << "\nTransfer successful!\n";
    } else {
        std::cout << "\nTransfer failed. Please check your inputs.\n";
    }
}

void handleWalletOperations(AccountSystem& system) {
    int choice = -1;
    
    while (choice != 0) {
        clearScreen();
        showWalletMenu();
        std::cin >> choice;
        
        switch (choice) {
            case 0:
                break;
            case 1:
                viewWalletBalance(system);
                break;
            case 2:
                viewTransactionHistory(system);
                break;
            case 3:
                transferPoints(system);
                break;
            default:
                std::cout << "\nInvalid choice. Please try again.\n";
                break;
        }
        
        if (choice != 0) {
            std::cout << "\nPress Enter to continue...";
            clearInputBuffer();
            std::cin.get();
        }
    }
}

int main() {
    AccountSystem system;
    int choice = -1;
    
    system.start();
    
    while (true) {
        clearScreen();
        showMainMenu(system);
        std::cin >> choice;
        
        if (!system.isLoggedIn()) {
            switch (choice) {
                case 0:
                    system.shutdown();
                    return 0;
                case 1:
                    login(system);
                    break;
                case 2:
                    registerUser(system);
                    break;
                default:
                    std::cout << "\nInvalid choice. Please try again.\n";
                    break;
            }
        } else {
            switch (choice) {
                case 0:
                    system.logout();
                    break;
                case 1:
                    viewProfile(system);
                    break;
                case 2:
                    changePassword(system);
                    break;
                case 3:
                    handleWalletOperations(system);
                    break;
                case 4:
                    if (system.isAdmin()) {
                        viewAllUsers(system);
                    } else {
                        std::cout << "\nInvalid choice. Please try again.\n";
                    }
                    break;
                case 5:
                    if (system.isAdmin()) {
                        registerUserByAdmin(system);
                    } else {
                        std::cout << "\nInvalid choice. Please try again.\n";
                    }
                    break;
                case 6:
                    if (system.isAdmin()) {
                        resetUserPassword(system);
                    } else {
                        std::cout << "\nInvalid choice. Please try again.\n";
                    }
                    break;
                default:
                    std::cout << "\nInvalid choice. Please try again.\n";
                    break;
            }
        }
        
        if (choice != 0) {
            std::cout << "\nPress Enter to continue...";
            clearInputBuffer();
            std::cin.get();
        }
    }
    
    return 0;
} 
