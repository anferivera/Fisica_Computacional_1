 
#include<iostream>
#include<cmath>
using namespace std; 

int main() 
{ // loop for as a ilustration in C++
    for (int i = 0; i < 10; i++) 
    { /* code */
        cout << "i = " << i << endl; 
    }

  int factorial = 1;
  int n;

  // This program calculates the factorial of a given integer n using a for loop.
  cout << "Calculating n!..." << endl;
  cout << "Enter the number n integer: " << endl;
  cin >> n;

  for (int i = 1; i <= n; i++)
  {    factorial *= i; 
  }
  cout << n << "! = " << factorial << endl;

  // For n=18 it faulds because the result is too large to be stored in an int variable, it exceeds the maximum value that an int can hold (2,147,483,647). The factorial of 18 is 6,402,373,705,728,000 which is much larger than the maximum value of an int. To handle larger factorials, you can use a larger data type such as long long or even a library for arbitrary-precision arithmetic like GMP.
  // change to float or double to handle larger numbers, but keep in mind that it may not be accurate for very large factorials due to precision limitations of floating-point representation. For n=18, using double would work, but for n=20 or higher, you may start to see inaccuracies. For very large factorials, consider using a library like GMP which can handle arbitrary-precision integers.
  return 0;
}
