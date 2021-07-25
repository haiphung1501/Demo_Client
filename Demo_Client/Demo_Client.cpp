#include "stdafx.h"
#include "Demo_Client.h"
#include "afxsock.h"
#include <string>
#include <fstream>
#include <windows.h>
#include <conio.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// The one and only application object

CWinApp theApp;
using namespace std;

string passwordInput(unsigned maxLength)
{
	string pw;
	for (char c; (c = _getch()); )
	{
		if (c == '\n' || c == '\r') { //Enter
			cout << "\n";
			break;
		}
		else if (c == '\b') { // Backspace
			cout << "\b \b";
			if (!pw.empty()) pw.erase(pw.size() - 1);
		}
		else if (c == -32) { //up down right left
			_getch();
		}
		else if (isprint(c) && pw.size() < maxLength) {
			std::cout << '*';
			pw += c;
		}
	}
	return pw;
}
void login(string& user, string& pass) {
	cout << "Enter Username:";
	getline(cin, user, '\n');

	cout << "Enter password:";
	pass = passwordInput(30);
}

DWORD WINAPI function_cal1(LPVOID arg) {
	SOCKET* hConnected = (SOCKET*)arg;
	CSocket client;
	//Chuyen ve lai CSocket
	client.Attach(*hConnected);
	int number_continue = 0;
	char letter;
	do {
		fflush(stdin);
		int number_a, number_b, number_result;
		char letter;
		printf("Nhap vao phep toan (+, -): ");
		scanf("%c", &letter);

		printf("Nhap vao so thu nhat: ");
		scanf("%d", &number_a);

		printf("Nhap vao so thu hai: ");
		scanf("%d", &number_b);

		//Gui phep toan den server
		client.Send(&letter, sizeof(letter), 0);
		//Gui so thu nhat den server
		client.Send(&number_a, sizeof(number_a), 0);
		//Gui so thu hai den server
		client.Send(&number_b, sizeof(number_b), 0);

		//Nhan ket qua tinh toan tu server
		client.Receive(&number_result, sizeof(number_result), 0);
		printf("Ket qua phep toan %d %c %d=%d\n", number_a, letter, number_b, number_result);

		printf("Nhap 1 de tiep tuc, 0 de thoat: ");
		scanf("%d", &number_continue);
		client.Send(&number_continue, sizeof(number_continue), 0);
	} while (number_continue);

	//Code
	cout << "Goi duoc Function Client";
	//Code

	delete hConnected;
	client.Close();
	return 0;
}
int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	HMODULE hModule = ::GetModuleHandle(NULL);

	if (hModule != NULL)
	{
		// initialize MFC and print and error on failure
		if (!AfxWinInit(hModule, NULL, ::GetCommandLine(), 0))
		{
			// TODO: change error code to suit your needs
			printf("Fatal Error: MFC initialization failed\n");
			nRetCode = 1;
		}
		else
		{
			// TODO: code your application's behavior here.
			AfxSocketInit(NULL);
			CSocket client;
			DWORD threadID;
			HANDLE threadStatus;

			//cout << "Enter server's IP: ";
			char ip[] = "127.0.0.1";
			//cin.getline(ip, 30);
			int port = 1234;
			//cout << "Enter server's port: ";
			//cin >> port;
			//cin.ignore();
			client.Create();
			int number_continue = 0;
			char letter;
			if (client.Connect(CA2W(ip), port)) {
				cout << "Succesfully Connected!" << endl;

				SOCKET* hConnected = new SOCKET();
				*hConnected = client.Detach();
				threadStatus = CreateThread(NULL, 0, function_cal1, hConnected, 0, &threadID);

				client.Attach(*hConnected);

				while (1) {
					DWORD dwWait = WaitForSingleObject(threadStatus, 0); // timeout right away if thread handle is not signalled
					if (dwWait == WAIT_OBJECT_0)
						exit(0);
				}
			}
			else
				cout << "Ket noi khong thanh cong" << endl;
		}
	}
	else
	{
		// TODO: change error code to suit your needs
		_tprintf(_T("Fatal Error: GetModuleHandle failed\n"));
		nRetCode = 1;
	}

	return nRetCode;
}