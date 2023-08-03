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

    unsigned long long hashFunction(const std::string& key, int i) {
        unsigned long long hash = 0;
        for (int j = 0; j < key.length(); ++j) {
            // hash = (hash + (key[j] - 'a' + 1)) * (i + 1);
            hash = (hash + (int(key[j]) + 1)) * (i + 1);
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
        unsigned long long tmp1 = tmp % (Typefilter.size),
                           tmp2 = tmp % (Typefilter.size * 2);

        if (Typefilter.primaryFilter[tmp1] == 0)
            return 0;
        if (Typefilter.secondaryFilter[tmp2] == 0)
            return 0; //Cross-checking requires both primary and secondary filters to be called and checked
    }
    return 1;
}

bool IsValidUsername(const string& username) {
    return (username.length() > 5 && username.length() < 10 && username.find(' ') == string::npos);
    //npos help check for even extreme long passwords
}

bool IsWeakPassword(const string& password, BloomFilter& WeakpassFilter) {
    return IsExisted(password, WeakpassFilter);
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
    {
        if (IsExisted(password,WeakpassFilter))
            return 4;
        return 3;
    }

    return 0;
}

void RegisterAccount(BloomFilter &UserFilter, BloomFilter &PassFilter, BloomFilter &WeakpassFilter, vector<UserAccount> &accounts)
{
    string username, password, pass_again;
    bool stop=0;
    while (!stop)
    {
        cout << "\t REGISTRATION FUNCTION \n";
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
                cout << "\n*Register successfully! \n";
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

            int choice=1;

            while (choice)
            {
                cout << "1. Back to menu. \n";
                cout << "2. Register again. \n";
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
                    cout << "\n*Invalid Choice! \n";
                cout << endl;
            }
        }
        else 
            cout << "\n*Passsword does not match. Please try again!\n";
    }
}

void MultipleRegistration(BloomFilter &UserFilter, BloomFilter &PassFilter, BloomFilter &WeakpassFilter, vector<UserAccount> &accounts)
{
    ifstream in("SignUp.txt");

    if (in.is_open() == 0)
    {
        cout << "\n*Cannot open file SignUp.txt! Try again! \n";
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

    cout << "*Muptiple register successfully! \n";

    in.close();
    outFail.close();
}

void Login(BloomFilter &UserFilter, BloomFilter &PassFilter, vector<UserAccount> accounts) {
    string username, password;
    bool stop=0;

    while (stop == 0)
    {
        cout << "\t LOG IN FUNCTION \n";
        cout << "Enter Username: ";
        getline(cin,username);
        cout << "Enter Password: ";
        getline(cin,password);

        if (IsExisted(username,UserFilter))
        {
            long long i, numaccount = accounts.size();
            for (i = 0; i < numaccount; i++)
            {
                if (accounts[i].getUsername() == username)
                {
                    if (accounts[i].getPassword() == password)
                    {
                        cout << "\n*Log in successfully! \n";
                        return;
                    }
                    else
                    {
                        cout << "\n*Wrong password! \n";
                        break;
                    }
                }
            }
            if (i == numaccount)
                cout << "\n*User does not exist! \n";
        }
        else
            cout << "\n*User does not exist! \n";

        int choice=1;

        while (choice)
        {
            cout << endl;
            cout << "1. Back to menu. \n";
            cout << "2. Log in again. \n";
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
                cout << "\n*Invalid Choice! \n";
            cout << endl;
        }
    }
}

void ChangePassword(BloomFilter &UserFilter, BloomFilter &PassFilter, BloomFilter &WeakpassFilter, vector<UserAccount>& account) {
    string username, oldPass, newPass, newPass_again;
    bool stop=0;

    while (stop == 0)
    {
        cout << "\t CHANGE PASSWORD FUNCTION \n";
        cout << "Enter Username: ";
        getline(cin,username);
        cout << "Enter old Password: ";
        getline(cin,oldPass);
        cout << "Enter new Password: ";
        getline(cin,newPass);
        cout << "Reconfirm new Password: ";
        getline(cin,newPass_again);
        if (newPass.compare(newPass_again) != 0) {
            cout << "\n*Password does not match! \n";
            return;
        }
        
        if (IsExisted(username,UserFilter) == 0) cout << "\n*User does not exist! \n";
        else {
            long long i, numaccount = account.size();
            for (i=0; i < numaccount; i++) {
                if (account[i].getUsername() == username) {
                    if (account[i].getPassword() == oldPass) {
                        if (IsValidPassword(username,newPass,WeakpassFilter) == 1) {
                            account[i].password = newPass;
                            PassFilter.insert(newPass);
                            cout << "\n*Changed password successfully! \n";
                            stop=1;
                            return;
                        }
                        else {
                            cout << "\n*Invalid password! \n";
                            break;
                        }
                            
                    }
                    else {
                        cout << "\n*Wrong password! \n";
                        break;
                    }
                }
            }
            if (i == numaccount)
                cout << "\n*User does not exist! \n";
        }
        int choice=1;

        while (choice)
        {
            cout << endl;
            cout << "1. Back to menu. \n";
            cout << "2. Change password again. \n";
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
                cout << "\n*Invalid Choice! \n";
            cout << endl;
        }
    }
}

int main() {
    // USE CROSS-CHECKING BLOOM FILTER TO CHECK WEAK PASSWORDS
    vector<UserAccount> accounts;
    BloomFilter UserFilter;
    BloomFilter PassFilter;
    BloomFilter WeakpassFilter;

    ReadWeakPasswordsFromFile(WeakpassFilter, "WeakPass.txt");

    // Prompt the user for actions
    cout << endl;
    int option=-1;
    do {
        cout << "\t ACCOUNT MANAGEMENT PROGRAM \n";
        cout << "*Choose an option:\n";
        cout << "1. Registration\n";
        cout << "2. Multiple registrations\n";
        cout << "3. Log-in\n";
        cout << "4. Change Password\n";
        cout << "0. Exit\n";
        cout << "=> Option: "; 
        cin >> option;
        cin.ignore();

        cout << endl;
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
