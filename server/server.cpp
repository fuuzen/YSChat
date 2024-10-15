#include <iostream>
#include <vector>
#include <stdlib.h>
#include <winsock2.h>
#include <time.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unordered_map>
#include <fstream>
#include <string>
#include <thread>

using namespace std;

# define QLEN  5
# define WSVERS  MAKEWORD (2, 0)

void server (SOCKET, unordered_map<string, string> &);
SOCKET passiveTCP (const char *, int);
void errexit (const char * format, ...);
void broadCast(SOCKET, char *, unsigned long long, string user);
bool reveiceAndCheck(SOCKET, char *, unsigned long long, string);

char * port1 = "11451";      /* send mseeage to this port */
char * port2 = "14514";      /* receive mseeage from this port */
char * filename = "users.db";  /* 不是 sqlite 只是文本存储数据（反正只是读数据）*/
vector<SOCKET> slaves;

int main (int argc, char * argv []) {
	struct sockaddr_in fsin;   /* the from address of a client */
	SOCKET msock, ssock;       /* master & slave sockets*/
	int alen;                  /* from-address length */
	WSADATA wsadata;
	int user_count = 0;

	switch (argc) {
	case 1:
		break;
	case 4:
		port2 = argv[3];
	case 3:
		port1 = argv[2];
	case 2:
		strcpy(filename, argv[1]);
		break;
	default:
		errexit("usage: server.exe [filename [port1] [port2]]\n");
	}

	ifstream file("users.db");
	if (!file.is_open())
        errexit("File open failed\n");
	string username, password;
	unordered_map<string, string> users;
	while (getline(file, username)) {
        getline(file, password);
		users[username] = password;
    }

	if (WSAStartup (WSVERS, &wsadata) != 0)
		errexit("WSAStartup failed\n");

	msock = passiveTCP(port1, QLEN);

	while (1) {
		alen = sizeof (struct sockaddr);
		ssock = accept (msock, (struct sockaddr * ) &fsin, &alen);
		if (ssock == INVALID_SOCKET)
			errexit("accept failed: %d\n", GetLastError ());
		slaves.push_back(ssock);
		thread(server, ssock, ref(users)).detach();   // slave socket 复制传值，这样一个用户关闭连接不会影响其他 slave socket
	}
	
	closesocket(ssock);
	WSACleanup();
	return 0;
}


void server (SOCKET s,  unordered_map<string, string> & users) {
	char loginState = 0;
	string user;
	int bytesReceived;

	while(!loginState){
		unsigned long long l_username, l_password;
		if(reveiceAndCheck(s, (char*)&l_username, 8, user))return;
		char username[l_username];
		if(reveiceAndCheck(s, username, l_username, user))return;
		if(reveiceAndCheck(s, (char*)&l_password, 8, user))return;
		char password[l_password];
		if(reveiceAndCheck(s, password, l_password, user))return;
		if(users[username] == password){
			loginState = 1;
			user = username;
		}
		send (s, (char *)&loginState, 1, 0);
	}

	printf("%s login success at socket %lld.\n", user.c_str(), s);

	unsigned long long l;
	while (1) {
        if(reveiceAndCheck(s, (char *)&l, 8, user))return;
		char message[l];
		if(reveiceAndCheck(s, message, l, user))return;
		printf("%s sent message: %s\n", user.c_str(), message);
		broadCast(s, message, l, user);
    }
}


void broadCast(SOCKET sender, char * message, unsigned long long l, string user){
	unsigned long long l_user = sizeof(user);
	for(auto s: slaves){
		send(s, (char*)&l_user, 8, 0);
		send(s, user.c_str(), l_user, 0);
		send(s, (char*)&l, 8, 0);
		send(s, message, l, 0);
	}
}


bool reveiceAndCheck(SOCKET s, char * buf, unsigned long long len, string user){
	if(recv(s, buf, len,0) <= 0){
		printf("%s exit from socket %lld.\n", user.c_str(), s);
		return true;
	}
	return false;
}


void errexit (const char * format, ...){
	va_list args;
	va_start (args, format);
	vfprintf (stderr, format, args);
	va_end (args);
	exit (1);
}


SOCKET passiveTCP (const char *service, int qlen){
	/* Allocate a socket */
    SOCKET socketfd = socket (PF_INET, SOCK_STREAM, 0);
    if ( socketfd == INVALID_SOCKET)
		errexit("can't create socket: %d\n", GetLastError ());
    /* Bind the socket */
	unsigned short port = (unsigned short)atoi(service);
    if (port == 0)
		errexit("Error: port not valid\n", service);
	struct sockaddr_in sin;
	memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = INADDR_ANY;
    sin.sin_port = htons(port);
    if (bind (socketfd, (struct sockaddr *) &sin, sizeof (sin)) == SOCKET_ERROR)
		errexit("can't bind to %s port: %d\n", service, GetLastError());
    if (listen (socketfd, qlen) == SOCKET_ERROR)
		errexit("can't bind to %s port: %d\n", service, GetLastError());
    return socketfd;
}