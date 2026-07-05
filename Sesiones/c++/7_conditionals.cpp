// if-else statement example: Determine if a number is positive, negative, or zero. 
#include<iostream>
#include<cmath>
using namespace std; 

int main() 
{ //number positive or negative
    double x;
    cout << "Enter a number: ";
    cin >> x;

    if (x > 0) {
        cout << "The number is positive." << endl;
    } 
    else if (x < 0) {
        cout << "The number is negative." << endl;
    } 
    else {
        cout << "The number is zero or a character." << endl;
    }

  return 0;
}
