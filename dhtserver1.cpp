#include <iostream>
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <map>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

using namespace std;

#define MYPORT 21503 // the port users will be connecting to
#define MAXBUFLEN 100

string recurseRequest(string);

int main(){
	map<string,string> myDictionary;
	//Reading file

	string line;
   	ifstream myfile ("server1.txt");
  	if (myfile.is_open()){
   		while ( getline (myfile,line) ){
   			
   			istringstream wordIterator(line);
 			string word[2];
 			int i = 0;
		    while (wordIterator) {
		        wordIterator >> word[i];
		        i++;
		        //cout << word << endl;
		    }
		    myDictionary[word[0]] = word[1];
     		//cout << line << endl;
    	}
    	myfile.close();
  	}
  	else{
  		cout << "Unable to open file";
  		return 0;
  	}

	//UDP Server Start
	int sockfd;
	struct sockaddr_in my_addr; // my address information
	struct sockaddr_in their_addr; // connector’s address information
	socklen_t addr_len, numbytes;
	char buf[MAXBUFLEN];

	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
		perror("socket");
		exit(1);
	}

	my_addr.sin_family = AF_INET; // host byte order
	my_addr.sin_port = htons(MYPORT); // short, network byte order
	my_addr.sin_addr.s_addr = INADDR_ANY; // automatically fill with my IP

	memset(&(my_addr.sin_zero), '\0', 8); // zero the rest of the struct

	if (bind(sockfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr)) == -1) {
		perror("bind");
		exit(1);
	}

	addr_len = sizeof(struct sockaddr);

	struct hostent *he;
	if ((he=gethostbyname("localhost")) == NULL) { // get the host info
		perror("gethostbyname");
		//return "";
	}
	struct in_addr **addr_list = (struct in_addr **)he->h_addr_list;
	cout << "The Server 1 has UDP port number " << ntohs(my_addr.sin_port);
	cout << " and IP address " << inet_ntoa(*addr_list[0]) << "." << endl;
	int clientCount = 0;
	while(1){
		if ((numbytes=recvfrom(sockfd,buf, MAXBUFLEN-1, 0, (struct sockaddr *)&their_addr, &addr_len)) == -1) {
			perror("recvfrom");
			exit(1);
		}
		buf[numbytes] = '\0';

		clientCount++;

		string receivedRequest(buf);

		string word[2];
		istringstream wordIterator(receivedRequest);
	 	int i = 0;
		while (wordIterator) {
		    wordIterator >> word[i];
		    i++;
		    //cout << word << endl;
		}
		string receivedKey = word[1];

		cout << "The Server 1 has received a request with key " << receivedRequest;
		cout << " from client " << clientCount << " with port number " << ntohs(their_addr.sin_port);
		cout << " and IP address " << inet_ntoa(their_addr.sin_addr) << "." << endl;

		//Find key in my own dictionary
		string finalValue;
		if(myDictionary.find(receivedKey) != myDictionary.end()){
			finalValue = myDictionary[receivedKey];
			finalValue = "POST " + finalValue;
		}
		else{
			finalValue = recurseRequest(receivedRequest);
			myDictionary[receivedKey] = finalValue;
		}

		if ((numbytes=sendto(sockfd, finalValue.c_str(), strlen(finalValue.c_str()), 0,(struct sockaddr *)&their_addr, sizeof(struct sockaddr))) == -1) {
			perror("sendto");
			exit(1);
		}

		cout << "The Server 1, sent reply " << finalValue;
		cout << " to client " << clientCount << " with port number " << ntohs(their_addr.sin_port);
		cout << " and IP address " << inet_ntoa(their_addr.sin_addr) << "." << endl << endl;
	}

	close(sockfd);
	return 0;
}

string recurseRequest(string request){
	#define PORT 22503 // the port client will be connecting to
	#define MAXDATASIZE 100 // max number of bytes we can get at once

	int sockfd, numbytes;
	char buf[MAXDATASIZE];
	struct hostent *he;

	struct sockaddr_in their_addr; // connector’s address information
	
	if ((he=gethostbyname("localhost")) == NULL) { // get the host info
		perror("gethostbyname");
		return "";
	}
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("socket");
		return "";
	}

	their_addr.sin_family = AF_INET; // host byte order
	their_addr.sin_port = htons(PORT); // short, network byte order
	their_addr.sin_addr = *((struct in_addr *)he->h_addr);
	memset(&(their_addr.sin_zero), '\0', 8); // zero the rest of the struct

	if (connect(sockfd, (struct sockaddr *)&their_addr,
		sizeof(struct sockaddr)) == -1) {
		perror("connect");
		return "";
	}

	struct sockaddr_in mysin;
	socklen_t mylen = sizeof(mysin);
	if (getsockname(sockfd, (struct sockaddr *)&mysin, &mylen) == -1){
	    perror("getsockname");
	    exit(1);
	}

	write(sockfd, request.c_str(), strlen(request.c_str()));
	cout << "The Server 1 sends the request " << request << " to the Server 2.";
	cout << "The TCP port number is " << ntohs(mysin.sin_port) << " and the IP address is " << inet_ntoa(mysin.sin_addr) << "." << endl;

	if ((numbytes=recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
		perror("recv");
		return "";
	}
	buf[numbytes] = '\0';

	cout << "The Server 1 received the value " << buf << " from the Server 2 with port number " << ntohs(their_addr.sin_port);
	cout << " and IP address " << inet_ntoa(their_addr.sin_addr) << "." << endl;

	printf("Received: %s\n",buf);
	close(sockfd);

	cout << "The Server 1 closed the TCP connection with the Server 2." << endl;

	string result(buf);
    return result;
}