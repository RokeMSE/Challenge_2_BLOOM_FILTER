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
    unsigned long long size;
    int hashCount;

    BloomFilter() : size(1e9 + 7), hashCount(3) {
        filter = vector<bool> (size,0);
    }

    void insert(const string& key) {
        for (int i = 0; i < hashCount; ++i) {
            unsigned long long hash = hashFunction(key, i);
            filter[hash] = true;
        }
    }

    unsigned long long hashFunction(const string& key, int i) {
        unsigned long long hash = 0;
        for (int j = 0; j < key.length(); ++j) {
            hash = (hash + (key[j] - 'a' + 1)) * (i + 1);
        }
        return hash % size;
    }    
};

//ACCOUNT MANAGEMENT FUNCTIONS:

bool isExisted(string key, BloomFilter Typefilter)
{
    for (int i=0; i<Typefilter.hashCount; i++)
    {
        unsigned long long tmp = Typefilter.hashFunction(key,i);
        if (Typefilter.filter[tmp] == 0)
            return 0;
    }
    return 1;
}

bool isValidUsername(const string& username) {
    return (username.length() > 5 && username.length() < 10 && username.find(' ') == string::npos);
    //npos help check for even extreme long passwords
}

bool isValidPassword(const string& username, const string& password) {
    if (password.length() > 10 && password.length() < 20 && password.find(' ') == string::npos && password != username) 
    {
        bool hasUpperCase = false, hasLowerCase = false, hasNumber = false, hasSpecialChar = false; 
        

        for (int i = 0; i < password.length(); ++i) 
        {
            if (isupper(password[i])) hasUpperCase = true;
            else if (islower(password[i])) hasLowerCase = true;
            else if (isdigit(password[i])) hasNumber = true;
            else hasSpecialChar = true;
        }

        return hasUpperCase && hasLowerCase && hasNumber && hasSpecialChar;
    }
    return false;
}

void ReadWeakPasswordsFromFile(BloomFilter& WeakpassFilter, const string& filename) {
    ifstream weakPasswordsFile(filename);

    string password;
    while (weakPasswordsFile >> password) {
        WeakpassFilter.insert(password);
    }

    weakPasswordsFile.close();
}

void ReadSignUpFile(vector<UserAccount> &accounts, string filename)
{
    ifstream in(filename);

    string user, password;
    while (!in.eof())
    {
        in >> user;
        in >> password;

        accounts.push_back(UserAccount(user,password));
    }

    in.close();
}

int CheckValidAccount(string username, string password, BloomFilter &UserFilter, BloomFilter &PassFilter, BloomFilter &WeakpassFilter, vector<UserAccount> &accounts)
{
    if (!isValidUsername(username))
        return 1;

    if (isExisted(username,UserFilter))
        return 2;

    if (!isValidPassword(username,password))
        return 3;
    
    if (isExisted(password,WeakpassFilter))
        return 4;

    UserFilter.insert(username);
    PassFilter.insert(password);
    accounts.push_back(UserAccount(username,password));
    return 0;
}

void RegisterAccount(BloomFilter &UserFilter, BloomFilter &PassFilter, BloomFilter &WeakpassFilter, vector<UserAccount> &accounts)
{
    string username, password, pass_again;
    bool stop=0;
    while (!stop)
    {
        cout << "Enter Username: ";
        getline(cin,username);
        cout << "Enter Password: ";
        getline(cin,password);
        cout << "Reconfirm Password: ";
        getline(cin,pass_again);

        if (password.compare(pass_again) == 0) 
        {
            int check = CheckValidAccount(username,password,UserFilter,PassFilter,WeakpassFilter, accounts);
            if (check == 0)
            {
                cout << "Register successfully! \n";
                stop=1;
                continue;
            }
            else if (check == 1)
                cout << "Invalid username. Please try again! \n";
            else if (check == 2)
                cout << "Username existed. Please try again! \n";
            else if (check == 3)
                cout << "Invalid password. Please try again! \n";
            else if (check == 4)
                cout << "Password is weak. Please try again! \n";
            cout << endl;

            int choice=2;

            while (choice)
            {
                cout << "1. Back to menu." << endl;
                cout << "2. Enter again." << endl;
                cout << "=> Choice: ";
                cin >> choice;
                cin.ignore();
                if (choice == 1)
                {
                    choice=0;
                    stop=1;
                }
                else if (choice == 2)
                    choice=0;
                else
                    cout << "Invalid Choice!" << endl;
            }
        }
        else 
            cout << "Passsword does not match. Please try again!\n";
    }
}

void MultipleRegistration(BloomFilter &UserFilter, BloomFilter &PassFilter, BloomFilter &WeakpassFilter, vector<UserAccount> &accounts)
{
    ifstream in("SignUp.txt");
    ofstream outFail("Fail.txt");
    
    string username,password;
    while (!in.eof())
    {
        in >> username;
        in >> password;
        
        if (CheckValidAccount(username,password,UserFilter,PassFilter,WeakpassFilter,accounts))
            outFail << username << " " << password << endl;
    }

    in.close();
    outFail.close();
}

bool Login(BloomFilter &UserFilter, BloomFilter &PassFilter, vector<UserAccount> accounts) {
    string username, password;

    cout << "Enter Username: ";
    getline(cin,username);
    cout << "Enter Password: ";
    getline(cin,password);

    /*
        Check account thư mũ đen
    */

}

void changePassword(const string& username, const string& oldPassword, const string& newPassword, 
                    BloomFilter& BloomFilter, vector<UserAccount>& account) {
    if (!isValidUsername(username)) {
        cout << "Account does not exist.\n";
        return;
    }

    for (int i = 0; i < account.size(); ++i) {
        if (account[i].getUsername() == username && account[i].getPassword() == oldPassword) {
            if (isValidPassword(username, newPassword)) {
                account[i] = UserAccount(username, newPassword);
                BloomFilter.insert(newPassword);
                cout << "Password changed successfully!\n";
            } else {
                cout << "Invalid new password. Please try again.\n";
            }
            return;
        }
    }
    cout << "Invalid username or old password. Please try again.\n";
}

int main() {
    // USE BLOOM FILTER TO CHECK WEAK PASSWORDS
    vector<UserAccount> accounts;
    BloomFilter UserFilter;
    BloomFilter PassFilter;
    BloomFilter WeakpassFilter;

    ReadWeakPasswordsFromFile(WeakpassFilter, "WeakPass.txt");

    // Prompt the user for actions
    int option=-1;
    do {
        cout << "Choose an option:\n";
        cout << "1. Registration\n";
        cout << "2. Multiple registrations\n";
        cout << "3. Log-in\n";
        cout << "4. Change Password\n";
        cout << "0. Exit\n";
        cout << "=> Option: "; 
        cin >> option;
        cin.ignore();

        switch (option) {
            case 1: {
                RegisterAccount(UserFilter,PassFilter,WeakpassFilter,accounts);
                break;
            }
            case 2: {
                MultipleRegistration(UserFilter,PassFilter,WeakpassFilter,accounts);
                break;
            }
            case 3: {
                Login(UserFilter,PassFilter,accounts);
                break;
            }
            case 4: {
                string username, oldPassword, newPass, newPass_again;
                cout << "Enter Username: ";
                cin >> username;
                cout << "Enter old Password: ";
                cin >> oldPassword;
                // isAccountExists(username, oldPassword);
                AGAIN4:
                cout << "Enter new Password: ";
                cin >> newPass;
                cout << "Reconfirm Password: ";
                cin >> newPass_again;
                if (newPass.compare(newPass_again) == 0) changePassword(username, oldPassword, newPass_again, WeakpassFilter, accounts);
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
        cout << endl;

    } while (option != 0);


    accounts.clear();
    return 0;
}
