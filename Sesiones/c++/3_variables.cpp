#include<iostream>
using namespace std;

int main()
{ 
    int a = 10; // one byte = 8 bits, range: -128 to 127
    float b = 3.14; // four bytes, range: approximately ±3.4 × 10^38
    double c = 3.14; // eight bytes, range: approximately ±1.7 × 10^308
    char d = 'A'; // one byte, range: 0 to 255
    bool e = true; // one byte, values: true or false

    cout << "Integer: " << a << endl; //endl is used to end the line and flush the output buffer
    cout << "Float: " << b << endl;
    cout << "Double: " << c << endl;
    cout << "Character: " << d << endl;
    cout << "Boolean: " << e << endl;

  return 0;
}
