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

/*     const string& getUsername() const {
        return username;
    }

    const string& getPassword() const {
        return password;
    } */
};

struct BloomFilter {
    vector<bool> filter;
    int size;
    int hashCount;

    BloomFilter() : size(1e9 + 7), hashCount(3) {
        // filter.resize(size);
        filter = vector<bool> (size,0);
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
    //npos help check for even extreme long passwords
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

bool isAccountExists(const string& username, const string& password) {
    bool user = 1, pass = 1;
/* Write code here
..................  */

    return user && pass;
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
    string weak_pass = "WeakPass.txt"; //Read only
    readWeakPasswordsFromFile(BloomFilter, weak_pass);
    string sign_up = "SignUp.txt"; //Write and read
    string fail = "Fail.txt"; //Write only

    // Prompt the user for actions
    int option;
    do {
        cout << "Choose an action:\n";
        cout << "1. Registration\n";
        cout << "2. Multiple registrations\n";
        cout << "3. Log-in\n";
        cout << "4. Change Password\n";
        cout << "0. Exit\n";
        cout << "=> Option: "; 
        cin >> option;

        switch (option) {
            case 1: {
                AGAIN1:
                string username, pass, pass_again;
                cout << "Enter Username: ";
                cin >> username;
                cout << "Enter Password: ";
                cin >> pass;
                cout << "Reconfirm Password: ";
                cin >> pass_again;
                if (pass.compare(pass_again) == 0) registerAccount(username, pass_again, BloomFilter);
                else {
                    cout << "Passsword does not match. Please enter again!\n.";
                    goto AGAIN1;
                }
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
                string username, oldPassword, newPass, newPass_again;
                cout << "Enter Username: ";
                cin >> username;
                cout << "Enter old Password: ";
                cin >> oldPassword;
                isAccountExists(username, oldPassword);
                AGAIN4:
                cout << "Enter new Password: ";
                cin >> newPass;
                cout << "Reconfirm Password: ";
                cin >> newPass_again;
                if (newPass.compare(newPass_again) == 0) changePassword(username, oldPassword, newPass_again, BloomFilter);
                else {
                    cout << "Passsword does not match. Please enter again!\n.";
                    goto AGAIN4;
                }
                break;
            }
            case 0:
                cout << "-----------------LOGGING OFF----------------\n";
                break;
            default:
                cout << "Invalid option. Please try again.\n";
                break;
        }
    } while (option != 0);

    return 0;
}
