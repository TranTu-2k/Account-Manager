#include "AuthManager.h"
#include <ctime>
#include <cstdlib>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <cstring>
#include <algorithm>

// Define the base32 character set
const std::string OTP::BASE32_CHARS = "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567";

OTP::OTP() : 
    code(""),
    targetUser(""),
    expirationTime(0),
    purpose(""),
    digits(6),
    timeInterval(30) {
}

OTP::OTP(const std::string& targetUser, const std::string& purpose, int validityInMinutes) :
    secretKey(""),
    targetUser(targetUser),
    purpose(purpose),
    digits(6),
    timeInterval(30) {
    
    unsigned int seed = static_cast<unsigned int>(time(NULL));
    seed ^= (seed << 13);
    seed ^= (seed >> 17);
    seed ^= (seed << 5);
    srand(seed);
    
    int randomCode = 100000 + (rand() % 900000);
    
    std::stringstream ss;
    ss << randomCode;
    code = ss.str();
    
    expirationTime = time(NULL) + validityInMinutes * 60;
    
    std::cout << "OTP Code: " << code << " (Valid for " << validityInMinutes << " minutes)" << std::endl;
    if (purpose.find("profile_update") != std::string::npos) {
        std::cout << "Reason: Profile information update" << std::endl;
    } else if (purpose.find("Transfer points") != std::string::npos) {
        std::cout << "Reason: Points transfer authorization" << std::endl;
    }
}

// Constructor for TOTP implementation
OTP::OTP(const std::string& targetUser, const std::string& purpose, 
        const std::string& secretKey, size_t digits, size_t timeInterval,
        int validityInMinutes) :
    secretKey(secretKey),
    targetUser(targetUser),
    purpose(purpose),
    digits(digits),
    timeInterval(timeInterval) {
    
    expirationTime = time(NULL) + validityInMinutes * 60;
    
    // Generate the initial TOTP code
    generateNewCode();
}

// Generate a new TOTP code based on current time
void OTP::generateNewCode() {
    code = generateTOTP(time(NULL));
}

std::string OTP::getCode() const {
    return code;
}

std::string OTP::getTargetUser() const {
    return targetUser;
}

std::string OTP::getPurpose() const {
    return purpose;
}

std::string OTP::getSecretKey() const {
    return secretKey;
}

bool OTP::isValid() const {
    return time(NULL) < expirationTime;
}

// Generate a random base32 string for use as secret key
std::string OTP::generateRandomBase32(size_t length) const {
    std::string result;
    unsigned int seed = static_cast<unsigned int>(time(NULL));
    seed ^= (seed << 13);
    seed ^= (seed >> 17);
    seed ^= (seed << 5);
    srand(seed);
    
    for (size_t i = 0; i < length; ++i) {
        result += BASE32_CHARS[rand() % BASE32_CHARS.length()];
    }
    
    return result;
}

// Static method to generate a new secret key
std::string OTP::generateSecretKey(size_t length) {
    OTP temp;
    return temp.generateRandomBase32(length);
}

// Convert a string to Base32 encoding
std::string OTP::toBase32(const std::string& input) const {
    std::string result;
    int bits = 0;
    int value = 0;
    
    for (size_t i = 0; i < input.length(); ++i) {
        value = (value << 8) | (input[i] & 0xFF);
        bits += 8;
        
        while (bits >= 5) {
            bits -= 5;
            result += BASE32_CHARS[(value >> bits) & 0x1F];
        }
    }
    
    if (bits > 0) {
        result += BASE32_CHARS[(value << (5 - bits)) & 0x1F];
    }
    
    return result;
}

// Decode a base32 string to bytes
std::vector<char> OTP::decodeBase32(const std::string& base32) const {
    std::vector<char> result;
    int buffer = 0;
    int bitsLeft = 0;
    
    for (size_t i = 0; i < base32.length(); ++i) {
        char c = base32[i];
        if (c == ' ' || c == '\t' || c == '\r' || c == '\n' || c == '-') {
            continue;
        }
        
        c = toupper(c);
        size_t charIndex = BASE32_CHARS.find(c);
        if (charIndex == std::string::npos) {
            continue; // Skip invalid characters
        }
        
        buffer = (buffer << 5) | static_cast<int>(charIndex);
        bitsLeft += 5;
        
        if (bitsLeft >= 8) {
            bitsLeft -= 8;
            result.push_back(static_cast<char>((buffer >> bitsLeft) & 0xFF));
        }
    }
    
    return result;
}

// Convert an integer to bytes
std::vector<char> OTP::toBytes(unsigned long value) const {
    std::vector<char> result(8, 0);
    
    for (int i = 7; i >= 0; --i) {
        result[i] = static_cast<char>(value & 0xFF);
        value >>= 8;
    }
    
    return result;
}

// Simple implementation of SHA1 for HMAC
// NOTE: This is a simplified version for demonstration purposes
// In a real application, use a proper cryptographic library
std::vector<char> OTP::hmacSha1(const std::vector<char>& key, const std::vector<char>& message) const {
    const size_t BLOCK_SIZE = 64;
    const size_t HASH_SIZE = 20;
    
    // Prepare key
    std::vector<char> blockSizedKey;
    if (key.size() > BLOCK_SIZE) {
        // If key is longer than block size, hash it
        // For simplicity, we'll just truncate it in this example
        blockSizedKey.assign(key.begin(), key.begin() + BLOCK_SIZE);
    } else {
        blockSizedKey = key;
        // Pad with zeros to block size
        blockSizedKey.resize(BLOCK_SIZE, 0);
    }
    
    // Prepare inner and outer padding
    std::vector<char> iPad(BLOCK_SIZE, 0x36);
    std::vector<char> oPad(BLOCK_SIZE, 0x5c);
    
    // XOR key with pads
    for (size_t i = 0; i < BLOCK_SIZE; ++i) {
        iPad[i] ^= blockSizedKey[i];
        oPad[i] ^= blockSizedKey[i];
    }
    
    // Concatenate iPad with message
    std::vector<char> innerData = iPad;
    innerData.insert(innerData.end(), message.begin(), message.end());
    
    // Hash innerData (simplified SHA1)
    std::vector<char> innerHash(HASH_SIZE, 0);
    for (size_t i = 0; i < innerData.size(); ++i) {
        innerHash[i % HASH_SIZE] ^= innerData[i];
    }
    
    // Concatenate oPad with innerHash
    std::vector<char> outerData = oPad;
    outerData.insert(outerData.end(), innerHash.begin(), innerHash.end());
    
    // Hash outerData (simplified SHA1)
    std::vector<char> result(HASH_SIZE, 0);
    for (size_t i = 0; i < outerData.size(); ++i) {
        result[i % HASH_SIZE] ^= outerData[i];
    }
    
    return result;
}

// Dynamic truncation as defined in RFC 4226
unsigned long OTP::dynamicTruncate(const std::vector<char>& hmacResult) const {
    int offset = hmacResult[hmacResult.size() - 1] & 0x0F;
    
    unsigned long p = 0;
    p |= (hmacResult[offset] & 0x7F) << 24;
    p |= (hmacResult[offset + 1] & 0xFF) << 16;
    p |= (hmacResult[offset + 2] & 0xFF) << 8;
    p |= (hmacResult[offset + 3] & 0xFF);
    
    return p;
}

// Generate TOTP code for a specific timestamp
std::string OTP::generateTOTP(time_t timestamp) const {
    unsigned long counter = timestamp / timeInterval;
    std::vector<char> message = toBytes(counter);
    std::vector<char> key = decodeBase32(secretKey);
    std::vector<char> hash = hmacSha1(key, message);
    
    unsigned long truncatedHash = dynamicTruncate(hash);
    
    // Calculate 10^digits without using pow
    unsigned long powerOfTen = 1;
    for (size_t i = 0; i < digits; ++i) {
        powerOfTen *= 10;
    }
    
    unsigned long code = truncatedHash % powerOfTen;
    
    std::stringstream ss;
    ss << std::setw(digits) << std::setfill('0') << code;
    return ss.str();
}

// Verify a TOTP code
bool OTP::verify(const std::string& otpCode, size_t validWindow) const {
    time_t currentTime = time(NULL);
    
    // Check within the window (before and after current time)
    for (int i = -static_cast<int>(validWindow); i <= static_cast<int>(validWindow); ++i) {
        time_t checkTime = currentTime + i * timeInterval;
        std::string expectedCode = generateTOTP(checkTime);
        
        if (expectedCode == otpCode) {
            return true;
        }
    }
    
    return false;
}

AuthManager::AuthManager(DataManager& dm) 
    : currentLoggedInUser(""), dataManager(dm) {
}

std::string AuthManager::hashPassword(const std::string& password) const {
    unsigned long hash = 5381;
    for (size_t i = 0; i < password.length(); ++i) {
        hash = ((hash << 5) + hash) + password[i];
    }
    
    std::stringstream ss;
    ss << std::hex << std::setw(16) << std::setfill('0') << hash;
    return ss.str();
}

std::string AuthManager::generateRandomPassword(int length) const {
    const std::string chars = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz!@#$%^&*";
    
    srand(static_cast<unsigned int>(time(NULL) ^ (time(NULL) >> 16)));
    
    std::string password;
    for (int i = 0; i < length; i++) {
        int pos = rand() % chars.size();
        password += chars[pos];
    }
    
    return password;
}

bool AuthManager::registerUser(const std::string& username, 
                             const std::string& password, 
                             const std::string& fullName,
                             const std::string& email,
                             const std::string& phoneNumber,
                             UserRole role,
                             bool isAutoGenerated) {
    std::string actualPassword = password;
    bool passwordWasGenerated = false;
    
    if (password.empty() || isAutoGenerated) {
        actualPassword = generateRandomPassword();
        passwordWasGenerated = true;
    }
    
    std::string hashedPassword = hashPassword(actualPassword);
    
    User newUser(username, hashedPassword, fullName, email, phoneNumber, role);
    
    if (passwordWasGenerated) {
        newUser.setIsAutoGeneratedPassword(true);
        newUser.setIsFirstLogin(true);
    }
    
    bool success = dataManager.saveUser(newUser);
    
    if (success) {
        dataManager.saveData();
        std::cout << "User registered: " << username << std::endl;
        
        if (passwordWasGenerated) {
            std::cout << "Auto-generated password: " << actualPassword << std::endl;
        }
    } else {
        std::cout << "Failed to register user: " << username << std::endl;
    }
    
    return success;
}

bool AuthManager::registerUserByAdmin(const std::string& username, 
                                    const std::string& fullName,
                                    const std::string& email,
                                    const std::string& phoneNumber) {
    if (!isAdmin()) {
        std::cout << "Only administrators can register new users." << std::endl;
        return false;
    }
    
    bool result = registerUser(username, "", fullName, email, phoneNumber, REGULAR, true);
    
    return result;
}

bool AuthManager::login(const std::string& username, const std::string& password) {
    std::string hashedPassword = hashPassword(password);
    
    User* user = dataManager.getUser(username);
    if (!user || user->getPasswordHash() != hashedPassword) {
        return false;
    }
    
    currentLoggedInUser = username;
    
    user->setLastLoginDate(time(NULL));
    dataManager.saveUser(*user);
    dataManager.saveData();
    
    if (user->getIsAutoGeneratedPassword()) {
        std::cout << "WARNING: You are using an auto-generated password. ";
        std::cout << "Please change your password for security reasons." << std::endl;
    }
    
    return true;
}

void AuthManager::logout() {
    currentLoggedInUser = "";
}

bool AuthManager::changePassword(const std::string& username, 
                               const std::string& oldPassword, 
                               const std::string& newPassword) {
    std::string hashedOldPassword = hashPassword(oldPassword);
    
    User* user = dataManager.getUser(username);
    if (!user || user->getPasswordHash() != hashedOldPassword) {
        std::cout << "Old password is incorrect." << std::endl;
        return false;
    }
    
    std::string hashedNewPassword = hashPassword(newPassword);
    user->setPasswordHash(hashedNewPassword);
    
    if (user->getIsAutoGeneratedPassword()) {
        user->setIsAutoGeneratedPassword(false);
        user->setIsFirstLogin(false);
        std::cout << "Password updated successfully. Your account now has a custom password." << std::endl;
    } else {
        std::cout << "Password updated successfully." << std::endl;
    }
    
    bool success = dataManager.saveUser(*user);
    dataManager.saveData(); // Ensure data is written to file immediately
    return success;
}

bool AuthManager::resetPassword(const std::string& username) {
    if (currentLoggedInUser != username && !isAdmin()) {
        return false;
    }
    
    User* user = dataManager.getUser(username);
    if (!user) {
        std::cout << "User not found." << std::endl;
        return false;
    }
    
    std::string tempPassword = generateRandomPassword();
    std::string hashedTempPassword = hashPassword(tempPassword);
    
    user->setPasswordHash(hashedTempPassword);
    user->setIsAutoGeneratedPassword(true);
    user->setIsFirstLogin(true);
    
    bool success = dataManager.saveUser(*user);
    dataManager.saveData(); // Ensure data is written to file immediately
    if (success) {
        std::cout << "Password reset. Temporary password: " << tempPassword << std::endl;
    }
    
    return success;
}

bool AuthManager::generateOTP(const std::string& username, const std::string& purpose) {
    OTP otp(username, purpose);
    
    activeOTPs[username] = otp;
    
    std::cout << "OTP generated for " << username << ": " << otp.getCode() << std::endl;
    
    return true;
}

bool AuthManager::verifyOTP(const std::string& username, const std::string& otpCode) {
    std::map<std::string, OTP>::iterator it = activeOTPs.find(username);
    if (it == activeOTPs.end()) {
        std::cout << "No active OTP found for " << username << std::endl;
        return false;
    }
    
    OTP& otp = it->second;
    
    if (!otp.isValid()) {
        std::cout << "OTP expired" << std::endl;
        activeOTPs.erase(it);
        return false;
    }
    
    if (otp.getCode() != otpCode) {
        std::cout << "Invalid OTP" << std::endl;
        return false;
    }
    
    activeOTPs.erase(it);
    
    return true;
}

std::string AuthManager::getCurrentUser() const {
    return currentLoggedInUser;
}

bool AuthManager::isLoggedIn() const {
    return !currentLoggedInUser.empty();
}

bool AuthManager::isAdmin() const {
    if (!isLoggedIn()) {
        return false;
    }
    
    User* user = dataManager.getUser(currentLoggedInUser);
    if (!user) {
        return false;
    }
    
    return user->isAdmin();
}

// Add the new TOTP methods to AuthManager
bool AuthManager::setupTOTP(const std::string& username) {
    User* user = dataManager.getUser(username);
    if (!user) {
        std::cout << "User not found." << std::endl;
        return false;
    }
    
    // Generate a new secret key
    std::string secretKey = OTP::generateSecretKey(16);
    
    // Store the secret key with the user
    user->setTOTPSecret(secretKey);
    
    bool success = dataManager.saveUser(*user);
    dataManager.saveData();
    
    if (success) {
        std::cout << "TOTP set up for user: " << username << std::endl;
        std::cout << "Secret key: " << secretKey << std::endl;
        std::cout << "Please store this secret key in your authenticator app." << std::endl;
        
        // Create OTP object to generate a sample code
        OTP totp(username, "setup", secretKey);
        std::cout << "Current code: " << totp.getCode() << std::endl;
    }
    
    return success;
}

bool AuthManager::verifyTOTP(const std::string& username, const std::string& totpCode) {
    User* user = dataManager.getUser(username);
    if (!user) {
        std::cout << "User not found." << std::endl;
        return false;
    }
    
    std::string secretKey = user->getTOTPSecret();
    if (secretKey.empty()) {
        std::cout << "TOTP not set up for this user." << std::endl;
        return false;
    }
    
    OTP totp(username, "verification", secretKey);
    bool isValid = totp.verify(totpCode);
    
    if (isValid) {
        std::cout << "TOTP verification successful." << std::endl;
    } else {
        std::cout << "Invalid TOTP code." << std::endl;
    }
    
    return isValid;
}
