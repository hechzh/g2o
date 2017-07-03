#include <iostream>
#include "HELLOWORLDCONFIG.h"
#include <math.h>
using namespace std;

int main( int argc, char* argv[])

{
    double a;
    cout<<"Version : "<<HELLOWORLD_VERSION_MAJOR<<"."<<HELLOWORLD_VERSION_MINOR<<endl;
    cout<<"hello, world! "<<endl;
    cin >> a ;
    cout<<"You type in the number "<<a<< endl;
    if (a>=0) {
        cout << "The square root of the number is " << sqrt(a) <<" ."<<endl;
    }else{
        cout << "The input value is invalid . " << endl;
    }
    return 0;

}
