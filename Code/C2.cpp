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

/* struct BloomFilter {
    vector<bool> filter;
    unsigned long long size;
    int hashCount;

    BloomFilter() : size(1e9 + 7), hashCount(3) {
        filter = vector<bool> (size,0);
    }

    void insert(const string& key) {
        for (int i = 0; i < hashCount; ++i) {
            unsigned long long hash = hashFunction(key, i);
            filter[hash] = 1;
        }
    }

    unsigned long long hashFunction(const string& key, int i) {
        unsigned long long hash = 0;
        for (int j = 0; j < key.length(); ++j) {
            hash = (hash + (key[j] - 'a' + 1)) * (i + 1);
        }
        return hash % size;
    }    
}; */

//THIS IS USING CROSS-CHECKING BLOOM FILTERS FOR REDUCED 0 POSITIVES
//Cross-checking Bloom Filters: https://www.naturalspublishing.com/files/published/53oi45i66w1of2.pdf
struct BloomFilter {
    std::vector<bool> primaryFilter;
    std::vector<bool> secondaryFilter;
    unsigned long long size;
    int hashCount;

    BloomFilter() : size(1e9 + 7), hashCount(3) {
        primaryFilter = std::vector<bool>(size, 0);
        secondaryFilter = std::vector<bool>(2 * size, 0);
    }

    void insert(const std::string& key) {
        for (int i = 0; i < hashCount; ++i) {
            unsigned long long primaryHash = hashFunction(key, i) % size;
            unsigned long long secondaryHash = hashFunction(key, i) % (2 * size);
            primaryFilter[primaryHash] = 1;
            secondaryFilter[secondaryHash] = 1;
        }
    }

    bool contains(const std::string& key) {
        for (int i = 0; i < hashCount; ++i) {
            unsigned long long primaryHash = hashFunction(key, i) % size;
            if (!primaryFilter[primaryHash])
                return 0;
        }
        for (int i = 0; i < hashCount; ++i) {
            unsigned long long secondaryHash = hashFunction(key, i) % (2 * size);
            if (!secondaryFilter[secondaryHash])
                return 0;
        }
        return 1;
    }

    unsigned long long hashFunction(const std::string& key, int i) {
        unsigned long long hash = 0;
        for (int j = 0; j < key.length(); ++j) {
            hash = (hash + (key[j] - 'a' + 1)) * (i + 1);
        }
        return hash;
    }
};

//ACCOUNT MANAGEMENT FUNCTIONS:
bool IsExisted(string key, BloomFilter Typefilter)
{
    for (int i=0; i<Typefilter.hashCount; i++)
    {
        unsigned long long tmp = Typefilter.hashFunction(key,i);
        if (Typefilter.primaryFilter[tmp] == 0)
            return 0;
        if (Typefilter.secondaryFilter[tmp] == 0)
            return 0; //Cross-checking requires both primary and secondary filters to be called and checked
    }
    return 1;
}

bool IsValidUsername(const string& username) {
    return (username.length() > 5 && username.length() < 10 && username.find(' ') == string::npos);
    //npos help check for even extreme long passwords
}

bool IsWeakPassword(const string& password, BloomFilter& WeakpassFilter) {
    return WeakpassFilter.contains(password);
}

bool IsValidPassword(const string& username, const string& password, BloomFilter& WeakpassFilter) {
    if (password.length() > 10 && password.length() < 20 && password != username 
        && password.find(' ') == string::npos) {
        bool hasUpperCase = 0, hasLowerCase = 0, hasNumber = 0, hasSpecialChar = 0; 
        
        for (int i = 0; i < password.length(); ++i) 
        {
            if (isupper(password[i])) hasUpperCase = 1;
            else if (islower(password[i])) hasLowerCase = 1;
            else if (isdigit(password[i])) hasNumber = 1;
            else hasSpecialChar = 1;
        }
        return hasUpperCase && hasLowerCase && hasNumber && hasSpecialChar && !IsWeakPassword(password,WeakpassFilter);
    }
    return 0;
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

int CheckValidAccount(string username, string password, BloomFilter &UserFilter, BloomFilter &PassFilter, BloomFilter &WeakpassFilter)
{
    if (!IsValidUsername(username))
        return 1;

    if (IsExisted(username,UserFilter))
        return 2;

    if (!IsValidPassword(username,password,WeakpassFilter))
        return 3;
    
    if (IsExisted(password,WeakpassFilter))
        return 4;

    return 0;
}

void RegisterAccount(BloomFilter &UserFilter, BloomFilter &PassFilter, BloomFilter &WeakpassFilter, vector<UserAccount> &accounts)
{
    string username, password, pass_again;
    bool stop=0;
    while (!stop)
    {
        cout << endl;
        cout << "Enter Username: ";
        getline(cin,username);
        cout << "Enter Password: ";
        getline(cin,password);
        cout << "Reconfirm Password: ";
        getline(cin,pass_again);

        if (password.compare(pass_again) == 0) 
        {
            int check = CheckValidAccount(username,password,UserFilter,PassFilter,WeakpassFilter);
            if (check == 0)
            {
                cout << "\nRegister successfully! \n";
                stop=1;

                UserFilter.insert(username);
                PassFilter.insert(password);
                accounts.push_back(UserAccount(username,password));
                continue;
            }
            else if (check == 1)
                cout << "\n*Invalid username. Please try again! \n";
            else if (check == 2)
                cout << "\n*Username existed. Please try again! \n";
            else if (check == 3)
                cout << "\n*Invalid password. Please try again! \n";
            else if (check == 4)
                cout << "\n*Password is weak. Please try again! \n";
            cout << endl;

            int choice=2;

            while (choice)
            {
                cout << "1. Back to menu. \n";
                cout << "2. Enter again. \n";
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
                    cout << "Invalid Choice! \n";
            }
        }
        else 
            cout << "Passsword does not match. Please try again!\n";
    }
}

void MultipleRegistration(BloomFilter &UserFilter, BloomFilter &PassFilter, BloomFilter &WeakpassFilter, vector<UserAccount> &accounts)
{
    ifstream in("SignUp.txt");

    if (in.is_open() == 0)
    {
        cout << "\n Cannot open file SignUp.txt! Try again! \n";
        return;
    }

    ofstream outFail("Fail.txt");
    
    string username,password;
    while (!in.eof())
    {
        in >> username;
        in >> password;
        
        if (CheckValidAccount(username,password,UserFilter,PassFilter,WeakpassFilter))
            outFail << username << " " << password << endl;
        else
        {
            UserFilter.insert(username);
            PassFilter.insert(password);
            accounts.push_back(UserAccount(username,password));
        }
    }

    cout << "\n Muptiple register successfully! \n";

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

void ChangePassword(BloomFilter &UserFilter, BloomFilter &PassFilter, BloomFilter &WeakpassFilter, vector<UserAccount>& account) {
    int choice;
    string username, oldPass, newPass, newPass_again;
    cout << "Enter Username: ";
    getline(cin,username);
    cout << "Enter old Password: ";
    getline(cin,oldPass);
    cout << "Enter new Password: ";
    getline(cin,newPass);
    cout << "Reconfirm Password: ";
    getline(cin,newPass_again);
    if (newPass.compare(newPass_again) != 0) {
        cout << "Password does not match! \n";
        return;
    }
    
    if (IsExisted(username,UserFilter) == 0) cout << "User does not exist! \n";
    else {
        for (int i=0; i < account.size(); i++) {
            if (account[i].getUsername() == username) {
                if (account[i].getPassword() == oldPass) {
                    if (IsValidPassword(username,newPass,WeakpassFilter) == 0) {
                        account[i].password = newPass;
                        PassFilter.insert(newPass);
                        cout << "Changed password successfully! \n";
                    }
                    else
                        cout << "Invalid password! \n";
                        return;
                }
                else {
                    cout << "Wrong password! \n";
                    return;
                }
            }
        }
    }
    return;
}

int main() {
    // USE CROSS-CHECKING BLOOM FILTER TO CHECK WEAK PASSWORDS
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
                ChangePassword(UserFilter,PassFilter,WeakpassFilter,accounts);
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
