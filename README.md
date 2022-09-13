# CMakeTestTask

Learning CMake and MultiThreading in C++

This project consists of two programms:
- Sender
- Handler

They are connected via socket. And both of programms can be run on Linux and Windows.

## Sender
Sender consits of two threads. First one is waiting for user to input line of not more than 64 numbers.
After input it checks if line are met requirements. If it does, all even numbers will be replaced with 'RnD'.
Proccessed line is put in shared buffer.
The second thread reads shared buffer and clears it. Thread prints received data and calculates sum of numbers in the line. After that it sends data to the Handler program.

## Handler
Receives data from the Seneder and checks if it length is more than 2 and sum is multiple of 32. If data met the requirements, then the Handler will print message about receiving, else it will print that data is incorrect.
