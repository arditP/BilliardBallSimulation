/* 
 * File:   main.cpp
 * Author: ardit
 *
 * Created on October 1, 2013, 8:55 AM
 */

#include <cstdlib>
#include <iostream>
#include <fstream>
#include "BilliardBall.h"
#include "ErrorChecking.h"
using namespace std;

/*
 * 
 */

void printError(int errno)
{
    switch(errno)
    {
        case ETOKEN:
            cout<<"Error: too many or to fewtokens in each line of the file"<<endl;
            break;
        case ENOTNUM:
            cout<<"Error: couldn't parse the number. Check the file!!!"<<endl;
            break;
        case EHITTING:
            cout<<"Error: the file has more than one ball with velocity greater than 0."<<endl;
            cout<<"This simulation doesn't allow more than one balls to be the hitting ball!!!"<<endl;
            break;
        case EFILE:
            cout<<"Error: there was an error with the file"<<endl;
            break;
        case ETOMANYBALLS:
            cout<<"Error: there are more than 10 balls in the file"<<endl;
    }
}

int main(int argc, char** argv) {
    
    // The string with the input file name
    string filename;
    
    // The initialization of the object BilliardBall
    BilliardBall simBilliard;
    
    // The menu with the instructions for the user
    cout<<"*********** Welcome to the Billiard Ball Simulator ***********"<<endl;
    cout<<"Please input the name of the text file that has the ball position"<<endl;
    cout<<"(Be sure to write the filename and the extension of the file)"<<endl;
    cout<<"> ";
    
    // save the input from the user into the string filename
    getline(cin, filename);
    
    // Create an ifstream variable that will point to the file if the file exist
    // and pass that variable to a function inside the BilliardBall object. If
    // the filename is not correct or another error has occured.
    ifstream inputFile(filename.c_str());
    
    if (inputFile.is_open()){
        if (simBilliard.parseInputFile(inputFile) == -1)
        {
            printError(simBilliard.errno);
            return -1;
        }
    } else {
        simBilliard.errno = EFILE;
        printError(simBilliard.errno);
        return -1;
    }
    simBilliard.runSimulation();
    return 0;
}