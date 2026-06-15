// This program computes the area of a circle given its radius using the cmath library
#include<iostream>
#include<cmath>
using namespace std; 

int main() 
{ 
  /* example compute the area of a circle with radius r */
  float r, area;

  cout << "Enter the radius of the circle: ";
  cin >> r;
  area = M_PI*pow(r, 2); // M_PI is a constant defined in cmath

  cout << "The area of the circle is: " << area << endl;

  return 0;
}
