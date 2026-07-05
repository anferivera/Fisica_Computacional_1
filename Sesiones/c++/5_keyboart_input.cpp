// This program demonstrates how to take input from the keyboard in C++ and perform a simple addition operation.
#include<iostream>
using namespace std; 

int main() 
{ 
  int x;
  float y,z;
  cout << "Enter the first integer: ";
  cin >> x;
  cout << "Enter the second integer: ";
  cin >> y;
  z = x + y;
  cout << "The sum of " << x << " and " << y << " is " << z << endl;

  return 0;
}
