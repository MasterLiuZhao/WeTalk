#include <iostream>
#include <list>
#include <unistd.h>
#include <fcntl.h>
#include <string>
#include <signal.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

using namespace std;

int msgid = 0;
list<int> child_list;
int client_socket_child = 0;

struct Msg{
	long channel_id;
	char txt[200];
};

Msg msg;

void clear_up(int signo){
	
	signal(SIGCHLD, clear_up);

	while(true){
		int pid = waitpid(0, NULL, WNOHANG);
		cout << "clear up process " << pid << endl;
		child_list.remove(pid);
		if(pid == -1 || pid == 0){
			break;
		}
	}
}

void receive_msg_from_child(int signo){
	signal(SIGUSR1, receive_msg_from_child);

	msgrcv(msgid, &msg, sizeof(msg), 1, 0);

	list<int>::iterator it = child_list.begin();
	while(it != child_list.end()){

		cout << "child pid = " << *it << endl;

		msg.channel_id = *it;
		msgsnd(msgid, &msg, sizeof(msg), 0);
		kill(*it, SIGUSR1);

		++it;
	}
}

void receive_msg_from_father(int signo){
	signal(SIGUSR1, receive_msg_from_father);

	msg.channel_id = getpid();

	msgrcv(msgid, &msg, sizeof(msg), getpid(), 0);

	int size = write(client_socket_child, msg.txt, strlen(msg.txt));
	if(size < 0){
		perror("write");
		return;
	}
}

int main(){

	short port = 2222;

	signal(SIGCHLD, clear_up);
	signal(SIGUSR1, receive_msg_from_child);

	msgid = msgget(100, IPC_CREAT | 0700);

	struct msqid_ds msgds;
	msgctl(msgid, IPC_STAT, &msgds);
	msgds.msg_qbytes = 1024000;
	msgctl(msgid, IPC_SET, &msgds);

	child_list.clear();

	int server_socket = socket(AF_INET, SOCK_STREAM, 0);
	if(server_socket < 0){
		perror("socket");
		return -1;
	}

	struct sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);
	server_addr.sin_addr.s_addr = 0;

	int bind_id = bind(server_socket, (sockaddr*)&server_addr, sizeof(server_addr));
	if(bind_id < 0){
		perror("bind");
		return -1;
	}

	int listen_id = listen(server_socket, 50);
	if(listen_id < 0){
		perror("listen");
		return -1;
	}

	while(true){
		
		struct sockaddr_in client_addr;
		socklen_t addr_len = sizeof(client_addr);

		int client_socket = accept(server_socket, (sockaddr*)&client_addr, &addr_len);
		if(client_socket < 0){
			perror("accpet");
			return -1;
		}

		char client_host[200] = {0};
		inet_ntop(AF_INET, &client_addr.sin_addr.s_addr, client_host, 200);

		cout << "accept client IP: " << client_host << endl;
		cout << "              Port:" << ntohs(client_addr.sin_port) << endl;
		cout << "              family::" << client_addr.sin_family << endl;

		// do something

		int pid = fork();

		if(pid < 0){
			perror("fork");
			return -1;
		}else if(pid == 0){

			close(server_socket); // close server socket in child process.

			msgid = msgget(100, IPC_CREAT | 0700);

			signal(SIGUSR1, receive_msg_from_father);

			client_socket_child = client_socket;

			while(true){
				char temp[200];
				int size = read(client_socket, temp, 200);
				if(size < 0){
					perror("read");
					return -1;
				}

				temp[size] = '\0';

				cout << "Client: " << temp << endl;

				char* pch = strchr(temp, '#');

				int index = pch - temp;

				char username[100] = {0};
				char user_message[200] = {0};

				for(int i = 0; i < index; i++){
					username[i] = temp[i];
				}

				for(int i = 0; i < size - index; i++){
					user_message[i] = temp[index + i + 1];
				}

				cout << "User : " << username << endl;
				cout << "User Message : " << user_message << endl;

				string str = username;
				str += " : ";

				if(strcmp(user_message, "exit") == 0){
					str += "has quitted this ChatGroup.";
				}else{
					str += user_message;
				}

				str += '\n';

				msg.channel_id = 1;
				strcpy(msg.txt, str.c_str());

				cout << "msg.channel_id = " << msg.channel_id << endl;
				cout << "msg.txt = " << msg.txt << endl;

				msgsnd(msgid, &msg, sizeof(msg), 0);

				kill(getppid(), SIGUSR1);


				if(strcmp(user_message, "exit") == 0){
					break;
				}

				/*
				size = write(client_socket, str.c_str(), str.size());
				if(size < 0){
					perror("write");
					return -1;
				}
				*/

			}

			close(client_socket);

			cout << "finish process " << getpid() << endl;

			return 0;
		}else{

			child_list.push_back(pid);
			close(client_socket); // close socket from client in parent process.
			continue;

		}

	}

	close(server_socket);
	//shmdt(client_socket_list);

}
