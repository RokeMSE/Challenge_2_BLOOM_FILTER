#include <iostream>
#include <string>
#include <fstream>
#include <vector>

using namespace std;

//STRUCTS
struct UserAccount {
    string username;
    string password;

    UserAccount(const string& username, const string& password) : username(username), password(password) {}

    const string& getUsername() const {
        return username;
    }

    const string& getPassword() const {
        return password;
    }
};

struct BloomFilter {
    vector<bool> filter;
    int size;
    int hashCount;

    BloomFilter() : size(1000000), hashCount(3) {
        filter.resize(size);
    }

    void insert(const string& password) {
        for (int i = 0; i < hashCount; ++i) {
            int hash = hashFunction(password, i);
            filter[hash] = true;
        }
    }

    bool isWeakPassword(const string& password) {
        for (int i = 0; i < hashCount; ++i) {
            int hash = hashFunction(password, i);
            if (!filter[hash]) {
                return false;
            }
        }
        return true;
    }

    int hashFunction(const string& password, int i) {
        int hash = 0;
        for (int j = 0; j < password.length(); ++j) {
            hash = (hash + (password[j] - 'a' + 1)) * (i + 1);
        }
        return hash % size;
    }    
};


//ACCOUNT MANAGEMENT FUNCTIONS:
bool isValidUsername(const string& username) {
    return (username.length() > 5 && username.length() < 10 && username.find(' ') == string::npos);
}

bool isValidPassword(const string& username, const string& password, const BloomFilter& BloomFilter) {
    if (password.length() > 10 && password.length() < 20 && password.find(' ') == string::npos &&
        password != username /* && Missing Password must not match the weak password listed in the file <WeakPass.txt>*/) {
        bool hasUpperCase = false, hasLowerCase = false, hasNumber = false, hasSpecialChar = false;
        for (int i = 0; i < password.length(); ++i) {
            if (isupper(password[i])) hasUpperCase = true;
            else if (islower(password[i])) hasLowerCase = true;
            else if (isdigit(password[i])) hasNumber = true;
            else hasSpecialChar = true;
        }
        return hasUpperCase && hasLowerCase && hasNumber && hasSpecialChar;
    }
    return false;
}

bool isUsernameExists(const string& username) {
}

void registerAccount(const string& username, const string& password, BloomFilter& BloomFilter) {
}

void readSignUpFile(BloomFilter& BloomFilter) {
}

bool login(const string& username, const string& password) {
}

void changePassword(const string& username, const string& oldPassword, const string& newPassword, BloomFilter& BloomFilter) {
}

void readWeakPasswordsFromFile(BloomFilter& BloomFilter, const string& filename) {
    ifstream weakPasswordsFile(filename);
    string password;
    while (weakPasswordsFile >> password) {
        BloomFilter.insert(password);
    }
}


int main() {
    BloomFilter BloomFilter;
    readWeakPasswordsFromFile(BloomFilter, "WeakPass.txt");

    // Prompt the user for actions
    int option;
    do {
        cout << "Choose an action:\n";
        cout << "1. Registration\n";
        cout << "2. Multiple registrations\n";
        cout << "3. Log-in\n";
        cout << "4. Change Password\n";
        cout << "0. Exit\n";
        cin >> option;

        switch (option) {
            case 1: {
                string username, password;
                cout << "Enter desired Username: ";
                cin >> username;
                cout << "Enter Password: ";
                cin >> password;
                registerAccount(username, password, BloomFilter);
                break;
            }
            case 2: {
                readSignUpFile(BloomFilter);
                break;
            }
            case 3: {
                string username, password;
                cout << "Enter Username: ";
                cin >> username;
                cout << "Enter Password: ";
                cin >> password;
                login(username, password);
                break;
            }
            case 4: {
                string username, oldPassword, newPassword;
                cout << "Enter Username: ";
                cin >> username;
                cout << "Enter old Password: ";
                cin >> oldPassword;
                cout << "Enter new Password: ";
                cin >> newPassword;
                changePassword(username, oldPassword, newPassword, BloomFilter);
                break;
            }
            case 0:
                cout << "Exiting...\n";
                break;
            default:
                cout << "Invalid option. Please try again.\n";
                break;
        }
    } while (option != 0);

    return 0;
}
