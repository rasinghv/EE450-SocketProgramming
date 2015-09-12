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

#define MYPORT 22503 // the port users will be connecting to
#define MAXBUFLEN 100

string recurseRequest(string);

int main(){
	//Reading from file
	map<string,string> myDictionary;
	//Reading file
	string line;
   	ifstream myfile ("server2.txt");
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

  	int listenfd = 0, connfd = 0;
    struct sockaddr_in serv_addr; 

    char sendBuff[1025];
    char recvBuff[1024];
    int n;
    time_t ticks; 

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&serv_addr, '0', sizeof(serv_addr));
    memset(sendBuff, '0', sizeof(sendBuff)); 

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(MYPORT); 

    bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)); 

    listen(listenfd, 10); 

    struct hostent *he;
	if ((he=gethostbyname("localhost")) == NULL) { // get the host info
		perror("gethostbyname");
		
	}
	struct in_addr **addr_list = (struct in_addr **)he->h_addr_list;

    cout << "The Server 2 has TCP port number " << ntohs(serv_addr.sin_port);
	cout << " and IP address " << inet_ntoa(*addr_list[0]) << "." << endl;

    while(1)
    {
        connfd = accept(listenfd, (struct sockaddr*)NULL, NULL); 

        struct sockaddr_in connsin;
		socklen_t connlen = sizeof(connsin);
		if (getsockname(connfd, (struct sockaddr *)&connsin, &connlen) == -1){
		    perror("getsockname");
		    exit(1);
		}

        if ( (n = read(connfd, recvBuff, sizeof(recvBuff)-1)) < 0){
	        printf("\n Read error \n");
	    } 
	    recvBuff[n] = 0;
	    string receivedRequest(recvBuff);

	    //cout << "received : " << recvBuff << endl;
	    cout << "The Server 2 has received a request with key " << receivedRequest;
		cout << " from Server 1 with port number " << ntohs(connsin.sin_port);
		cout << " and IP address " << inet_ntoa(connsin.sin_addr) << "." << endl;

	    
		string word[2];
		istringstream wordIterator(receivedRequest);
	 	int i = 0;
		while (wordIterator) {
		    wordIterator >> word[i];
		    i++;
		    //cout << word << endl;
		}
		string receivedKey = word[1];
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

        write(connfd, finalValue.c_str(), strlen(finalValue.c_str())); 

        cout << "The Server 2, sent reply " << finalValue;
		cout << " to Server 1 with port number " << ntohs(connsin.sin_port);
		cout << " and IP address " << inet_ntoa(connsin.sin_addr) << "." << endl << endl;

        close(connfd);
        sleep(1);
     }

	return 0;
}

string recurseRequest(string request){
	#define PORT 23503 // the port client will be connecting to
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
	cout << "The Server 2 sends the request " << request << " to the Server 3.";
	cout << "The TCP port number is " << ntohs(mysin.sin_port) << " and the IP address is " << inet_ntoa(mysin.sin_addr) << "." << endl;

	if ((numbytes=recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
		perror("recv");
		return "";
	}
	buf[numbytes] = '\0';

	cout << "The Server 2 received the value " << buf << " from the Server 3 with port number " << ntohs(their_addr.sin_port);
	cout << " and IP address " << inet_ntoa(their_addr.sin_addr) << "." << endl;

	printf("Received: %s\n",buf);
	close(sockfd);

	cout << "The Server 2 closed the TCP connection with the Server 3." << endl;

	string result(buf);
    return result;
}
