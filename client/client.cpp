#include <iostream>
#include <stdlib.h>
#include <winsock2.h>
#include <string>
#include <thread>
#include <atomic>
#include <condition_variable>

using namespace std;

#define BUFFER_SIZE 1024
#define WSVERS   MAKEWORD (2, 0)

void client (const char * , const char * );
SOCKET connectTCP (const char *, const char * );
void errexit (const char * format, ...);
void receiveMessage(SOCKET);
bool reveiceAndCheck(SOCKET, char *, unsigned long long);

atomic<bool> receiving(true);
condition_variable cv;
mutex mtx;

char * host = "127.0.0.1";  /* host to use if none supplied */
char * port = "11451";      /* port */


int main (int argc, char * argv[]){
	WSADATA wsadata;
	switch (argc) {
		case 1:
			break;
		case 3:
			port = argv[2];
		case 2:
			host = argv[1];
			break;
		default:
			errexit("usage: client.exe [host [port]]\n");
	}

	if (WSAStartup (WSVERS, &wsadata) != 0)
		errexit("WSAStartup failed\n");
	client (host, port) ;
	WSACleanup ();
	printf("\033[36m退出程序...\n\033[0m");
	return 0;
}


void client (const char * host, const char * service){
	SOCKET s;
	printf("\033[36m欢迎来到 \033[33mYSChat\033[36m 平台~\033[0m\n");
	s = connectTCP (host, service);
	char loginState = 0;
	unsigned long long l_username, l_password;
	string username, password;
	while(!loginState){
		printf("\033[36m请输入用户名:\033[0m\n");
		getline(cin, username);
		printf("\033[36m请输入密码:\033[0m\n");
		getline(cin, password);
		l_username = sizeof(username);
		send(s, (char*)&l_username, 8, 0);
		send(s, username.c_str(), l_username, 0);
		l_password = sizeof(password);
		send(s, (char*)&l_password, 8, 0);
		send(s, password.c_str(), l_password, 0);
		if(reveiceAndCheck(s, (char *)&loginState, 1))return;
		if(!loginState) printf("\033[31m用户名或密码错误!\033[0m\n");
	}
	printf("\033[36m登陆成功~ 输入[exit]退出程序...\033[0m\n");
	thread r(receiveMessage, s);
	unsigned long long l;
	string input;
	while (true) {
		getline(cin, input);
		if(input == "exit") break;
		l = input.size();
		send(s, (char *)&l, 8, 0);
		send(s, input.c_str(), input.size(), 0);
    }{
		lock_guard<mutex> lock(mtx);
		receiving = false;
	}
	cv.notify_all();
	closesocket (s);
	r.join();
}


void receiveMessage(SOCKET s){
	unsigned long long l, l_user;
	string input;
	while (1) {
		{
			unique_lock<mutex> lock(mtx);
			if (!receiving) break;
		}
		if(reveiceAndCheck(s, (char *)&l, 8))return;
		char user[l_user];
		if(reveiceAndCheck(s, user, l))return;
		if(reveiceAndCheck(s, (char *)&l, 8))return;
		char message[l];
		if(reveiceAndCheck(s, message, l))return;
		printf("%s : %s\n", user, message);
	}
}


void errexit (const char * format, ...){
	va_list args;
	va_start (args, format);
	vfprintf (stderr, format, args);
	va_end (args);
	exit (1);
}


bool reveiceAndCheck(SOCKET s, char * buf, unsigned long long len){
	if(recv(s, buf, len,0) <= 0){
		return true;
	}
	return false;
}


SOCKET connectTCP (const char *host, const char *service){
	/* Allocate a socket */
    SOCKET socketfd = socket (PF_INET, SOCK_STREAM, 0);
    if ( socketfd == INVALID_SOCKET)
		errexit("can't create socket : %d\n", GetLastError ());
    /* Bind the socket */
	unsigned short port = (unsigned short)atoi(service);
    if (port == 0)
		errexit("Error: port not valid\n", service);
	struct sockaddr_in sin;
	memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_port = htons(port);
    sin.sin_addr.s_addr = inet_addr(host);

    if (connect(socketfd, (struct sockaddr *)&sin, sizeof(sin)) == SOCKET_ERROR) {
        printf("Connect failed: %d\n", WSAGetLastError());
        closesocket(socketfd);
        WSACleanup();
        return 1;
    }

    return socketfd;
}
