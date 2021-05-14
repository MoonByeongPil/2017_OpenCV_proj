#include <iostream.h> 

2 using namespace std; 

3 int main(int argc,char *argv[]) 

4 { 

5    if(argc==2) 

6    { 

7       if(*argv[1]=='1') cout << "c로부터 1이 왔습니다." << endl; 

8       else if(*argv[1]=='0') cout << "c로부터 0이 왔습니다." << endl; 

9    } 

10    return 0; 

11 } 


