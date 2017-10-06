#include <iostream>
#include <process.h>
#include <Windows.h>

using namespace std;

void callBack(void * userdata)
{
	cout << "callBack executing... userdata=" << *((int*)userdata) << endl;
	cout << endl;

}

void callBack2(void * userdata)
{
	cout << "callBack executing2... userdata=" << *((int*)userdata) << endl;
}

int main()
{
	int arg = 10;
	int arg2 = 15;
	_beginthread(callBack, 0, (void *)&arg);
	Sleep(500);
	_beginthread(callBack2, 0, (void *)&arg2);
	
	while (1){
		Sleep(10);
	}

	return 0;
}
