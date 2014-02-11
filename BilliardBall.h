/* 
 * File:   BilliardBall.h
 * Author: ardit
 *
 * Created on October 1, 2013, 9:06 AM
 */
#include <iostream>
#include <fstream>
#include <string>
using namespace std;

#ifndef BILLIARDBALL_H
#define	BILLIARDBALL_H

#define NUM_TOKENS 4
#define FRICTION 1.0
#define BALL_RADIUS 3
#define BALL_MASS 1
#define PI 3.14159

// A struct that will contain the informations that will be parsed from 
// the input file. Such information will include the x and y position of
// the ball in the billiard ball table which will be implemented using a 2d
// array with fixed size.The struct also includes the velocity and the degree
// in which the shooting ball will go.
struct Ball {
    int id;
    // x and y position of the ball
    int xPos, yPos;
    // velocity of the ball
    double velocity;
    // degree of the ball
    double degree;
    int mass;
};

struct Table {
    int width;
    int height;
};

class BilliardBall 
{
    private:
        // The array of type struct Ball that will contain all the balls informations.
        Ball balls[10];
        // set hitting ball
        bool hittingBall;
        // save the hitting ball index
        int hittingBallIndex;
        Table table;
    public:
        int errno;
        // Constructor of the class which will initialize the balls array to 0.
        BilliardBall();
        // Destructor of the class
        ~BilliardBall();
        
        // The function that will parse the input file
        int parseInputFile(ifstream &inputFile);
        
        // Helper function used inside the parseInputFile function that will
        // parse the line and do error checking aswell.The input is a string
        // which is passed by reference.
        int parseLine(string &line, int &numOfBalls);
        
        // Print all the values of the specific ball
        void printBallValues(Ball ball);

        // Set the ball in the table array
        int setBallInTable(Ball currentBall, int &numOfBalls);

        // Print all Balls value in the stdout
        void printBalls();
        // Run the Billiard Ball simulation
        void runSimulation();
        
        // update the position of currentBall
        void updatePosition(Ball &currentBall);
        
        // calculate the balls new positions
        void calculateBallsNewValue(Ball &firstBall, Ball &secondBall);
        
        // returns the index of the touching Ball
        int checkBallToBallCollision(Ball currentBall);
        
        // detects if there is a wall collision and changes the direction of the ball
        bool detectWallCollision(Ball &currentBall); 
        
        // get the smallest distance the ball can move in x or y direction to keep the same ratio
        int getSmallestDistance(double velocity);

        // write the new values of the balls in an output file
        void writeToFile();
};


#endif	/* BILLIARDBALL_H */

