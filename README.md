This project is a simulation of a distributed hash table which is a set of key: value pairs. 
The key: value pairs are distributed among three servers i.e. dhtserver1, dhtserver2 & dhtserver3.
There are two clients who will contact the designated server which is dhtserver1 with a key in order to retrieve its corresponding value. 
The designated server will directly reply with the value if it is stored in its memory. 
Otherwise, it will contact other servers in a manner similar to recursive DNS.
Once the designated server will retrieve the value, it stores a copy of the pair key: value in its hash table and will send back the result to the client.
The servers communicate with each other using TCP sockets. The clients communicate with the designated server using Bidirectional UDP sockets.
