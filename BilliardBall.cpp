#include "BilliardBall.h"
#include "ErrorChecking.h"
#include <algorithm>
#include <sstream>
#include <cmath>
using namespace std;


// Helper function to trim the whitespace from the beginning and the end of the line.
void trim(string &line)
{
    // remove all whitespaces at the beginning and at the end of the string line
    line.erase(remove_if(line.begin(), line.end(), ::isspace), line.end());
}

// returns true if the given string is a num
bool is_num(const string &str)
{
    return str.find_first_not_of("0123456789") == std::string::npos;
}
// The BilliardBall class Constructor wich initilaizes all the private variables
BilliardBall::BilliardBall()
{
    // Initialize everything to zero for the object balls when the constructor is called
    for(int i = 0; i < 10; i++)
    {
        balls[i].id = 0;
        balls[i].xPos = 0;
        balls[i].yPos = 0;
        balls[i].velocity = 0.0;
        balls[i].degree = 0.0;
        balls[i].mass = BALL_MASS;
    }

    // set the table width = 180 unit and height = 370 unit
    table.width = 180;
    table.height = 370;
    // set hittingBall to false
    hittingBall = false;
    // set hittingBallIndex to -1
    hittingBallIndex = -1;
    errno = 0;
}

BilliardBall::~BilliardBall()
{
    // Nothing to destruct because we are not using any dynamic allocation
}

// This function is called inside main and is passed the adress of the filestream
// of the input file. It return -1 if and error is found
int BilliardBall::parseInputFile(ifstream &inputFile)
{
    string line;
    int numOfBalls = 0;
    while (getline(inputFile, line))
    {
        if(parseLine(line, numOfBalls) == -1)
            return -1;
    }
    
    // check if the number of balls is greater than 10 and return -1 and set the errno value
    if(numOfBalls > 10)
    {
        errno = ETOMANYBALLS;
        return -1;
    }
    
    inputFile.close();
    return 0;
}

// This function parses all the lines in the input file and checks for error
// it returns -1 if an error is found
int BilliardBall::parseLine(string &line, int &numOfBalls)
{
    // Remove leading and trailing whitespaces
    trim(line);
    
    // Check if the line is empty and return from the function to go to the next line
    if(line.empty())
        return 0;
    
    // The position of the delimiter that we are choosing to parse the line
    // in our case it is ';'
    int delimiterPos;
   
    // The tokens parsed from the string line which should be 4
    string tokens[4];
    
    // Count the number of tokens in the string;
    int numOfTokens = 0;
    
    while((delimiterPos = line.find(";")) != std::string::npos)  {
        tokens[numOfTokens] = line.substr(0, delimiterPos);
        if(!is_num(tokens[numOfTokens]))
        {
            errno = ENOTNUM;
            return -1;
        }
        line.erase(0, delimiterPos + 1);
        numOfTokens++;
    }
    
    // Checking if the number of tokens in the line is greater than 4
    if(numOfTokens != 4)
    {
        errno = ETOKEN;
        return -1;
    }


    // After error checking set the respective variables in ball struct
    balls[numOfBalls].id = numOfBalls + 1;
    balls[numOfBalls].xPos = atoi(tokens[0].c_str());
    balls[numOfBalls].yPos = atoi(tokens[1].c_str());
    balls[numOfBalls].velocity = atof(tokens[2].c_str());
    balls[numOfBalls].degree = atof(tokens[3].c_str()) * (PI/180);

   
    
    if(balls[numOfBalls].velocity != 0.0)
    {
        // set the hitting ball to true
        if(!hittingBall) {
            hittingBall = true;
            hittingBallIndex = numOfBalls;
        } else {
            // The hitting ball already set so return an error to the user that there can't be
            // more then 2 hitting balls in the billiard table
            errno = EHITTING;
            return -1;
        }
    }

    // Increment the numofBalls variable
    return setBallInTable(balls[numOfBalls], numOfBalls);

}

// Helpfull function during debuging
void BilliardBall::printBallValues(Ball ball)
{
    cout<<"*****************************************"<<endl;
    cout<<"The x position is: "<<ball.xPos<<endl;
    cout<<"The y position is: "<<ball.yPos<<endl;
    cout<<"The velocity is: "<<ball.velocity<<endl;
    cout<<"The degree is: "<<ball.degree<<endl;
    cout<<"*****************************************"<<endl;
}

int BilliardBall::setBallInTable(Ball currentBall, int &numOfBalls)
{

    if(currentBall.xPos <= BALL_RADIUS ||
       currentBall.xPos >= (table.width - BALL_RADIUS) || 
       currentBall.yPos <= BALL_RADIUS || 
       currentBall.yPos >= (table.height - BALL_RADIUS)) 
    {
        cout<<"The ball can't be at the edges"<<endl;
        return -1;
    }

    for(int i = 0; i < numOfBalls; i++)
    {
        for (int theta = 0; theta <= 360; theta++)
        {
            int xDiffCenterPos = fabs(currentBall.xPos - balls[i].xPos);
            int yDiffCenterPos = fabs(currentBall.yPos - balls[i].yPos);
            if(xDiffCenterPos < fabs(2*BALL_RADIUS*cos(theta)) && yDiffCenterPos < fabs(2*BALL_RADIUS*sin(theta)))
            {
                cout<<"The balls can't overlap!!!"<<endl;
                return -1;
            } 
        }
    }

    // increment the number of balls
    numOfBalls++;
    return 0;
}


void BilliardBall::printBalls()
{
    for (int i = 0; i < 10; i++)
    {
        printBallValues(balls[i]);
    }
}

void BilliardBall::runSimulation()
{
    cout<<"The simulation is Running..."<<endl;
    bool allBallsStopped = false;
    while (!allBallsStopped)
    {
        allBallsStopped = true;
        for(int i = 0; i<10; i++)
        {   
            if(balls[i].velocity != 0.0)
            {
                updatePosition(balls[i]);
                // there are still balls that are not stopped
                allBallsStopped = false;
            }
        }
    }
    cout<<"The simulation has STOPPED"<<endl;
    writeToFile();
    cout<<"The final positions are written in output.txt"<<endl;
}

void BilliardBall::updatePosition(Ball &currentBall)
{
    // convert from m/s to cm/s
    double velocityX = (currentBall.velocity * cos(currentBall.degree));
    double velocityY = (currentBall.velocity * sin(currentBall.degree));

    // get the smallest dx and dy values in order for the ratio to be the same 
    int dx = getSmallestDistance(velocityX);
    int dy = getSmallestDistance(velocityY);

    int counter = 0;
    if(currentBall.velocity > 0.1)
    {
        // update the current position of the ball
        currentBall.xPos = currentBall.xPos + dx;
        currentBall.yPos = currentBall.yPos + dy;

        // decrease the velocity
        currentBall.velocity =  (currentBall.velocity > 0.1) ? currentBall.velocity - (FRICTION / int(velocityX/dx)) : 
                                currentBall.velocity + (FRICTION / int(velocityX/dx));
       
        // detect a wall collision and recalculate the values of the velocity by returning
        if(detectWallCollision(currentBall))
            return;
        
        // if the return value from checkBallToBallCollision is different than -1 than
        // there is a ball to ball collision and the returned value is the index of the other ball
        int indexBallColliding = checkBallToBallCollision(currentBall);
        if(indexBallColliding != -1)
        {
            calculateBallsNewValue(currentBall, balls[indexBallColliding]);
            return;
        }

    }
    else {
        // set velocity and degree to 0 
        currentBall.velocity = 0.0;
        currentBall.degree = 0.0;
    }

}

int BilliardBall::checkBallToBallCollision(Ball currentBall)
{
    for(int i = 0; i < 10; i++)
    {
        if(currentBall.id != balls[i].id)
        {
                int xDiffCenterPos = fabs(currentBall.xPos - balls[i].xPos);
                int yDiffCenterPos = fabs(currentBall.yPos - balls[i].yPos);
                int sumRadius = 2 * BALL_RADIUS;
                int radiusSqr = sumRadius * sumRadius;
                int distanceSqr = (xDiffCenterPos * xDiffCenterPos) + (yDiffCenterPos * yDiffCenterPos);
                // if the distance square is smaller than radius sqr than they are colliding
                if(distanceSqr <= radiusSqr)
                    return i;
        }
    }

    // if no touching ball is found return -1
    return -1;
}

bool BilliardBall::detectWallCollision(Ball &currentBall)
{
    double velocityX = currentBall.velocity * cos(currentBall.degree);
    double velocityY = currentBall.velocity * sin(currentBall.degree);
    bool detected = false;
    
    if (currentBall.xPos - BALL_RADIUS < 0)
    {
        currentBall.xPos = BALL_RADIUS;
        velocityX *= -1;
        detected = true;
    }
    else if (currentBall.xPos + BALL_RADIUS > table.width)
    {
        currentBall.xPos = table.width - BALL_RADIUS; 
        velocityX *= -1;
        detected = true;
    }
    if (currentBall.yPos - BALL_RADIUS < 0) 
    {
        currentBall.yPos = BALL_RADIUS;
        velocityY *= -1;
        detected = true;
    }
    else if (currentBall.yPos + BALL_RADIUS > table.height)
    {
        currentBall.yPos = table.height - BALL_RADIUS; 
        velocityY *= -1;
        detected = true;
    }
   
    currentBall.degree = atan2(velocityY, velocityX);
    return detected;
}

int BilliardBall::getSmallestDistance(double velocity)
{
    int smallestDistance;

    if (velocity > 10000)
        smallestDistance = int(ceil(velocity/10000));
    else if (velocity > 1000)
        smallestDistance = int(ceil(velocity/1000));
    else if (velocity > 100)
        smallestDistance = int(ceil(velocity/100));  
    else if (velocity > 10)
        smallestDistance = int(ceil(velocity/10));
    else
        smallestDistance = int(ceil(velocity));

    return smallestDistance;
}

void BilliardBall::calculateBallsNewValue(Ball &firstBall, Ball &secondBall)
{
    int ydiff = firstBall.yPos - secondBall.yPos;
    int xdiff = firstBall.xPos - secondBall.xPos;

    // theta here is the angle between the centers of 2 balls in degrees
    double theta  = atan2(ydiff, xdiff);

    // derive the new x/y velocity in the rotated coordinate systems
    double v1x = firstBall.velocity * cos(firstBall.degree - theta);
    double v1y = firstBall.velocity * sin(firstBall.degree - theta);
    double v2x = secondBall.velocity * cos(secondBall.degree - theta);
    double v2y = secondBall.velocity * sin(secondBall.degree - theta);

    // derive the final x velocity of each ball
    double final1x = ((v1x * (BALL_MASS - BALL_MASS)) + (2 * BALL_MASS * v2x))/ (2*BALL_MASS);
    double final2x = ((v2x * (BALL_MASS - BALL_MASS)) + (2 * BALL_MASS * v1x))/ (2*BALL_MASS);

    // set the new values of the velocity
    firstBall.velocity = sqrt(2*pow(final1x,2) +  (v1y * pow(v1y, 2)));
    secondBall.velocity = sqrt(2*pow(final2x,2) +  (v2y * pow(v2y, 2)));
    
    // set the new value of the degree
    firstBall.degree = atan2(v1y, final1x) + theta;
    secondBall.degree = atan2(v2y, final2x) + theta;
    cout<<"COLLISION BALL NR: "<<firstBall.id<<" is colliding with "<<secondBall.id<<endl;
}

void BilliardBall::writeToFile()
{
    fstream outputFile;
    outputFile.open("output.txt",ios::out|ios::binary);
    for(int i = 0; i < 10; i++)
    {
        outputFile<<balls[i].xPos<<";"<<balls[i].yPos<<";"<<balls[i].velocity<<";"<<balls[i].degree<<";"<<endl;
    }
    outputFile.close();
}