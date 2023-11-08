// Project Identifier: 292F24D17A4455C1B5133EDD8C7CEAA0C9570A98
//EECS 281, Project 3
 
#include <getopt.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector> 
#include <queue>
#include <unordered_map>
#include <unordered_set>

using namespace std;
class bank{
    private:
        class User{
            private:
                uint64_t regTimestamp;
                string id;
                uint64_t pin;
                uint64_t startBalance;
                bool activeSession;
            public:
                //constructor
                User(const uint64_t &ts,const string &i, const uint64_t &p, const uint64_t &sb)
                : regTimestamp(ts), id(i), pin(p), startBalance(sb), activeSession(false){}

                void setTimestamp(const uint64_t &ts){
                    regTimestamp = ts;
                }
                uint64_t getTimestamp(){
                    return regTimestamp;
                }
                void setID(const string &i){
                    id = i;
                }
                string getID(){
                    return id;
                }
                void setStart(const uint64_t &n){
                    startBalance = n;
                }
                uint64_t getStart(){
                    return startBalance;
                }
                void setPin(const uint64_t &p){
                    pin = p;
                }
                uint64_t getPin(){
                    return pin;
                }
                void makeActive(){
                    activeSession = true;
                }
                void deActive(){
                    activeSession = false;
                }
             bool getActive(){
                    return activeSession;
                }
        };
        class Transaction{
            private:
                uint64_t placementTime;
                uint64_t senderIP;
                string sender;
                string receiver;
                uint64_t amount;
                uint64_t executionDate;
            public:
            //constructor
                Transaction(const uint64_t &pt,const uint64_t &si, const string &s, const string &r, const uint64_t &a, const uint64_t &ed)
                : placementTime(pt), senderIP(si), sender(s), receiver(r), amount(a), executionDate(ed){}

                void setPlacementTime(const uint64_t &i){
                    placementTime = i;
                }
                uint64_t getPlacementTime(){
                    return placementTime;
                }
                void setIP(const uint64_t &n){
                    senderIP = n;
                }
                uint64_t getIP(){
                    return senderIP;
                }
                void setSender(const string &s){
                    sender = s;
                }
                string getSender(){
                    return sender;
                }
                void setReceiver(const string &r){
                    receiver = r;
                }
                string getReceiver(){
                    return receiver;
                }
                void setAmount(const uint64_t &n){
                    amount = n;
                }
                uint64_t getAmount(){
                    return amount;
                }
                void setExecutionDate(const uint64_t &n){
                    executionDate = n;
                }
                uint64_t getExecutionDate(){
                    return executionDate;
                }
        };

        class executionComparator{
            public:
                bool operator()(Transaction *a, Transaction *b){
                    if(a->getExecutionDate() != b->getExecutionDate()){
                        return a->getExecutionDate() > b->getExecutionDate();
                    }
                    else{
                        return a->getPlacementTime() > b->getPlacementTime();
                    }
                }
        };


        bool file = false;
        string filename;
        bool verbose = false;
        unordered_map<string, User*> existingUsers;
        unordered_map<string, unordered_set<uint64_t>> userIPs;
        priority_queue<Transaction*, vector<Transaction*>, executionComparator> pendingTs;
        
        
        void throwFileError(){
            cerr << "file name not provided\n";
            exit(1);
        }
        uint64_t summarizeTS(vector<uint64_t> &toSum){
            uint64_t sum = 0;
            while(!toSum.empty()){
                sum += (toSum.back() * 100ULL);
                toSum.pop_back();
            } 
            return sum;
        }
        uint64_t convertTimestamp(string ts){
            stringstream sin(ts);
            vector<uint64_t> nums;
            string curr;
            while(getline(sin, curr, ':')){
                nums.push_back((uint64_t)(stoi(curr)));
            }
            getline(sin, curr);
            nums.push_back(((uint64_t)stoi(curr)));
            return summarizeTS(nums);
        }


        bool checkFraudulent(Transaction &t){
            if(userIPs[t.getSender()].find(t.getIP()) == userIPs[t.getSender()].end()){
                if(verbose){
                    cout << "Fraudulent transaction detected, aborting request.\n";
                }
                return false;
            }
            return true;
        }
        bool validateTransaction(Transaction &t){
            //wrote witih continueCheck but could definitely just take out and use
            //return falses instead
            bool continueCheck = true;
            const uint64_t execDate = t.getExecutionDate();
            const uint64_t placementT = t.getPlacementTime();
            //execution date <= 3 days
            if((execDate - placementT) > convertTimestamp("00:00:03:00:00:00")){
                continueCheck = false;
                if(verbose){
                    cout << "Select a time less than three days in the future.\n";
                }
            }
            //sender exists
            if(continueCheck && (existingUsers.find(t.getSender()) == existingUsers.end())){
                continueCheck = false;
                if(verbose){
                    cout << "Sender " << t.getSender() << " does not exist.\n";
                }
            }
            //recipient exists
            if(continueCheck && existingUsers.find(t.getReceiver()) == existingUsers.end()){
                continueCheck = false;
                if(verbose){
                    cout << "Recipient " << t.getReceiver() << " does not exist.\n";
                }
            }
            //execution date is later than the sender's and recipient's
            //registration date (both must have been created at time of transaction)
            if(continueCheck 
            && (t.getExecutionDate() < existingUsers[t.getSender()]->getTimestamp()
            || t.getExecutionDate() < existingUsers[t.getReceiver()]->getTimestamp())){
                continueCheck = false;
                if(verbose){
                    cout << "At the time of execution, sender and/or recipient have not registered.\n";
                }
            }
            //sender has an active user session
            if(continueCheck && (existingUsers[t.getSender()]->getActive() == false)){
                continueCheck = false;
                if(verbose){
                    cout << "Sender " << t.getSender() << " is not logged in.\n";
                }
            }
            if(continueCheck){
                return true;
            }
            return false;
        }

        bool senderReceiverHaveEnough(Transaction &t){
            //the recipient must have enough money in their account to pay the fee before they can receive the transfer. If the transaction can’t happen and verbose mode is on, print the following message:
            //Insufficient funds to process transaction <transaction_id>.

            //if above is the case, discard transaction, don't appear in user's completed
        }

        void ridComment(){
            string junkline;
            getline(cin, junkline);
        }
        uint64_t convertIP(const string &s){
            //finish this with correct implementation
            return 0;
        }
        //logs user in according to rules
        void login(){
            string userID;
            uint64_t pin;
            string longFormIP;
            uint64_t ip;
            cin >> userID >> pin >> longFormIP;
            ip = convertIP(longFormIP);
            if(existingUsers.find(userID) != existingUsers.end()){
                if(existingUsers[userID]->getPin() == pin){
                    userIPs[userID].insert(ip);
                    existingUsers[userID]->makeActive();
                    if(verbose){
                        cout << "User " << userID << " logged in.\n";
                    }
                    return;
                }
            }
            if(verbose){
                cout << "Failed to log in " << userID << ".\n";
            }
        }
        //logs user out according to rules
        void logout(){
            string userID;
            string longFormIP;
            uint64_t ip;
            cin >> userID >> longFormIP;
            ip = convertIP(longFormIP);

            if(existingUsers.find(userID) != existingUsers.end()){
                // If the user has an active session and the IP is an IP the user logged in with before,
                if(existingUsers[userID]->getActive() && (userIPs[userID].find(ip) != userIPs[userID].end())){
                    // this logs them out, 
                    existingUsers[userID]->deActive();
                    // and removes that IP from the valid IP list for that user. 
                    userIPs[userID].erase(ip);

                    // If the user successfully logs out and the verbose flag is set, 
                    //print User <USER_ID> logged out.
                    if(verbose){
                        cout << "User " << userID << " logged out.\n";
                    }
                }
                else{
                    //If the IP doesn’t match any of the IP’s that the user has previously 
                    //logged in with and the verbose flag is set, print Failed to log out <USER_ID>. 
                    if(verbose){
                        cout << "Failed to log out " << userID << ".\n";
                    }
                }
            }

        }

        void place(){
            // Usage: place <TIMESTAMP> <IP> <SENDER> <RECIPIENT> <AMOUNT> <EXEC_DATE> <o/s>
            // This command will have 7 parts, the timestamp at which the order is placed, 
            //     the IP of the sender, 
            //     the sender ID, 
            //     the recipient ID, 
            //     the transaction amount, 
            //     proposed execution date, 
            //     and whether the transaction fee is covered by our account 
            //         (o, meaning the sender) 
            //         or shared equally between the sender and the recipient (s). 
            string stringFormtimestamp;
            string longFormIP;
            string sender;
            string recipient;
            uint64_t amount;
            string stringFormExecDate;
            char oORs;

            cin >> stringFormtimestamp >> longFormIP >> sender >> recipient 
                    >> amount >> stringFormExecDate >> oORs;
            
            //do all conversions from strings to uint64_t for what requires it
            uint64_t timestamp = convertTimestamp(stringFormtimestamp);
            uint64_t execDate = convertTimestamp(stringFormExecDate);
            uint64_t ip = convertIP(longFormIP);
            Transaction currTransaction(timestamp, ip, sender, recipient, amount, execDate);
            bool isGood = validateTransaction(currTransaction);
            if(isGood){
                isGood = checkFraudulent(currTransaction);
            }
            //now add the transaction
            if(isGood && senderReceiverHaveEnough(currTransaction)){
                pendingTs.push(&currTransaction);
                if(verbose){
                    cout << "Transaction at " << stringFormtimestamp << ": $" << amount 
                    << " from " << sender << " to " << recipient << " at " << stringFormExecDate << ".\n";
                }
            }
            //*****Logged out users can’t place transaction requests without logging back in. *******
            
            // This is used to place a transaction to be executed in the future. 
            // You should make sure that all place commands contain non-decreasing 
            // timestamps (see the Error-checking section). 
            // As per international guidelines, 281Bank is only allowed to place 
            // transactions that are processed up to three days ahead of the given 
            // timestamp, meaning EXEC_DATE - TIMESTAMP <= 3 days.

            // When producing the output of the place command, the output should 
            // always occur in non-decreasing order of timestamps, in terms of 
            // when they are placed and actually executed.
        }
        bool callCommand(const string &commandName){
            //bool successLogin;
            if(commandName[0] == '#'){
                ridComment();
            }
            else if(commandName == "login"){
                //might have to save the outcome of this, not sure yet
                login();
            }
            else if(commandName == "out"){
                logout();
                
            }
            else if(commandName == "place"){
                place();
            }
        }

        void readCommands(){
            readRegistrationFile();
            string curr;
            cin >> curr;
            while(curr != "$$$"){
                callCommand(curr);
                cin >> curr;
            }
        }

        void readRegistrationFile(){
            fstream fin(filename);
            while(fin.good()){
                //read line one by one
                string ts;
                getline(fin, ts, '|');
                string i;
                getline(fin, i, '|');
                string p;
                getline(fin, p, '|');
                string sb;
                getline(fin, sb);
                uint64_t pin = (uint64_t)stoi(p);
                uint64_t startBalance = (uint64_t)stoi(sb);
                User newU(convertTimestamp(ts), i, pin, startBalance);
                existingUsers[i] = &newU;
            }
        }
    public:
        
        void getMode(int argc, char * argv[]) {
            opterr = false; // Let us handle all error output for command line options
            int choice = 0;
            int index = 0;
            option long_options[] = {
                    {"help", no_argument, nullptr, 'h'},
                    {"verbose", no_argument, nullptr, 'v'},
                    {"file", required_argument, nullptr, 'f'},
                    { nullptr, 0, nullptr, '\0' }, //leave alone this has to be last thing in long options
            };  // long_options[]
            while ((choice = getopt_long(argc, argv, "hvf:", long_options, &index)) != -1) {
                switch (choice) {
                    case 'h':
                        printHelp(argv);
                        exit(0);
                    
                    case 'v':
                        verbose = true;
                        break;

                    case 'f':
                        filename = optarg;
                        file = true;
                        break;
                    
                    
                    default:
                        cerr << "Error: invalid command line provided\n";
                        cerr << "See -h or --help for details\n";
                        exit(1);
                }  // switch ..choice
            }  // whiles
            if(!file){
                throwFileError();
            }
        }  // getMode()

            // Print help for the user when requested.
            // argv[0] is the name of the currently executing program
        void printHelp(char *argv[]) {
            cout << "Usage: " << argv[0] << " [-v and/or -f {filename} or -h] > \"outputfile\" \n";
            //create a more helpful print message
        }  // printHelp()

};