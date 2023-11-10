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
#include <set>

using namespace std;
class bank{
    private:
    class Transaction{
            private:
                uint64_t placementTime;
                uint64_t senderIP;
                string sender;
                string receiver;
                uint64_t amount;
                uint64_t executionDate;
                string stringFromExecDate;
                bool shared;
                uint64_t transactionID;
            public:
            //constructor
                Transaction(const uint64_t &pt,const uint64_t &si, const string &s, const string &r, const uint64_t &a, const uint64_t &ed, const bool &o, const string &sed)
                : placementTime(pt), senderIP(si), sender(s), receiver(r), amount(a), executionDate(ed), stringFromExecDate(sed), shared(o), transactionID(0){}

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
                bool isShared(){
                    return shared;
                }
                string getExecString(){
                    return stringFromExecDate;
                }
                void setTransactionID(const uint64_t &n){
                    transactionID = n;
                }
                uint64_t getTransactionID(){
                    return transactionID;
                }
        };
        class User{
            private:
                uint64_t regTimestamp;
                string id;
                uint64_t pin;
                uint64_t balance;
                bool activeSession;
                vector<Transaction*> userTransactions;
            public:
                //constructor
                User(const uint64_t &ts,const string &i, const uint64_t &p, const uint64_t &sb)
                : regTimestamp(ts), id(i), pin(p), balance(sb), activeSession(false){}

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
                void addMoney(const uint64_t &n){
                    balance += n;
                }
                void removeMoney(const uint64_t &n){
                    balance -= n;
                }
                uint64_t getBalance(){
                    return balance;
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
                void addTransaction(Transaction* t){
                    userTransactions.push_back(t);
                }
        };
        

        class executionComparator{
            public:
                bool operator()(Transaction *a, Transaction *b){
                    if(a->getExecutionDate() != b->getExecutionDate()){
                        return a->getExecutionDate() > b->getExecutionDate();
                    }
                    else{
                        return a->getTransactionID() > b->getTransactionID();
                    }
                }
        };
        class chronologicalComparator{
            public:
                bool operator()(Transaction *a, Transaction *b){
                    if(a->getPlacementTime() != b->getPlacementTime()){
                        return a->getPlacementTime() > b->getPlacementTime();
                    }
                    else{
                        return a->getTransactionID() > b->getTransactionID();
                    }
                }
        };


        bool file = false;
        string filename;
        bool verbose = false;
        unordered_map<string, User*> existingUsers;
        unordered_map<string, unordered_set<uint64_t>> userIPs;
        priority_queue<Transaction*, vector<Transaction*>, executionComparator> pendingTransactions;
        set<Transaction*, chronologicalComparator> chronologicalTransactions;
        deque<Transaction*> transactionMasterList;
        uint64_t transactionIDCounter = 0;

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

        uint64_t generateNewID(){
            return transactionIDCounter++;
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

        bool hasSufficientFunds(string userID, uint64_t amountDue){
            if(existingUsers[userID]->getBalance() - amountDue < 0){
                return false;
            }
            return true;
        }

        void ridComment(){
            string junkline;
            getline(cin, junkline);
        }
        uint64_t convertIP(const string &s){
            return (uint64_t)(stoi(s));
        }

        string removeColons(const string &s){
            string toReturn;
            for(int i = 0; i < s.length(); i++){
                if(s[i] != ':'){
                    toReturn += s[i];
                }
            }
            return toReturn;
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
            bool shared;
            if(oORs == 'o'){
                shared = false;
            }
            else if(oORs == 's'){
                shared = true;
            }
            Transaction currTransaction(timestamp, ip, sender, recipient, amount, execDate, shared, stringFormExecDate);
            bool isGood = validateTransaction(currTransaction);
            if(isGood){
                isGood = checkFraudulent(currTransaction);
            }
            //now execute transactions <= the transaction
            executeLessThanEqual(&currTransaction);
            if(isGood){
                currTransaction.setTransactionID(generateNewID());
                if(verbose){
                    cout << "Transaction placed at " << removeColons(stringFormtimestamp) << ": $" << amount 
                    << " from " << sender << " to " << recipient << " at " << removeColons(stringFormExecDate) << ".\n";
                }
                transactionMasterList.push_back(&currTransaction);
                pendingTransactions.push(&currTransaction);
                chronologicalTransactions.insert(&currTransaction);
            }
        }

        void callCommand(const string &commandName){
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

        bool hasBeenLoyal(const string& userID){
            if(existingUsers[userID]->getTimestamp() - convertTimestamp("05:00:00:00:00:00") >= 0){
                return true;
            }
            return false;
        }

        uint64_t calcFee(const uint64_t &amount){
            uint64_t fee = (uint64_t)((int)amount / 100);
            if(fee < 10){
                return 10;
            }
            else if (fee > 450){
                return 450;
            }
            else{
                return fee;
            }
        }

        void execute(Transaction* t){
            if(!t->isShared()){
                uint64_t totalAmt = t->getAmount();
                uint64_t fee = calcFee(t->getAmount());
                if(hasBeenLoyal(t->getSender())){
                    fee = (fee * 3) / 4;
                }
                totalAmt += fee;
                if(hasSufficientFunds(t->getSender(), totalAmt)){
                    existingUsers[t->getSender()]->removeMoney(totalAmt);
                    existingUsers[t->getReceiver()]->addMoney(t->getAmount());
                    if(verbose){
                        cout << "Transaction executed at " << removeColons(t->getExecString()) << 
                        ": $" << t->getAmount() << " from " << t->getSender() << " to " 
                        << t->getReceiver() << ".\n";
                    }
                }
                else{
                    if(verbose){
                        cout << "Insufficient funds to process transaction " << t->getTransactionID() << ".\n";
                    }
                }
            }else{
                //shared means the fee is shared between the two parties
                uint64_t totalAmt = t->getAmount();
                uint64_t fee = calcFee(t->getAmount());
                if(hasBeenLoyal(t->getSender())){
                    fee = (fee * 3) / 4;
                }
                //first make both set equal to fee/2
                uint64_t senderFee = fee/2;
                uint64_t receiverFee = fee/2;
                //if fee is odd
                if(fee % 2 != 0){
                    //make the sender fee a rounded up fee/2 
                    senderFee = (uint64_t)ceil(fee/2.0);
                    //make the reciever a truncated fee/2
                    receiverFee = fee/2;
                }
                totalAmt += senderFee;
                if(hasSufficientFunds(t->getSender(), totalAmt) && hasSufficientFunds(t->getReceiver(), receiverFee)){
                    //remove fees from both + amount from sender
                    existingUsers[t->getSender()]->removeMoney(totalAmt);
                    existingUsers[t->getReceiver()]->removeMoney(receiverFee);
                    existingUsers[t->getReceiver()]->addMoney(t->getAmount());
                    if(verbose){
                        cout << "Transaction executed at " << removeColons(t->getExecString()) << 
                        ": $" << t->getAmount() << " from " << t->getSender() << " to " 
                        << t->getReceiver() << ".\n";
                    }
                }
                else{
                    if(verbose){
                        cout << "Insufficient funds to process transaction " << t->getTransactionID() << ".\n";
                    }
                }
            }
        }

        void executeLessThanEqual(Transaction* t){
            while(pendingTransactions.top()->getExecutionDate() <= t->getPlacementTime()){
                execute(pendingTransactions.top());
                pendingTransactions.pop();
            }
        }

        void executeTransactions(){
            while(!pendingTransactions.empty()){
                execute(pendingTransactions.top());
                pendingTransactions.pop();
            }
        }

        void runList(){
            string stringForX;
            string stringForY;
            cin >> stringForX >> stringForY;
            
            uint64_t x = convertTimestamp(stringForX);
            uint64_t y = convertTimestamp(stringForY);

            uint64_t numTransactions;
            
            for(auto &t: chronologicalTransactions){
                if(t->getExecutionDate() >= x && t->getExecutionDate() < y){
                    if(t->getAmount() != 1){
                        cout << "  " << t->getTransactionID() << ": " << t->getSender() 
                        << " sent " << t->getAmount() << " dollars to " << t->getReceiver()
                        << " at " << removeColons(t->getExecString()) << ".\n";
                    }
                    else{
                        cout << t->getTransactionID() << ": " << t->getSender() 
                        << " sent " << t->getAmount() << " dollar to " << t->getReceiver()
                        << " at " << removeColons(t->getExecString()) << ".\n";
                    }
                    numTransactions++;
                }
            }
            cout << "There were " << numTransactions << " transactions that were placed between time "
            << stringForX << " to " << stringForY << ".\n";
        }

        void runBankRevenue(){

        }

        void runHistory(){

        }

        void runSummarizeDay(){

        }
        void readQueries(){
            string curr;
            while(cin >> curr){
                if(curr == "l"){
                    runList();
                }else if(curr == "r"){
                    runBankRevenue();
                }else if(curr == "h"){
                    runHistory();
                }else if(curr == "s"){
                    runSummarizeDay();
                }
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
            executeTransactions();

            readQueries();
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