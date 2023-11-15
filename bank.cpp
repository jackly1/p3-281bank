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
#include <cmath>
#include <iterator>
#include <stdlib.h>

using namespace std;
class Transaction{
    private:
        uint64_t placementTime;
        string stringFormPlacementTime;
        string senderIP;
        string sender;
        string receiver;
        uint64_t amount;
        uint64_t executionDate;
        uint64_t fee;
        string stringFromExecDate;
        bool shared;
        uint64_t transactionID;
    public:
    //constructor
        Transaction(const uint64_t &pt, const string &sfpt, const string &si, const string &s, const string &r, const uint64_t &a, const uint64_t &ed, const bool &o, const string &sed)
        : placementTime(pt), stringFormPlacementTime(sfpt), senderIP(si), sender(s), receiver(r), amount(a), executionDate(ed), fee(0), stringFromExecDate(sed), shared(o), transactionID(0){}

        uint64_t getPlacementTime(){
            return placementTime;
        }
        string getIP(){
            return senderIP;
        }
        string getSender(){
            return sender;
        }
        string getReceiver(){
            return receiver;
        }
        uint64_t getAmount(){
            return amount;
        }
        void setFee(const uint64_t &f){
            fee = f;
        }
        uint64_t getFee(){
            return fee;
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

class lowerBoundComparator{
    public:
        bool operator()(Transaction* t1, const uint64_t &executionDate){
            return t1->getExecutionDate() > executionDate;
        }
};

class User{
    private:
        uint64_t regTimestamp;
        string stringTimestamp;
        string id;
        uint64_t pin;
        uint64_t balance;
        bool activeSession;
        unordered_set<string> ips;
        uint64_t transactionsSent;
        uint64_t transactionsReceived;
    public:
        //constructor
        User(const uint64_t &ts, const string &stringts, const string &i, const uint64_t &p, const uint64_t &sb)
        : regTimestamp(ts), stringTimestamp(stringts), id(i), pin(p), balance(sb), activeSession(false), transactionsSent(0), transactionsReceived(0){}

        uint64_t getTimestamp(){
            return regTimestamp;
        }

        string getStringTimestamp(){
            return stringTimestamp;
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
        void increaseSent(){
            transactionsSent++;
        }
        uint64_t numSent(){
            return transactionsSent;
        }
        void increaseReceived(){
            transactionsReceived++;
        }
        uint64_t numReceived(){
            return transactionsReceived;
        }
        void push(string s){
            ips.insert(s);
        }
        void removeElt(string s){
            ips.erase(s);
        }
        bool elementExists(string s){
            if(ips.find(s) != ips.end()){
                return true;
            }
            return false;
        }
};

class bank{
    private:
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
        bool file = false;
        string filename;
        bool verbose = false;
        unordered_map<string, User*> existingUsers;
        priority_queue<Transaction*, vector<Transaction*>, executionComparator> pendingTransactions;
        priority_queue<Transaction*,  vector<Transaction*>, executionComparator> chronologicalTransactions;
        vector<Transaction*> transactionMasterList;
        uint64_t transactionIDCounter = 0;
        uint64_t lastPlacedTimestamp = 0;

        void throwFileError(){
            cerr << "registrations filename not provided\n";
            exit(1);
        }
        uint64_t convertTimestamp(string ts){
            string total;

            total += ts.substr(0,2);
            total += ts.substr(3,2);
            total += ts.substr(6,2);
            total += ts.substr(9,2);
            total += ts.substr(12,2);
            total += ts.substr(15,2);
            uint64_t toReturn = stoull(total);
            return toReturn;
        }

        uint64_t generateNewID(){
            return transactionIDCounter++;
        }


        bool checkFraudulent(Transaction *t){
            if(existingUsers[t->getSender()]->elementExists(t->getIP()) == false){
                if(verbose){
                    cout << "Fraudulent transaction detected, aborting request.\n";
                }
                return false;
            }
            return true;
        }
        bool validateTransaction(Transaction *t){
            //wrote witih continueCheck but could definitely just take out and use
            //return falses instead
            bool continueCheck = true;
            const uint64_t execDate = t->getExecutionDate();
            const uint64_t placementT = t->getPlacementTime();
            //execution date <= 3 days
            if((execDate - placementT) > convertTimestamp("00:00:03:00:00:00")){
                continueCheck = false;
                if(verbose){
                    cout << "Select a time less than three days in the future.\n";
                }
            }
            //sender exists
            if(continueCheck && (existingUsers.find(t->getSender()) == existingUsers.end())){
                continueCheck = false;
                if(verbose){
                    cout << "Sender " << t->getSender() << " does not exist.\n";
                }
            }
            //recipient exists
            if(continueCheck && existingUsers.find(t->getReceiver()) == existingUsers.end()){
                continueCheck = false;
                if(verbose){
                    cout << "Recipient " << t->getReceiver() << " does not exist.\n";
                }
            }
            //execution date is later than the sender's and recipient's
            //registration date (both must have been created at time of transaction)
            if(continueCheck 
            && (t->getExecutionDate() < existingUsers[t->getSender()]->getTimestamp()
            || t->getExecutionDate() < existingUsers[t->getReceiver()]->getTimestamp())){
                continueCheck = false;
                if(verbose){
                    cout << "At the time of execution, sender and/or recipient have not registered.\n";
                }
            }
            //sender has an active user session
            if(continueCheck && (existingUsers[t->getSender()]->getActive() == false)){
                continueCheck = false;
                if(verbose){
                    cout << "Sender " << t->getSender() << " is not logged in.\n";
                }
            }
            if(continueCheck){
                return true;
            }
            return false;
        }

        bool hasSufficientFunds(string userID, uint64_t amountDue){
            if(amountDue > existingUsers[userID]->getBalance()){
                return false;
            }
            return true;
        }

        void ridComment(){
            string junkline;
            getline(cin, junkline);
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
            ifstream fin(filename, ifstream::in);
            string fullLine;
            uint64_t pos = 0;
            while(fin >> fullLine){
                //read timestamp first
                string ts;
                pos = fullLine.find("|");
                ts = fullLine.substr(0, pos);
                fullLine.erase(0, pos + 1);

                //then reads the userID
                string i;
                pos = fullLine.find("|");
                i = fullLine.substr(0, pos);
                fullLine.erase(0, pos + 1);

                //then reads pin
                string p;
                pos = fullLine.find("|");
                p = fullLine.substr(0, pos);
                fullLine.erase(0, pos + 1);
                
                //finally reads balance from the rest of the line
                string sb = fullLine;
                
                //converts pin and balance to unsigned ints
                uint64_t pin = stoull(p);
                uint64_t startBalance = stoull(sb);
                //creates a new user with above info
                User* newU = new User(convertTimestamp(ts), ts, i, pin, startBalance);
                
                //adds the newuser to the existingUsers hash table at <userID, User*>
                existingUsers[i] = newU;
            }
            fin.close();
            //runs through each command line now that users have been registered
            string curr;
            cin >> curr;
            while(curr != "$$$"){
                callCommand(curr);
                cin >> curr;
            }
        }
        string removeColons(const string &s){
            string toReturn;
            for(uint64_t i = 0; i < s.length(); i++){
                if(s[i] != ':'){
                    toReturn += s[i];
                }
            }
            return toReturn;
        }
        //logs user in according to rules
        void login(){
            string userID;
            string p;
            string ip;
            cin >> userID >> p >> ip;
            uint64_t pin = stoull(p);
            if(existingUsers.find(userID) != existingUsers.end()){
                if(existingUsers[userID]->getPin() == pin){
                    existingUsers[userID]->makeActive();
                    existingUsers[userID]->push(ip);
                    if(verbose){
                        cout << "User " << userID << " logged in.\n";
                    }
                }
                else{
                    if(verbose){
                        cout << "Failed to log in " << userID << ".\n";
                    }
                }
            }
            else{
                if(verbose){
                    cout << "Failed to log in " << userID << ".\n";
                }
            }
        }
        //logs user out according to rules
        void logout(){
            string userID;
            string ip;
            cin >> userID >> ip;

            if(existingUsers.find(userID) != existingUsers.end()){
                // If the user has an active session and the IP is an IP the user logged in with before,
                if(existingUsers[userID]->getActive() && (existingUsers[userID]->elementExists(ip))){
                    // this logs them out, 
                    existingUsers[userID]->deActive();
                    // and removes that IP from the valid IP list for that user. 
                    existingUsers[userID]->removeElt(ip);

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
            string stringFormtimestamp;
            string ip;
            string sender;
            string recipient;
            uint64_t amount;
            string stringFormExecDate;
            char oORs;

            cin >> stringFormtimestamp >> ip >> sender >> recipient 
                    >> amount >> stringFormExecDate >> oORs;
            
            //do all conversions from strings to uint64_t for what requires it
            uint64_t timestamp = convertTimestamp(stringFormtimestamp);
            uint64_t execDate = convertTimestamp(stringFormExecDate);

            bool shared = false;
            if(oORs == 'o'){
                shared = false;
            }
            else if(oORs == 's'){
                shared = true;
            }
            if(lastPlacedTimestamp == 0){
                lastPlacedTimestamp = timestamp;
            }
            if(timestamp >= lastPlacedTimestamp && execDate >= timestamp){
                Transaction* currTransaction = new Transaction(timestamp, stringFormtimestamp ,ip, sender, recipient, amount, execDate, shared, stringFormExecDate);
                
                bool isGood = validateTransaction(currTransaction);
                if(isGood){
                    isGood = checkFraudulent(currTransaction);
                }
                //now execute transactions <= the transaction
                if(isGood){
                    executeLessThanEqual(currTransaction);
                    currTransaction->setTransactionID(generateNewID());
                    if(verbose){
                        cout << "Transaction placed at " << currTransaction->getPlacementTime() << ": $" << amount 
                        << " from " << sender << " to " << recipient << " at " << currTransaction->getExecutionDate() << ".\n";
                    }
                    transactionMasterList.push_back(currTransaction);
                    pendingTransactions.push(currTransaction);
                    chronologicalTransactions.push(currTransaction);
                    lastPlacedTimestamp = timestamp;
                }
            }
            else{
                if(timestamp < lastPlacedTimestamp){
                    cerr << "Invalid decreasing timestamp in 'place' command.";
                }
                if(execDate < timestamp){
                    cerr << "You cannot have an execution date before the current timestamp.";
                }
                exit(1);
            }
        }

        void callCommand(const string &commandName){
            //bool successLogin;
            if(commandName[0] == '#'){
                ridComment();
            }
            else if(commandName == "login"){
                login();
            }
            else if(commandName == "out"){
                logout();
                
            }
            else if(commandName == "place"){
                place();
            }
        }

        bool hasBeenLoyal(const string& userID, const uint64_t &transactionTimestamp){
            uint64_t difference = transactionTimestamp - existingUsers[userID]->getTimestamp();
            if(difference > 50000000000ull){
                return true;
            }
            return false;
        }

        uint64_t calcFee(Transaction *t){
            uint64_t fee = (t->getAmount() / 100);
            if(fee > 450){
                fee = 450;
            }
            else if (fee < 10){
                fee = 10;
            }

            if(hasBeenLoyal(t->getSender(), t->getExecutionDate())){
                fee = (fee * 3) / 4;
            }
            return fee;
        }

        void execute(Transaction* t){
            if(!t->isShared()){
                uint64_t totalAmt = t->getAmount();
                uint64_t fee = calcFee(t);
                t->setFee(fee);
                totalAmt += fee;
                if(hasSufficientFunds(t->getSender(), totalAmt)){
                    existingUsers[t->getSender()]->removeMoney(totalAmt);
                    existingUsers[t->getReceiver()]->addMoney(t->getAmount());
                    existingUsers[t->getSender()]->increaseSent();
                    existingUsers[t->getReceiver()]->increaseReceived();
                    if(verbose){
                        cout << "Transaction executed at " << t->getExecutionDate() << 
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
                uint64_t fee = calcFee(t);
                t->setFee(fee);
                //first make both set equal to fee/2
                uint64_t senderFee = fee/2;
                uint64_t receiverFee = fee/2;
                //if fee is odd
                if(fee % 2 != 0){
                    //make the sender fee a rounded up fee/2 
                    senderFee = (fee/2) + 1;
                }
                totalAmt += senderFee;
                if(hasSufficientFunds(t->getSender(), totalAmt) && hasSufficientFunds(t->getReceiver(), receiverFee)){
                    //remove fees from both + amount from sender
                    existingUsers[t->getSender()]->removeMoney(totalAmt);
                    existingUsers[t->getReceiver()]->removeMoney(receiverFee);
                    existingUsers[t->getReceiver()]->addMoney(t->getAmount());
                    //increase how many each of sent and received
                    existingUsers[t->getSender()]->increaseSent();
                    existingUsers[t->getReceiver()]->increaseReceived();
                    if(verbose){
                        cout << "Transaction executed at " << t->getExecutionDate() << 
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
            while(!pendingTransactions.empty() && pendingTransactions.top()->getExecutionDate() <= t->getPlacementTime()){
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

            uint64_t numTransactions = 0;
            
            while(!chronologicalTransactions.empty()){
                if(chronologicalTransactions.top()->getExecutionDate() >= x && chronologicalTransactions.top()->getExecutionDate() < y){
                    if(chronologicalTransactions.top()->getAmount() != 1){
                        cout << chronologicalTransactions.top()->getTransactionID() << ": " << chronologicalTransactions.top()->getSender() 
                        << " sent " << chronologicalTransactions.top()->getAmount() << " dollars to " << chronologicalTransactions.top()->getReceiver()
                        << " at " << chronologicalTransactions.top()->getExecutionDate() << ".\n";
                    }
                    else{
                        cout << chronologicalTransactions.top()->getTransactionID() << ": " << chronologicalTransactions.top()->getSender() 
                        << " sent " << chronologicalTransactions.top()->getAmount() << " dollar to " << chronologicalTransactions.top()->getReceiver()
                        << " at " << chronologicalTransactions.top()->getExecutionDate() << ".\n";
                    }
                    numTransactions++;
                }
                chronologicalTransactions.pop();
            }
            if(numTransactions > 1){
                cout << "There were " << numTransactions << " transactions that were placed between time "
                << x << " to " << y << ".\n";
            }
            else{
                cout << "There was " << numTransactions << " transaction that was placed between time "
                << x << " to " << y << ".\n";
            }
        }


        void numTimePassedInInterval(const string& s1, const string& s2){
            uint64_t x = convertTimestamp(s1);
            uint64_t y = convertTimestamp(s2);

            uint64_t difference = y - x;

            uint64_t years = difference % 1000000000000ull / 10000000000ull;
            uint64_t months = difference % 10000000000ull / 100000000ull;
            uint64_t days = difference % 100000000ull / 1000000ull;
            uint64_t hours = difference % 1000000ull / 10000ull;
            uint64_t minutes = difference % 10000ull / 100ull;
            uint64_t seconds = difference % 100ull;

            if(years != 0){
                if(years > 1){
                    cout << " " << years << " years";
                }
                else{
                     cout << " " << years << " year";
                }
            }
            if(months != 0){
                if(months > 1){
                    cout << " " << months << " months" ;
                }
                else{
                     cout << " " << months << " month";
                }
            }
            if(days != 0){
                if(days > 1){
                    cout << " " << days << " days" ;
                }
                else{
                     cout << " " << days << " day";
                }
            }
            if(hours != 0){
                if(hours > 1){
                    cout << " " << hours << " hours" ;
                }
                else{
                     cout << " " << hours << " hour";
                }
            }
            if(minutes != 0){
                if(minutes > 1){
                    cout << " " << minutes << " minutes" ;
                }
                else{
                     cout << " " << minutes << " minute";
                }
            }
            if(seconds != 0){
                if(seconds > 1){
                    cout << " " << seconds << " seconds";
                }
                else{
                     cout << " " << seconds << " second";
                }
            }
            cout << ".";
        }


        void runBankRevenue(){
            
            string stringForX;
            string stringForY;
            cin >> stringForX >> stringForY;
            
            uint64_t x = convertTimestamp(stringForX);
            uint64_t y = convertTimestamp(stringForY);

            uint64_t amountGenerated = 0;

            for(auto &t: transactionMasterList){
                if(t->getExecutionDate() >= x && t->getExecutionDate() < y){
                    amountGenerated += t->getFee();
                }
            }

            cout << "281Bank has collected " << amountGenerated << " dollars in fees over";
            numTimePassedInInterval(stringForX, stringForY);
            cout << "\n";
        }

        void runHistory(){
            string userID;
            cin >> userID;
            if(existingUsers.find(userID) != existingUsers.end()){
                cout << "Customer " << userID << " account summary:\n";
                cout << "Balance: $" << existingUsers[userID]->getBalance() <<"\n";
                cout << "Total # of transactions: " << (existingUsers[userID]->numReceived() + existingUsers[userID]->numSent()) << "\n";

                deque<Transaction*> incoming;
                deque<Transaction*> outgoing;
                for(auto &t:transactionMasterList){
                    if(t->getReceiver() == userID){
                        incoming.push_back(t);
                    }
                    else if(t->getSender() == userID){
                        outgoing.push_back(t);
                    }
                }

                cout << "Incoming " << existingUsers[userID]->numReceived() << ":\n";
                uint64_t counter = 0;
                while(!incoming.empty() && counter <= 10){
                    if(incoming.front()->getAmount() != 1){
                        cout << incoming.front()->getTransactionID() << ": " << incoming.front()->getSender() << " sent " 
                        << incoming.front()->getAmount() << " dollars to " << incoming.front()->getReceiver() << " at "
                        << incoming.front()->getExecutionDate() << ".\n";
                    }
                    else{
                        cout << incoming.front()->getTransactionID() << ": " << incoming.front()->getSender() << " sent " 
                        << incoming.front()->getAmount() << " dollar to " << incoming.front()->getReceiver() << " at "
                        << incoming.front()->getExecutionDate() << ".\n";
                    }
                    incoming.pop_front();
                    counter++;
                }

                counter = 0;
                cout << "Outgoing " << existingUsers[userID]->numSent() << ":\n";
                while(!outgoing.empty() && counter <= 10){
                    if(outgoing.front()->getAmount() != 1){
                        cout << outgoing.front()->getTransactionID() << ": " << outgoing.front()->getSender() << " sent " 
                        << outgoing.front()->getAmount() << " dollars to " << outgoing.front()->getReceiver() << " at "
                        << outgoing.front()->getExecutionDate() << ".\n";
                    }
                    else{
                        cout << outgoing.front()->getTransactionID() << ": " << outgoing.front()->getSender() << " sent " 
                        << outgoing.front()->getAmount() << " dollar to " << outgoing.front()->getReceiver() << " at "
                        << outgoing.front()->getExecutionDate() << ".\n";
                    }
                    outgoing.pop_front();
                    counter++;
                }
            }
            else{
                cout << "User " << userID << " does not exist.\n";
            }    
        }

        void runSummarizeDay(){
            string timestamp;
            cin >> timestamp;

            uint64_t lowerDayBound = convertTimestamp(timestamp);
            //truncate and turn everything after the day to 0s
            lowerDayBound = (lowerDayBound / 1000000ULL) * 1000000ULL;

            uint64_t upperDayBound = lowerDayBound + 1000000ULL;

            int counter = 0;
            uint64_t amountGenerated = 0;

            cout << "Summary of [" << lowerDayBound << ", " << upperDayBound << "):\n";
            
            for(auto &t: transactionMasterList){
                if(t->getExecutionDate() >= lowerDayBound && t->getExecutionDate() < upperDayBound){
                    if(t->getAmount() == 1){
                        cout << t->getTransactionID() << ": " << t->getSender() << " sent "
                        << t->getAmount() << " dollar to " << t->getReceiver() << " at " << 
                        (t->getExecutionDate()) << ".\n";
                    }
                    else{
                        cout << t->getTransactionID() << ": " << t->getSender() << " sent "
                        << t->getAmount() << " dollars to " << t->getReceiver() << " at " << 
                        (t->getExecutionDate()) << ".\n";
                    }

                    amountGenerated += t->getFee();
                    counter++;
                }
                else if(t->getExecutionDate() > upperDayBound){
                    break;
                }
            }
            if(counter == 1){
                cout << "There was a total of " << counter << " transaction," 
                << " 281Bank has collected " << amountGenerated << " dollars in fees.\n";
            }
            else{
                cout << "There were a total of " << counter << " transactions," 
                << " 281Bank has collected " << amountGenerated << " dollars in fees.\n";
            }
            
        }
        void cleanUpPointers(){
            for(auto &t:transactionMasterList){
                delete t;
            }
            for(auto& u:existingUsers){
                delete u.second;
            }
        }
    public:
        void read(){
            readCommands();
            executeTransactions();
            readQueries();
            cleanUpPointers();
        }
        
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