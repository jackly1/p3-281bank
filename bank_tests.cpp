// Project Identifier: 292F24D17A4455C1B5133EDD8C7CEAA0C9570A98
//EECS 281, Project 3

#include "bank.cpp"
#include <iostream>
#include <sstream>

using namespace std;

/*
1506732  2483625  3774074  
4338424  5283760  6330490  
7235047  8087950  9017082  
0883975*/
//caen runner: rsync -rtv --exclude '.git*' ../p1-puzzle/ jackly@login-course.engin.umich.edu:/p1-puzzle-copy/
//ssh jackly@login-course.engin.umich.edu

int main(int argc, char *argv[]) {
  // This should be in all of your projects, speeds up I/O
  ios_base::sync_with_stdio(false);


  // Get the mode from the command line and read in the data
  bank b;
  b.getMode(argc, argv);
  b.read();
}