#include <iostream>
#include <string>
#include <limits>
#include <iomanip> // For setw
#include "AccountSystem.h"
#include "AuthManager.h" // Add this include for OTP class

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
        std::cout << "2. Update Profile\n";
        std::cout << "3. Change Password\n";
        std::cout << "4. Wallet Operations\n";
        std::cout << "5. 2FA Settings\n";
        if (system.isAdmin()) {
            std::cout << "\n----- Admin Menu -----\n";
            std::cout << "6. View All Users\n";
            std::cout << "7. Register New User\n";
            std::cout << "8. Reset User Password\n";
            std::cout << "9. Update User Profile\n";
            std::cout << "10. View All Regular Users\n";
            std::cout << "11. Add Funds to User Wallet\n";
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
    std::cout << "3. View Transaction Summary\n";
    std::cout << "4. View Transaction Details\n";
    std::cout << "5. Transfer Points\n";
    std::cout << "\n0. Back to Main Menu\n";
    std::cout << "\nChoice: ";
}

void registerUser(AccountSystem& system) {
    std::string username, password, fullName, email, phoneNumber;
    
    std::cout << "\n===== User Registration =====\n";
    
    std::cout << "Enter Username: ";
    std::cin >> username;
    
    clearInputBuffer();
    
    std::cout << "Enter Password (leave blank for auto-generated password): ";
    std::getline(std::cin, password);
    
    std::cout << "Enter Full Name: ";
    std::getline(std::cin, fullName);
    
    std::cout << "Enter Email: ";
    std::getline(std::cin, email);
    
    std::cout << "Enter Phone Number: ";
    std::getline(std::cin, phoneNumber);
    
    if (system.registerUser(username, password, fullName, email, phoneNumber)) {
        std::cout << "\nRegistration successful! You can now login.\n";
        if (password.empty()) {
            std::cout << "Please note the auto-generated password shown above.\n";
        }
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
    
    clearInputBuffer();
    
    std::cout << "Enter Password: ";
    std::getline(std::cin, password);
    
    if (system.login(username, password)) {
        // Check if 2FA is enabled for this user
        User* user = system.getDataManager().getUser(username);
        
        if (user && user->isTOTPEnabled()) {
            std::cout << "\nTwo-factor authentication is enabled for this account.\n";
            
            // Get the user's TOTP secret key
            std::string secretKey = user->getTOTPSecret();
            
            // Create an OTP object and generate the current code
            OTP totp(username, "verification", secretKey);
            std::string currentCode = totp.getCode();
            
            // Display the current code to the user
            std::cout << "Current authentication code: " << currentCode << std::endl;
            std::cout << "Enter the 6-digit code from your authenticator app: ";
            
            std::string totpCode;
            std::cin >> totpCode;
            
            if (system.verifyTOTP(username, totpCode)) {
                std::cout << "\n2FA verification successful!\n";
            } else {
                std::cout << "\n2FA verification failed. You will be logged out.\n";
                system.logout();
                std::cout << "Press Enter to continue...";
                clearInputBuffer();
                std::cin.get();
                return;
            }
        }
        
        std::cout << "\nLogin successful!\n";
        
        // Check if user has an auto-generated password
        if (user && user->getIsAutoGeneratedPassword()) {
            std::cout << "\nYou are using an auto-generated password.\n";
            std::cout << "You must change your password before continuing.\n";
            
            // Force password change
            std::string oldPassword = password;
            std::string newPassword;
            
            std::cout << "\n===== Required Password Change =====\n";
            std::cout << "Enter New Password: ";
            std::getline(std::cin, newPassword);
            
            std::string otpCode;
            // Generate OTP for password change
            if (system.generateOTP(username, "password_change")) {
                std::cout << "\nAn OTP has been sent to you to verify password change.\n";
                std::cout << "Please enter the OTP to confirm: ";
                std::cin >> otpCode;
                
                if (system.changePassword(oldPassword, newPassword, otpCode)) {
                    std::cout << "\nPassword changed successfully!\n";
                } else {
                    std::cout << "\nPassword change failed. Invalid OTP or system error.\n";
                    std::cout << "You will be logged out for security reasons.\n";
                    system.logout();
                }
            } else {
                std::cout << "\nFailed to generate OTP. You will be logged out for security reasons.\n";
                system.logout();
            }
        }
    } else {
        std::cout << "\nLogin failed. Please check your credentials.\n";
    }
}

void changePassword(AccountSystem& system) {
    std::string oldPassword, newPassword;
    
    std::cout << "\n===== Change Password =====\n";
    
    std::cout << "Enter Old Password: ";
    clearInputBuffer();
    std::getline(std::cin, oldPassword);
    
    std::cout << "Enter New Password: ";
    std::getline(std::cin, newPassword);
    
    std::string username = system.getCurrentUser();
    
    // Generate OTP for password change
    if (system.generateOTP(username, "password_change")) {
        std::string otpCode;
        std::cout << "\nAn OTP has been sent to you to verify password change.\n";
        std::cout << "Please enter the OTP to confirm: ";
        std::cin >> otpCode;
        
        if (system.changePassword(oldPassword, newPassword, otpCode)) {
            std::cout << "\nPassword changed successfully!\n";
        } else {
            std::cout << "\nPassword change failed. Invalid OTP or incorrect old password.\n";
        }
    } else {
        std::cout << "\nFailed to generate OTP. Password change cancelled.\n";
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

void updateUserProfile(AccountSystem& system) {
    std::string fullName, email, phoneNumber;
    std::string username = system.getCurrentUser();
    
    User* user = system.getDataManager().getUser(username);
    if (!user) {
        std::cout << "\nError retrieving user profile.\n";
        return;
    }
    
    std::cout << "\n===== Update Profile =====\n";
    std::cout << "Username: " << username << " (cannot be changed)\n";
    
    clearInputBuffer();
    
    std::cout << "Current Full Name: " << user->getFullName() << "\n";
    std::cout << "New Full Name (leave blank to keep current): ";
    std::getline(std::cin, fullName);
    if (fullName.empty()) {
        fullName = user->getFullName();
    }
    
    std::cout << "Current Email: " << user->getEmail() << "\n";
    std::cout << "New Email (leave blank to keep current): ";
    std::getline(std::cin, email);
    if (email.empty()) {
        email = user->getEmail();
    }
    
    std::cout << "Current Phone Number: " << user->getPhoneNumber() << "\n";
    std::cout << "New Phone Number (leave blank to keep current): ";
    std::getline(std::cin, phoneNumber);
    if (phoneNumber.empty()) {
        phoneNumber = user->getPhoneNumber();
    }
    
    std::cout << "\nPlease confirm these changes:\n";
    std::cout << "Full Name: " << fullName << "\n";
    std::cout << "Email: " << email << "\n";
    std::cout << "Phone Number: " << phoneNumber << "\n";
    
    char confirm;
    std::cout << "Proceed with changes? (y/n): ";
    std::cin >> confirm;
    
    if (tolower(confirm) != 'y') {
        std::cout << "Profile update cancelled.\n";
        return;
    }
    
    // Generate OTP for profile update
    if (system.generateOTP(username, "profile_update")) {
        std::string otpCode;
        std::cout << "\nAn OTP has been sent to you with the details of the changes.\n";
        std::cout << "Please enter the OTP to confirm: ";
        std::cin >> otpCode;
        
        if (system.updateUserProfile(username, fullName, email, phoneNumber, otpCode)) {
            std::cout << "\nProfile updated successfully!\n";
        } else {
            std::cout << "\nProfile update failed. Invalid OTP or system error.\n";
        }
    } else {
        std::cout << "\nFailed to generate OTP. Profile update cancelled.\n";
    }
}

void updateUserProfileByAdmin(AccountSystem& system) {
    if (!system.isAdmin()) {
        std::cout << "Only administrators can update other user profiles.\n";
        return;
    }
    
    std::string username, fullName, email, phoneNumber;
    
    std::cout << "\n===== Update User Profile (Admin) =====\n";
    
    // List all users for selection
    std::vector<User> users = system.getAllUsers();
    
    if (users.empty()) {
        std::cout << "No users found in the system." << std::endl;
        return;
    }
    
    std::cout << "\n----- User List -----\n";
    for (size_t i = 0; i < users.size(); ++i) {
        std::cout << (i + 1) << ". " << users[i].getUsername() 
                << " (" << users[i].getFullName() << ")" << std::endl;
    }
    
    int userChoice;
    std::cout << "\nSelect a user (1-" << users.size() << "): ";
    std::cin >> userChoice;
    
    if (userChoice < 1 || userChoice > static_cast<int>(users.size())) {
        std::cout << "Invalid selection." << std::endl;
        return;
    }
    
    User selectedUser = users[userChoice - 1];
    username = selectedUser.getUsername();
    
    User* user = system.getDataManager().getUser(username);
    if (!user) {
        std::cout << "\nUser not found.\n";
        return;
    }
    
    clearInputBuffer();
    
    std::cout << "Current Full Name: " << user->getFullName() << "\n";
    std::cout << "New Full Name (leave blank to keep current): ";
    std::getline(std::cin, fullName);
    if (fullName.empty()) {
        fullName = user->getFullName();
    }
    
    std::cout << "Current Email: " << user->getEmail() << "\n";
    std::cout << "New Email (leave blank to keep current): ";
    std::getline(std::cin, email);
    if (email.empty()) {
        email = user->getEmail();
    }
    
    std::cout << "Current Phone Number: " << user->getPhoneNumber() << "\n";
    std::cout << "New Phone Number (leave blank to keep current): ";
    std::getline(std::cin, phoneNumber);
    if (phoneNumber.empty()) {
        phoneNumber = user->getPhoneNumber();
    }
    
    std::cout << "\nPlease confirm these changes to " << username << "'s profile:\n";
    std::cout << "Full Name: " << fullName << "\n";
    std::cout << "Email: " << email << "\n";
    std::cout << "Phone Number: " << phoneNumber << "\n";
    
    char confirm;
    std::cout << "Proceed with changes? (y/n): ";
    std::cin >> confirm;
    
    if (tolower(confirm) != 'y') {
        std::cout << "Profile update cancelled.\n";
        return;
    }
    
    // Generate OTP for profile update by admin
    if (system.generateOTP(username, "admin_profile_update")) {
        std::string otpCode;
        std::cout << "\nAn OTP has been sent to the user (" << username << ") with the details of the changes.\n";
        std::cout << "Please enter the OTP provided by the user to confirm: ";
        std::cin >> otpCode;
        
        if (system.updateUserProfile(username, fullName, email, phoneNumber, otpCode)) {
            std::cout << "\nProfile updated successfully!\n";
        } else {
            std::cout << "\nProfile update failed. Invalid OTP or system error.\n";
        }
    } else {
        std::cout << "\nFailed to generate OTP. Profile update cancelled.\n";
    }
}

void viewAllUsers(AccountSystem& system) {
    std::vector<User> users = system.getAllUsers();
    
    std::cout << "\n===== All Users =====\n";
    if (users.empty()) {
        std::cout << "No users found.\n";
        return;
    }
    
    std::cout << "Total users: " << users.size() << "\n\n";
    
    int userCount = 0;
    for (std::vector<User>::const_iterator user = users.begin(); user != users.end(); ++user) {
        userCount++;
        std::cout << userCount << ". Username: " << user->getUsername() << "\n";
        std::cout << "   Full Name: " << user->getFullName() << "\n";
        std::cout << "   Email: " << user->getEmail() << "\n";
        std::cout << "   Phone: " << user->getPhoneNumber() << "\n";
        std::cout << "   Role: " << (user->isAdmin() ? "Administrator" : "Regular User") << "\n";
        std::cout << "----------------------------\n";
    }
    
    // Option to view detailed information about a specific user
    std::string viewDetailedOption;
    std::cout << "\nDo you want to view detailed information for a specific user? (y/n): ";
    std::cin >> viewDetailedOption;
    
    if (viewDetailedOption == "y" || viewDetailedOption == "Y") {
        int userIndex;
        std::cout << "Enter user number (1-" << users.size() << "): ";
        std::cin >> userIndex;
        
        if (userIndex > 0 && userIndex <= static_cast<int>(users.size())) {
            User selectedUser = users[userIndex - 1];
            std::string username = selectedUser.getUsername();
            
            // Display detailed user information
            std::cout << "\n===== Detailed User Information =====\n";
            std::cout << "Username: " << username << "\n";
            std::cout << "Full Name: " << selectedUser.getFullName() << "\n";
            std::cout << "Email: " << selectedUser.getEmail() << "\n";
            std::cout << "Phone Number: " << selectedUser.getPhoneNumber() << "\n";
            std::cout << "Role: " << (selectedUser.isAdmin() ? "Administrator" : "Regular User") << "\n";
            
            time_t creationDate = selectedUser.getCreationDate();
            time_t lastLoginDate = selectedUser.getLastLoginDate();
            
            std::cout << "Account Creation: " << ctime(&creationDate);
            std::cout << "Last Login: " << ctime(&lastLoginDate);
            
            // Display wallet information if available
            Wallet* wallet = system.getDataManager().getWalletByOwner(username);
            if (wallet) {
                std::cout << "\n----- Wallet Information -----\n";
                std::cout << "Wallet ID: " << wallet->getWalletId() << "\n";
                std::cout << "Balance: " << wallet->getBalance() << " points\n";
            } else {
                std::cout << "\nNo wallet found for this user.\n";
            }
            
            // Admin actions menu
            char adminAction;
            std::cout << "\nAdmin Actions for " << username << ":\n";
            std::cout << "1. Update Profile\n";
            std::cout << "2. Reset Password\n";
            std::cout << "0. Back to User List\n";
            std::cout << "Select action: ";
            std::cin >> adminAction;
            
            switch (adminAction) {
                case '1':
                    updateUserProfileByAdmin(system);
                    break;
                case '2':
                    {
                        std::string resetUsername = username;
                        std::cout << "\n===== Reset User Password =====\n";
                        std::cout << "Username: " << resetUsername << "\n";
                        
                        // Generate OTP for password reset by admin
                        if (system.generateOTP(resetUsername, "admin_password_reset")) {
                            std::string otpCode;
                            std::cout << "\nAn OTP has been sent to the user to verify password reset.\n";
                            std::cout << "Please enter the OTP provided by the user to confirm: ";
                            std::cin >> otpCode;
                            
                            if (system.resetPassword(resetUsername, otpCode)) {
                                std::cout << "\nPassword reset successful.\n";
                            } else {
                                std::cout << "\nPassword reset failed. Invalid OTP or system error.\n";
                            }
                        } else {
                            std::cout << "\nFailed to generate OTP. Password reset cancelled.\n";
                        }
                    }
                    break;
                default:
                    break;
            }
        } else {
            std::cout << "Invalid user number.\n";
        }
    }
}

void viewAllUsersExceptAdmin(AccountSystem& system) {
    std::vector<User> allUsers = system.getAllUsers();
    std::vector<User> users;
    
    // Lọc ra những người dùng không phải admin
    for (std::vector<User>::const_iterator user = allUsers.begin(); user != allUsers.end(); ++user) {
        if (!user->isAdmin()) {
            users.push_back(*user);
        }
    }
    
    std::cout << "\n===== All Regular Users =====\n";
    if (users.empty()) {
        std::cout << "No regular users found.\n";
        return;
    }
    
    std::cout << "Total regular users: " << users.size() << "\n\n";
    
    int userCount = 0;
    for (std::vector<User>::const_iterator user = users.begin(); user != users.end(); ++user) {
        userCount++;
        std::cout << userCount << ". Username: " << user->getUsername() << "\n";
        std::cout << "   Full Name: " << user->getFullName() << "\n";
        std::cout << "   Email: " << user->getEmail() << "\n";
        std::cout << "   Phone: " << user->getPhoneNumber() << "\n";
        std::cout << "   Role: Regular User\n";
        std::cout << "----------------------------\n";
    }
    
    // Option to view detailed information about a specific user
    std::string viewDetailedOption;
    std::cout << "\nDo you want to view detailed information for a specific user? (y/n): ";
    std::cin >> viewDetailedOption;
    
    if (viewDetailedOption == "y" || viewDetailedOption == "Y") {
        int userIndex;
        std::cout << "Enter user number (1-" << users.size() << "): ";
        std::cin >> userIndex;
        
        if (userIndex > 0 && userIndex <= static_cast<int>(users.size())) {
            User selectedUser = users[userIndex - 1];
            std::string username = selectedUser.getUsername();
            
            // Display detailed user information
            std::cout << "\n===== Detailed User Information =====\n";
            std::cout << "Username: " << username << "\n";
            std::cout << "Full Name: " << selectedUser.getFullName() << "\n";
            std::cout << "Email: " << selectedUser.getEmail() << "\n";
            std::cout << "Phone Number: " << selectedUser.getPhoneNumber() << "\n";
            std::cout << "Role: Regular User\n";
            
            time_t creationDate = selectedUser.getCreationDate();
            time_t lastLoginDate = selectedUser.getLastLoginDate();
            
            std::cout << "Account Creation: " << ctime(&creationDate);
            std::cout << "Last Login: " << ctime(&lastLoginDate);
            
            // Display wallet information if available
            Wallet* wallet = system.getDataManager().getWalletByOwner(username);
            if (wallet) {
                std::cout << "\n----- Wallet Information -----\n";
                std::cout << "Wallet ID: " << wallet->getWalletId() << "\n";
                std::cout << "Balance: " << wallet->getBalance() << " points\n";
            } else {
                std::cout << "\nNo wallet found for this user.\n";
            }
            
            // Admin actions menu
            char adminAction;
            std::cout << "\nAdmin Actions for " << username << ":\n";
            std::cout << "1. Update Profile\n";
            std::cout << "2. Reset Password\n";
            std::cout << "0. Back to User List\n";
            std::cout << "Select action: ";
            std::cin >> adminAction;
            
            switch (adminAction) {
                case '1':
                    updateUserProfileByAdmin(system);
                    break;
                case '2':
                    {
                        std::string resetUsername = username;
                        std::cout << "\n===== Reset User Password =====\n";
                        std::cout << "Username: " << resetUsername << "\n";
                        
                        // Generate OTP for password reset by admin
                        if (system.generateOTP(resetUsername, "admin_password_reset")) {
                            std::string otpCode;
                            std::cout << "\nAn OTP has been sent to the user to verify password reset.\n";
                            std::cout << "Please enter the OTP provided by the user to confirm: ";
                            std::cin >> otpCode;
                            
                            if (system.resetPassword(resetUsername, otpCode)) {
                                std::cout << "\nPassword reset successful.\n";
                            } else {
                                std::cout << "\nPassword reset failed. Invalid OTP or system error.\n";
                            }
                        } else {
                            std::cout << "\nFailed to generate OTP. Password reset cancelled.\n";
                        }
                    }
                    break;
                default:
                    break;
            }
        } else {
            std::cout << "Invalid user number.\n";
        }
    }
}

void resetUserPassword(AccountSystem& system) {
    std::string username;
    
    std::cout << "\n===== Reset User Password =====\n";
    
    std::cout << "Enter Username: ";
    std::cin >> username;
    
    // Generate OTP for password reset
    if (system.generateOTP(username, "password_reset")) {
        std::string otpCode;
        std::cout << "\nAn OTP has been sent to the user to verify password reset.\n";
        std::cout << "Please enter the OTP provided by the user to confirm: ";
        std::cin >> otpCode;
        
        if (system.resetPassword(username, otpCode)) {
            std::cout << "\nPassword reset successful.\n";
        } else {
            std::cout << "\nPassword reset failed. Invalid OTP or system error.\n";
        }
    } else {
        std::cout << "\nFailed to generate OTP. Password reset cancelled.\n";
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
        std::cout << "\nYou don't have a wallet yet.\n";
        return;
    }
    
    std::vector<Transaction> transactions = system.getTransactionHistory(wallet->getWalletId());
    
    std::cout << "\n===== Transaction History =====\n";
    std::cout << "Wallet ID: " << wallet->getWalletId() << std::endl;
    std::cout << "Current Balance: " << wallet->getBalance() << " points" << std::endl;
    std::cout << "Total Transactions: " << transactions.size() << std::endl;
    
    if (transactions.empty()) {
        std::cout << "\nNo transactions found.\n";
        return;
    }
    
    std::cout << "\n" << std::string(100, '-') << std::endl;
    std::cout << std::left << std::setw(10) << "Index" 
              << std::setw(15) << "Date" 
              << std::setw(15) << "Amount" 
              << std::setw(20) << "Direction" 
              << std::setw(15) << "Status"
              << "Description" << std::endl;
    std::cout << std::string(100, '-') << std::endl;
    
    for (size_t i = 0; i < transactions.size(); ++i) {
        const Transaction& tx = transactions[i];
        std::string direction;
        double amount = tx.getAmount();
        
        if (tx.getSenderWalletId() == wallet->getWalletId()) {
            direction = "Sent to";
        } else {
            direction = "Received from";
        }
        
        char dateStr[20];
        time_t timestamp = tx.getTimestamp();
        strftime(dateStr, sizeof(dateStr), "%Y-%m-%d", localtime(&timestamp));
        
        std::cout << std::left << std::setw(10) << (i + 1)
                  << std::setw(15) << dateStr
                  << std::setw(15) << amount
                  << std::setw(20) << direction
                  << std::setw(15) << tx.getStatusString()
                  << tx.getDescription() << std::endl;
    }
}

void viewTransactionSummary(AccountSystem& system) {
    Wallet* wallet = system.getCurrentUserWallet();
    if (!wallet) {
        std::cout << "\nYou don't have a wallet yet.\n";
        return;
    }
    
    system.displayTransactionSummary(wallet->getWalletId());
}

void viewTransactionDetails(AccountSystem& system) {
    Wallet* wallet = system.getCurrentUserWallet();
    if (!wallet) {
        std::cout << "\nYou don't have a wallet yet.\n";
        return;
    }
    
    // First show a list of transactions
    std::vector<Transaction> transactions = system.getTransactionHistory(wallet->getWalletId());
    
    if (transactions.empty()) {
        std::cout << "\nNo transactions found.\n";
        return;
    }
    
    std::cout << "\n===== Transactions =====\n";
    for (size_t i = 0; i < transactions.size(); ++i) {
        const Transaction& tx = transactions[i];
        std::cout << (i+1) << ". ";
        
        if (tx.getSenderWalletId() == wallet->getWalletId()) {
            std::cout << "Sent " << tx.getAmount() << " points";
        } else {
            std::cout << "Received " << tx.getAmount() << " points";
        }
        
        std::cout << " (" << tx.getStatusString() << ")" << std::endl;
    }
    
    int choice;
    std::cout << "\nEnter transaction number to view details (0 to cancel): ";
    std::cin >> choice;
    
    if (choice <= 0 || choice > static_cast<int>(transactions.size())) {
        return;
    }
    
    system.displayTransactionDetails(transactions[choice-1].getTransactionId());
}

void transferPoints(AccountSystem& system) {
    std::string receiverWalletId, description, otpCode;
    double amount;
    
    std::cout << "\n===== Transfer Points with OTP Verification =====\n";
    
    // Get all wallets from the system
    std::vector<Wallet> wallets = system.getAllWallets();
    std::string currentUser = system.getCurrentUser();
    
    if (wallets.empty()) {
        std::cout << "No wallets found in the system." << std::endl;
        return;
    }
    
    // Filter out the current user's wallet if they have one
    std::vector<Wallet> otherWallets;
    for (size_t i = 0; i < wallets.size(); ++i) {
        if (wallets[i].getOwnerUsername() != currentUser) {
            otherWallets.push_back(wallets[i]);
        }
    }
    
    if (otherWallets.empty()) {
        std::cout << "No other wallets found to transfer to." << std::endl;
        return;
    }
    
    // Display wallet list for selection
    std::cout << "\n----- Available Wallets -----\n";
    for (size_t i = 0; i < otherWallets.size(); ++i) {
        std::cout << (i + 1) << ". Wallet ID: " << otherWallets[i].getWalletId() 
                << " (Owner: " << otherWallets[i].getOwnerUsername() << ")" << std::endl;
    }
    
    int walletChoice;
    std::cout << "\nSelect a wallet (1-" << otherWallets.size() << "): ";
    std::cin >> walletChoice;
    
    if (walletChoice < 1 || walletChoice > static_cast<int>(otherWallets.size())) {
        std::cout << "Invalid selection." << std::endl;
        return;
    }
    
    receiverWalletId = otherWallets[walletChoice - 1].getWalletId();
    
    std::cout << "Enter Amount: ";
    std::cin >> amount;
    
    clearInputBuffer();
    
    std::cout << "Enter Description: ";
    std::getline(std::cin, description);
    
    // Phase 1: Initiate transfer and generate OTP
    if (!system.initiateTransfer(receiverWalletId, amount, description)) {
        std::cout << "\nCould not initiate transfer. Please check details and try again.\n";
        return;
    }
    
    std::cout << "\nTransfer request initiated successfully!\n";
    std::cout << "We've sent an OTP to verify this transaction.\n";
    std::cout << "For demonstration: Check console output for the OTP code.\n";
    
    // Phase 2: Confirm transfer with OTP verification
    std::cout << "\nEnter OTP Code: ";
    std::cin >> otpCode;
    
    if (system.confirmTransfer(receiverWalletId, amount, otpCode, description)) {
        std::cout << "\nTransfer Successful!\n";
        std::cout << "Amount: " << amount << " points transferred to wallet: " << receiverWalletId << "\n";
    } else {
        std::cout << "\nTransfer Failed!\n";
        std::cout << "The OTP code may be invalid, expired, or there might be insufficient funds.\n";
    }
}

void handleWalletOperations(AccountSystem& system) {
    int choice;
    
    do {
        clearScreen(); // Clear screen before showing wallet menu
        showWalletMenu();
        std::cin >> choice;
        
        clearScreen(); // Clear screen after selection
        
        switch (choice) {
            case 1:
                viewWalletBalance(system);
                break;
            case 2:
                viewTransactionHistory(system);
                break;
            case 3:
                viewTransactionSummary(system);
                break;
            case 4:
                viewTransactionDetails(system);
                break;
            case 5:
                transferPoints(system);
                break;
            case 0:
                break;
            default:
                std::cout << "\nInvalid choice. Please try again.\n";
        }
        
        if (choice != 0) {
            std::cout << "\nPress Enter to continue...";
            clearInputBuffer();
            std::cin.get();
            clearScreen(); // Clear screen after "Press Enter to continue"
        }
    } while (choice != 0);
}

// Add new function to handle adding funds to user wallets (admin only)
void addFundsToUserWallet(AccountSystem& system) {
    std::cout << "\n===== Add Funds to User Wallet (Admin Only) =====\n";
    
    // List all users
    std::vector<User> users = system.getAllUsers();
    
    if (users.empty()) {
        std::cout << "No users found in the system." << std::endl;
        return;
    }
    
    std::cout << "\n----- User List -----\n";
    for (size_t i = 0; i < users.size(); ++i) {
        std::cout << (i + 1) << ". " << users[i].getUsername() 
                << " (" << users[i].getFullName() << ")" << std::endl;
    }
    
    int userChoice;
    std::cout << "\nSelect a user (1-" << users.size() << "): ";
    std::cin >> userChoice;
    
    if (userChoice < 1 || userChoice > static_cast<int>(users.size())) {
        std::cout << "Invalid selection." << std::endl;
        return;
    }
    
    User selectedUser = users[userChoice - 1];
    std::string username = selectedUser.getUsername();
    
    // Get user's wallet
    Wallet* wallet = system.getDataManager().getWalletByOwner(username);
    
    if (!wallet) {
        std::cout << "User does not have a wallet. Creating one..." << std::endl;
        std::string walletId = system.createWallet(username);
        if (walletId.empty()) {
            std::cout << "Failed to create wallet for user." << std::endl;
            return;
        }
        wallet = system.getDataManager().getWallet(walletId);
        if (!wallet) {
            std::cout << "Failed to retrieve newly created wallet." << std::endl;
            return;
        }
    }
    
    // Display current balance
    std::cout << "\nUser: " << username << std::endl;
    std::cout << "Wallet ID: " << wallet->getWalletId() << std::endl;
    std::cout << "Current Balance: " << wallet->getBalance() << " points" << std::endl;
    
    // Get amount to add
    double amount;
    std::cout << "\nEnter amount to add: ";
    std::cin >> amount;
    
    if (amount <= 0) {
        std::cout << "Invalid amount. Amount must be greater than 0." << std::endl;
        return;
    }
    
    std::string adminUsername = system.getCurrentUser();
    
    // Generate OTP for admin to add funds
    if (system.generateOTP(adminUsername, "admin_add_funds")) {
        std::string otpCode;
        std::cout << "\nAn OTP has been generated to verify this admin action.\n";
        std::cout << "Please enter the OTP to confirm adding " << amount << " points to " << username << "'s wallet: ";
        std::cin >> otpCode;
        
        // Add funds to the wallet with OTP verification
        if (system.adminAddFundsToWallet(wallet->getWalletId(), amount, otpCode)) {
            std::cout << "\nFunds added successfully!" << std::endl;
        } else {
            std::cout << "\nFailed to add funds to wallet. Invalid OTP or system error." << std::endl;
        }
    } else {
        std::cout << "\nFailed to generate OTP. Adding funds cancelled." << std::endl;
    }
}

// New function to show 2FA settings menu
void show2FAMenu(const AccountSystem& system) {
    std::cout << "\n===== Two-Factor Authentication (2FA) Settings =====\n";
    
    const DataManager& dataManager = system.getDataManager();
    const User* user = dataManager.getUser(system.getCurrentUser());
    if (user && user->isTOTPEnabled()) {
        std::cout << "2FA Status: Enabled\n";
        std::cout << "1. Disable 2FA\n";
    } else {
        std::cout << "2FA Status: Disabled\n";
        std::cout << "1. Enable 2FA\n";
    }
    std::cout << "2. Test 2FA\n";
    std::cout << "\n0. Back to Main Menu\n";
    std::cout << "\nChoice: ";
}

// Function to handle 2FA settings
void handle2FASettings(AccountSystem& system) {
    int choice;
    std::string username = system.getCurrentUser();
    User* user = system.getDataManager().getUser(username);
    
    while (true) {
        clearScreen();
        show2FAMenu(system);
        std::cin >> choice;
        clearInputBuffer();
        
        switch (choice) {
            case 0: 
                return;
                
            case 1: { // Enable/Disable 2FA
                if (user && user->isTOTPEnabled()) {
                    // Disable 2FA
                    std::cout << "\nAre you sure you want to disable 2FA? (y/n): ";
                    char confirm;
                    std::cin >> confirm;
                    
                    if (tolower(confirm) == 'y') {
                        user->enableTOTP(false);
                        system.getDataManager().saveUser(*user);
                        system.getDataManager().saveData();
                        std::cout << "\n2FA has been disabled for your account.\n";
                    }
                } else {
                    // Enable 2FA
                    if (system.setupTOTP(username)) {
                        std::cout << "\nPlease enter the verification code from your authenticator app: ";
                        std::string code;
                        std::cin >> code;
                        
                        if (system.verifyTOTP(username, code)) {
                            std::cout << "\n2FA has been successfully enabled for your account.\n";
                            std::cout << "Please keep your secret key in a safe place.\n";
                        } else {
                            user->enableTOTP(false);
                            system.getDataManager().saveUser(*user);
                            system.getDataManager().saveData();
                            std::cout << "\nVerification failed. 2FA setup was canceled.\n";
                        }
                    } else {
                        std::cout << "\nFailed to set up 2FA. Please try again later.\n";
                    }
                }
                std::cout << "Press Enter to continue...";
                clearInputBuffer();
                std::cin.get();
                break;
            }
                
            case 2: { // Test 2FA
                if (user && user->isTOTPEnabled()) {
                    // Get the user's TOTP secret key
                    std::string secretKey = user->getTOTPSecret();
                    
                    // Create an OTP object and generate the current code
                    OTP totp(username, "verification", secretKey);
                    std::string currentCode = totp.getCode();
                    
                    // Display the current code to the user
                    std::cout << "\nCurrent authentication code: " << currentCode << std::endl;
                    std::cout << "Enter your current 2FA code to test: ";
                    
                    std::string code;
                    std::cin >> code;
                    
                    if (system.verifyTOTP(username, code)) {
                        std::cout << "\n2FA verification successful!\n";
                    } else {
                        std::cout << "\n2FA verification failed. Please make sure your authenticator app is properly synced.\n";
                    }
                } else {
                    std::cout << "\nYou need to enable 2FA first.\n";
                }
                std::cout << "Press Enter to continue...";
                clearInputBuffer();
                std::cin.get();
                break;
            }
                
            default:
                std::cout << "\nInvalid choice. Press Enter to continue...";
                clearInputBuffer();
                std::cin.get();
        }
    }
}

int main() {
    AccountSystem system;
    system.start();
    
    int choice;
    bool exitProgram = false;
    
    do {
        clearScreen();
        
        // Check if the user has an auto-generated password at the beginning of each menu loop
        if (system.isLoggedIn()) {
            std::string username = system.getCurrentUser();
            User* user = system.getDataManager().getUser(username);
            
            if (user && user->getIsAutoGeneratedPassword()) {
                std::cout << "\nYou are using an auto-generated password.\n";
                std::cout << "You must change your password before continuing.\n";
                
                std::string oldPassword;
                std::string newPassword;
                
                std::cout << "\n===== Required Password Change =====\n";
                std::cout << "Enter Current Password: ";
                clearInputBuffer();
                std::getline(std::cin, oldPassword);
                
                std::cout << "Enter New Password: ";
                std::getline(std::cin, newPassword);
                
                std::string otpCode;
                // Generate OTP for password change
                if (system.generateOTP(username, "password_change")) {
                    std::cout << "\nAn OTP has been sent to you to verify password change.\n";
                    std::cout << "Please enter the OTP to confirm: ";
                    std::cin >> otpCode;
                    
                    if (system.changePassword(oldPassword, newPassword, otpCode)) {
                        std::cout << "\nPassword changed successfully!\n";
                        std::cout << "\nPress Enter to continue...";
                        clearInputBuffer();
                        std::cin.get();
                        clearScreen();
                    } else {
                        std::cout << "\nPassword change failed. Invalid OTP or system error.\n";
                        std::cout << "You will be logged out for security reasons.\n";
                        system.logout();
                        
                        std::cout << "\nPress Enter to continue...";
                        clearInputBuffer();
                        std::cin.get();
                        clearScreen();
                        continue;
                    }
                } else {
                    std::cout << "\nFailed to generate OTP. You will be logged out for security reasons.\n";
                    system.logout();
                    
                    std::cout << "\nPress Enter to continue...";
                    clearInputBuffer();
                    std::cin.get();
                    clearScreen();
                    continue;
                }
            }
        }
        
        showMainMenu(system);
        std::cin >> choice;
        
        clearScreen(); // Clear screen after selection
        
        if (system.isLoggedIn()) {
            switch (choice) {
                case 1:
                    viewProfile(system);
                    break;
                case 2:
                    updateUserProfile(system);
                    break;
                case 3:
                    changePassword(system);
                    break;
                case 4:
                    handleWalletOperations(system);
                    break;
                case 5:
                    handle2FASettings(system);
                    break;
                case 6:
                    if (system.isAdmin()) {
                        viewAllUsers(system);
                    } else {
                        std::cout << "\nInvalid choice. Please try again.\n";
                    }
                    break;
                case 7:
                    if (system.isAdmin()) {
                        registerUserByAdmin(system);
                    } else {
                        std::cout << "\nInvalid choice. Please try again.\n";
                    }
                    break;
                case 8:
                    if (system.isAdmin()) {
                        resetUserPassword(system);
                    } else {
                        std::cout << "\nInvalid choice. Please try again.\n";
                    }
                    break;
                case 9:
                    if (system.isAdmin()) {
                        updateUserProfileByAdmin(system);
                    } else {
                        std::cout << "\nInvalid choice. Please try again.\n";
                    }
                    break;
                case 10:
                    if (system.isAdmin()) {
                        viewAllUsersExceptAdmin(system);
                    } else {
                        std::cout << "\nInvalid choice. Please try again.\n";
                    }
                    break;
                case 11:
                    if (system.isAdmin()) {
                        addFundsToUserWallet(system);
                    } else {
                        std::cout << "\nInvalid choice. Please try again.\n";
                    }
                    break;
                case 0:
                    system.logout();
                    std::cout << "\nLogged out successfully.\n";
                    std::cout << "You can now log in with another account.\n";
                    choice = -1; // Set choice to a value different from 0 to continue the loop
                    break;
                default:
                    std::cout << "\nInvalid choice. Please try again.\n";
            }
        } else {
            // Handle non-logged in menu
            switch (choice) {
                case 0:
                    system.shutdown();
                    std::cout << "\nExiting program. Goodbye!\n";
                    return 0; // Kết thúc chương trình khi chọn thoát từ menu đăng nhập
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
        }
        
        if (choice != 0) {
            std::cout << "\nPress Enter to continue...";
            clearInputBuffer();
            std::cin.get();
            clearScreen(); // Clear screen after "Press Enter to continue"
        }
    } while (true); // Thay đổi điều kiện lặp thành true để chương trình chỉ kết thúc khi chọn exit từ menu đăng nhập
    
    return 0;
} 
