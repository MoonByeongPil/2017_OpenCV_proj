#include <iostream>

using namespace std;
int main(int argc, char *argv[]) {
	if (argc == 2) 
	{
		if (*argv[1] == '1') cout << "c로부터 1이 왔습니다." << endl;

		else if (*argv[1] == '0') cout << "c로부터 0이 왔습니다." << endl;
	}
	return 0;

}
