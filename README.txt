*****put the Data.txt and Date.txt file in the SSH software*********


************************command inputting example*********************
client:/sbin/inconfig          // get the ip address

server:g++ server.cpp -o server
server:./server			 //set up the connection
client:g++ client.cpp -o client
client:./client IP

client:ADD YINGYING ZHANG 313-212-3123 // add function
client:ECHO:200 OK
server:200 OK
server:The new Record ID is 1003
client:ADD YINGYING 313-212-3123   //input the error
client:ECHO:invalid command 2

client:DELETE 1003		   //delete the record
client:ECHO:200 OK
client:DELETE 			   //   delete error
client:ECHO:invalid command 2
server:200 OK

client:LIST			   //list the information
client:ECHO:200 OK
server:The list of records in the book:
       1002 Zzz Www 212-121-3212

client:QUIT			  //quit the server
client:ECHO:200 OK
server:200 OK
server:(qiut)			


client:SHUTDOWN			 // shutdown the client
client:ECHO:200 OK 
server:200 OK


