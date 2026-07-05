#include<iostream>
using namespace std;

int main()
{ 
int a = 10, b = 20;
double c = 3.14;

cout << " a = " << a << "\n b = " << b << "\n c = " << c << "\n";

cout <<  "Result of a + b = " << a + b <<"\n";
cout << "Result of a / b = " << a / b << "\n"; //error, the result is 0 because a and b are integers, we need to cast one of them to double
cout << "Result of a / c = " << a / c << "\n";


//increment and decrement operators
a++; //increment a by 1
cout << "\nAfter incrementing a: " << a << endl; //endl is used to end the line jump to the next line and flush the output buffer
a--; //decrement a by 1
cout << "After decrementing a: " << a << endl;


//other for to identify variables
int x,y,z;
x = 1;
y = 2;
z = x + y; //assignment operator, assigns the value of x + y to z
cout << "\nThe value of z is: " << z << endl;


  return 0;
}
