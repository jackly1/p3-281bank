// Project Identifier: 292F24D17A4455C1B5133EDD8C7CEAA0C9570A98
//EECS 281, Project 3
 
#include <getopt.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector> 
#include <deque>
#include <unordered_map>
#include <unordered_set>

using namespace std;
class bankDrive{
    private:
        bool file = false;
        string filename;
        bool verbose = false;
        void throwFileError(){
            cerr << "file name not provided\n";
            exit(1);
        }
    public:
        void readFile(){
            //change all below to not cin, but fin
            string comm;
            cin >> comm;
            string restLine;
            while(comm[0] == '#'){
                getline(cin, restLine);
                cin >> comm;
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
                        fstream fin(filename);
                        
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