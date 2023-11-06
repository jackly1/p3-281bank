// Project Identifier: 292F24D17A4455C1B5133EDD8C7CEAA0C9570A98
//EECS 281, Project 3
 
#include <getopt.h>
#include <iostream>
#include <fstream>
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
            public:
                //constructor
                User(const uint64_t &ts,const string &i, const uint64_t &p, const uint64_t &sb)
                : regTimestamp(ts), id(i), pin(p), startBalance(sb){}

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
                : placementTime(pt), senderIP(si), sender(i), receiver(r), amount(a), executionDate(ed){}

                void setPlacementTime(const string &i){
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

        bool file = false;
        string filename;
        bool verbose = false;
        unordered_map<string, User> existingUsers;
        unordered_map<User, uint64_t> userIPs;
        priority_queue<Transaction> pendingTs;
        
        
        void throwFileError(){
            cerr << "file name not provided\n";
            exit(1);
        }

        uint64_t subtractDates(const uint64_t &d1, const uint64_t &d2){

        }
        uint64_t summarizeTS(const vector<uint64_t> &toSum){
            uint64_t sum = 0;
            uint64_t multiplier = 1;
            while(!toSum.empty()){
                sum += (toSum.back() * multiplier);
                multiplier *= 99;
                toSum.pop_back();
            }
            return toSum;
        }
        uint64_t convertTimestamp(const string &ts){
            stringstream sin(ts);
            vector<uint64_t> nums;
            string curr;
            while(getline(sin, curr, ':')){
                nums.push_back(((uint64_t)atoi(curr)));
            }
            getline(sin, curr);
            nums.push_back(((uint64_t)atoi(curr)));
        }
        bool validTransaction(const Transaction &t){
            if(subtractDates(t.getExecutionDate(), t.getPlacementTime()) <= 3
            && ){
                return true;
            }
            return false;
        } 
        void place(const Transaction &t){
            if(validTransaction(t)){

            }
        }
    public:
        void ridComment(){
            string junkline;
            getline(cin, junkline);
        }
        uint64_t convertIP(const string &s){
            //finish this with correct implementation
            return (uint64_t)s;
        }
        bool login(){
            string userID;
            uint64_t pin;
            string longFormIP;
            uint64_t ip;
            cin >> userID >> pin >> longFormIP;
            ip = convertIP(longFormIP);
            if(existingUsers.find(userID) != existingUsers.end()){
                if(existingUsers[userID].getPin() == pin){
                    userIPs[existingUsers[userID]] = ip;
                    return true;
                }
                return false;
            }
            return false;
        }
        void callCommand(const string &commandName){
            if(commandName[0] == '#'){
                ridComment();
            }
            else if(commandName == "login"){
                login();
            }
            else if(commandName == "out"){

            }
            else if(commandName == "place"){

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
                uint64_t p;
                getline(fin, p, '|');
                uint64_t sb;
                getline(fin, sb);
                User newU(ts, i, p, sb);
                existingUsers.insert(newU);
            }
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
                        string arg{optarg};
                        file = true;
                        filename = arg;

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