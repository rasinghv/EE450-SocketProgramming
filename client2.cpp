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

#define MYPORT 21503
#define MAXBUFLEN 100

int main(int argc, char *argv[]){
	map<string,string> mySearchterms;

	//Reading file
	string line;
   	ifstream myfile ("client2.txt");
   	//cout << "Available Search Terms : " ;
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
		    mySearchterms[word[0]] = word[1];
     		//cout << word[0] << ", ";
    	}
    	myfile.close();
  	}
  	else{
  		cout << "Unable to open file";
  		return 0;
  	}

  	string sTerm, searchKey;

  	cout << "Please Enter Your Search (USC, UCLA etc.): ";  	
  	getline(cin,sTerm);

  	if(mySearchterms.find(sTerm) == mySearchterms.end()){
  		cout << "Error: Search Term Not Found In the List." << endl;
  		return 0;
  	} 
  	else{
  		searchKey = mySearchterms[sTerm];
  		cout << "The Client 2 has received a request with search word " << sTerm << ", which maps to key " << searchKey << "." << endl;
  	}

  	string myRequestString = "GET " + searchKey;

  	//UDP socket Start
	int sockfd;
	struct sockaddr_in their_addr; // connector’s address information
	struct hostent *he;
	int numbytes;
	char buf[MAXBUFLEN];

	if ((he=gethostbyname("localhost")) == NULL) { // get the host info
		perror("gethostbyname");
		exit(1);
	}
	struct in_addr **addr_list = (struct in_addr **)he->h_addr_list;
	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
		perror("socket");
		exit(1);
	}

	their_addr.sin_family = AF_INET; // host byte order
	their_addr.sin_port = htons(MYPORT); // short, network byte order
	their_addr.sin_addr = *((struct in_addr *)he->h_addr);
	memset(&(their_addr.sin_zero), '\0', 8); // zero the rest of the struct

	socklen_t addr_len = sizeof(struct sockaddr);

	if ((numbytes=sendto(sockfd, myRequestString.c_str(), strlen(myRequestString.c_str()), 0,(struct sockaddr *)&their_addr, sizeof(struct sockaddr))) == -1) {
		perror("sendto");
		exit(1);
	}

	struct sockaddr_in mysin;
	socklen_t mylen = sizeof(mysin);
	if (getsockname(sockfd, (struct sockaddr *)&mysin, &mylen) == -1){
	    perror("getsockname");
	    exit(1);
	}

	cout << "The Client 2 sends the request " << myRequestString << " to the Server 1 with port number " << ntohs(their_addr.sin_port);
	cout << " and IP address " << inet_ntoa(their_addr.sin_addr) << "." << endl;

	cout << "The Client2’s port number is " << ntohs(mysin.sin_port);
	cout << " and the IP address is " << inet_ntoa(*addr_list[0]) << "." << endl;

	if ((numbytes=recvfrom(sockfd,buf, MAXBUFLEN-1, 0, (struct sockaddr *)&their_addr, &addr_len)) == -1) {
		perror("recvfrom");
		exit(1);
	}
	buf[numbytes] = '\0';

	cout << "The Client 2 received the value " << buf << " from the Server 1 with port number " << ntohs(their_addr.sin_port);
	cout << " and IP address " << inet_ntoa(their_addr.sin_addr) << "." << endl;

	cout << "The Client2’s port number is " << ntohs(mysin.sin_port);
	cout << " and the IP address is " << inet_ntoa(*addr_list[0]) << "." << endl;

	string value(buf);
	value = value.substr(5);
	cout << "The requested value is " << value << "." << endl;
 
	close(sockfd);
	return 0;
}