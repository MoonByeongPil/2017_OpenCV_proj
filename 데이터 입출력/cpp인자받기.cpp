#include <iostream>

using namespace std;
int main(int argc, char *argv[]) {
	if (argc == 2) 
	{
		if (*argv[1] == '1') cout << "c�κ��� 1�� �Խ��ϴ�." << endl;

		else if (*argv[1] == '0') cout << "c�κ��� 0�� �Խ��ϴ�." << endl;
	}
	return 0;

}
