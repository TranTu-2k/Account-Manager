#ifndef AUTH_MANAGER_H
#define AUTH_MANAGER_H

#include <string>
#include <vector>
#include <map>
#include "DataManager.h"
#include "User.h"

class OTP {
private:
    std::string code;
    std::string targetUser;
    time_t expirationTime;
    std::string purpose;

public:
    // Default constructor needed for std::map
    OTP();
    
    OTP(const std::string& targetUser, const std::string& purpose, int validityInMinutes = 5);

    std::string getCode() const;
    std::string getTargetUser() const;
    std::string getPurpose() const;
    bool isValid() const;
};

class AuthManager {
private:
    std::string currentLoggedInUser;
    std::map<std::string, OTP> activeOTPs;
    DataManager& dataManager;
    
    std::string hashPassword(const std::string& password) const;
    
    std::string generateRandomPassword(int length = 12) const;

public:
    AuthManager(DataManager& dm);

    bool registerUser(const std::string& username, 
                     const std::string& password, 
                     const std::string& fullName,
                     const std::string& email,
                     const std::string& phoneNumber,
                     UserRole role = REGULAR,
                     bool isAutoGenerated = false);
                     
    bool registerUserByAdmin(const std::string& username, 
                            const std::string& fullName,
                            const std::string& email,
                            const std::string& phoneNumber);

    bool login(const std::string& username, const std::string& password);
    void logout();

    bool changePassword(const std::string& username, 
                       const std::string& oldPassword, 
                       const std::string& newPassword);
                       
    bool resetPassword(const std::string& username);

    bool generateOTP(const std::string& username, const std::string& purpose);
    bool verifyOTP(const std::string& username, const std::string& otpCode);
    
    std::string getCurrentUser() const;
    bool isLoggedIn() const;
    bool isAdmin() const;
};

#endif 
