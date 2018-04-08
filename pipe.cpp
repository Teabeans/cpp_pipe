//-------|---------|---------|---------|---------|---------|---------|---------|
//
// Pipe Experiment
// pipe.cpp
//
//-------|---------|---------|---------|---------|---------|---------|---------|

//-----------------------------------------------------------------------------|
// Authorship
//-----------------------------------------------------------------------------|
//
// Tim Lum
// twhlum@gmail.com
// Created:  2018.04.07
// Modified: 2018.04.08
//

//-----------------------------------------------------------------------------|
// File Description
//-----------------------------------------------------------------------------|
//
// This file is an experiment with the fork() and pipe() methods in C++
//

//-----------------------------------------------------------------------------|
// Package Files
//-----------------------------------------------------------------------------|
//
// None
//

// Necessary for I/O operations
#include <iostream>

// Necessary for deque operations
#include <deque>

// Necessary for queue operations
#include <queue>

// Necessary for string operations
#include <string>

// Necessary for int to string conversions via streams
#include <sstream>

// Necessary for gettimeofday() operations
#include <sys/time.h>

// Necessary for Squareroot and Power functions
#include <math.h>

// Necessary for fork() and wait() operations
#include <sys/types.h>

// Necessary for wait() operations
#include <sys/wait.h>

// Necessary for fork() operations
#include <unistd.h>

// Additional includes per assignment:

// Necessary for fork() and wait()
#include <sys/types.h>

// Necessary for wait()
#include <sys/wait.h>

// Necessary for fork() and pipe()
#include <unistd.h>

// Necessary for exit()
#include <stdlib.h>

// Necessary for perror()
#include <stdio.h>

using namespace std; 

// Point class included for testing purposes
class Point {
   public:
   int id;
   double x, y;          // x and y coordinates
   Point( ) {
      x = 0.0;
      y = 0.0;
      id = 0;
   } // Closing Point()
   Point( double initX, double initY, int id ) {
      this->x = initX;
      this->y = initY;
      this->id = id;
   }
   void print( ) {
      printf( "x = %f .y = %f\n", x, y );
   } // Closing print()
}; // Closing Point{}

// (+) --------------------------------|
// #send()
// C. Univ. of Wash. Bothell 2018 -----|
// Desc:    Sends a deque to a destination process through a pipe File Descriptor (FD)
// Params:  int arg1 - The file descriptor of a pipe
//          deque<Point> arg2 - The deque to send
// PreCons: NULL
// PosCons: NULL
// RetVal:  None
// MetCall: NULL
void send( int fd, deque<Point> &q ) {
   cout << "send(): FD == " << fd << endl;
   int size = q.size();
   double x  [ size ];
   double y  [ size ];
   int    id [ size ];

   // Serialize all deque items to x, y, and id arrays
   cout << "send() - Serializing..." << endl;
   for ( int i = 0 ; i < size ; i++ ) {
      Point p = q.front();
      x[i] = p.x;
      y[i] = p.y;
      id[i] = p.id;
      q.pop_front();
   }

   // Send all data through a pipe
   cout << "send() - Writing..." << endl;
   write( fd, &size, sizeof(int));
   write( fd, x, sizeof( double ) * size );
   write( fd, y, sizeof( double ) * size );
   write( fd, id, sizeof( int ) * size );
} // Closing send()

// (+) --------------------------------|
// #recv()
// C. Univ. of Wash. Bothell 2018 -----|
// Desc:    Receives a deque from a source process through a pipe
// Params:  int arg1 - The file descriptor of a pipe
//          deque<Point> arg2 - The deque to load with received data
// PreCons: NULL
// PosCons: NULL
// RetVal:  NULL
// MetCall: NULL
void recv( int fd, deque<Point> &q ) {
   // Receive all data through a pipe
   int size = 0;

   // read (File descriptor, target variable, bit size of packet)
   read(fd, &size, sizeof(int));
   double x[size];
   double y[size];
   int id[size];
   read(fd, x, sizeof(double)*size);
   read(fd, y, sizeof(double)*size);
   read(fd, id, sizeof(int)*size);

   // de-serialized x, y, and id arrays to all deque items
   for (int i = 0 ; i < size ; i++) {
      Point *p = new Point( x[i], y[i], id[i]);
      q.push_back(*p);
   }
} // Closing recv()


// (+) --------------------------------|
// #pipe_test()
//-------------------------------------|
// Desc:    NULL
// Params:  NULL
// PreCons: NULL
// PosCons: NULL
// RetVal:  NULL
// MetCall: NULL
deque<Point> pipe_test() {
   cout << "Enumerating..." << endl;
   enum channel {RECV_L = 3, SEND_L = 4, RECV_R = 5, SEND_R = 6};
   cout << "Enum SEND_L   : " << SEND_L << endl;
   cout << "Enum RECEIVE: " << RECV_L << endl;

   // Generate pipe1
   int pipe1[2];
   pipe(pipe1);

   deque<Point> lResult;

   // Fork a left child
   cout << "Forking Left..." << endl;
   int lPID = fork();

   // And if we're in the child process
   if (lPID == 0 ) {
      cout << "(L) Created! ( " << getpid() << " )" << endl;

      // Close reception channel
      cout << "(L) Closing pipe1[RECV_L]" << endl;
      close (pipe1[RECV_L]);

      // Make a sample deque
      cout << "(L) Sample deque..." << endl;
      Point* samplePtr = new Point;
      deque<Point> sampleDeckL;
      sampleDeckL.push_back(*samplePtr);

      // Send the packet
      cout << "(L) Sending deque..." << endl;
      send(SEND_L, sampleDeckL);

      // Close this process, its work is complete
      cout << "(L) Closing." << endl;
      exit(0);
   }

   // But if we're in the parent process...
   else {
      // Close sending
      cout << "(CTRL) Closing pipe1[SEND_L]" << endl;
      close (pipe1[SEND_L]);

      // Receive the packet
      cout << "(CTRL) Receiving..." << endl;
      recv(RECV_L, lResult);
      cout << "(CTRL) Received!" << endl;
   }

   // Generate pipe2
   int pipe2[2];
   pipe(pipe2);

   deque<Point> rResult;

   // Fork a right child
   cout << "Forking Right..." << endl;
   int rPID = fork();

   // And if we're in the child process
   if (rPID == 0 ) {
      cout << "(R) Created! ( " << getpid() << " )" << endl;

      // Close reception channel
      cout << "(R) Closing pipe1[RECV_R]" << endl;
      close (pipe2[RECV_R]);

      // Make a sample deque
      cout << "(R) Sample deque..." << endl;
      Point* samplePtr = new Point(1, 1, 1);
      deque<Point> sampleDeckR;
      sampleDeckR.push_back(*samplePtr);

      // Send the packet
      cout << "(R) Sending deque..." << endl;
      send(SEND_R, sampleDeckR);

      // Close this process, its work is complete
      cout << "(R) Closing." << endl;
      exit(0);
   }
   // But if we're in the parent process...
   else {
      // Close sending
      cout << "(CTRL) Closing pipe1[1] (R)" << endl;
      close (pipe2[SEND_R]);

      // Receive the packet
      cout << "(CTRL) Receiving (R)..." << endl;
      recv(RECV_R, rResult);
      cout << "(CTRL) Received (R)!" << endl;
   }

   cout << "(CTRL) Waiting..." << endl;
   wait(&lPID);
   cout << "(CTRL) Closed: " << lPID << endl;
   wait(&rPID);
   cout << "(CTRL) Closed: " << rPID << endl;

   cout << "Children closed (L & R)." << endl;
   deque<Point> results;
   results.push_back(lResult.front());
   results.push_back(rResult.front());
   return(results);

} // Closing pipe_test()



int main( int argc, char* argv[] ) {

   deque<Point> resultDQ = pipe_test();

   cout << "Results:" << endl;
   resultDQ.at(0).print();
   resultDQ.at(1).print();

   return 0;
} // Closing main()
