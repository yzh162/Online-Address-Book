/*
 * server.cpp
 */
#include <stdio.h>
#include <iostream>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <cstring>
#include <cstdlib>
#include <sstream>
#include <vector>
#include <fstream>

using namespace std;



#define SERVER_PORT 5432
#define MAX_PENDING 5
#define MAX_LINE 256
#define RECORD_START 1000

bool FirstName(string &firstName) // check whether first name input is correct or not
{
	bool check = true;
	for(int i=0;i<firstName.length();i++)
	{
		if(firstName[i]>'z' || firstName[i]>'Z' && firstName[i]<'a' || firstName[i]<'A')//check if first name has non-alpha chars
		{
			check = false;
		}
		if(check == false)
			break;
	}
	if(check == true)
	{
		if(firstName[0]<='z' && firstName[0]>='a')
		{
			firstName[0]-=32;//convert first char to UPPER CASE.
		}
		for(int i=1;i<firstName.length();i++)
		{
			if(firstName[i]<='Z' && firstName[i]>='A')	//check for upper case chars where it should be lower case
			{
				firstName[i]+=32;//convert chars to lower case
			}
		}
	}
	return check;
}

bool LastName(string &lastName)  //check whether last name input is correct or not
{
	bool check = true;
	for(int i=0;i<lastName.length();i++)
	{
		if(lastName[i]>'z' || lastName[i]>'Z' && lastName[i]<'a' || lastName[i]<'A')//check if last name has non-alpha chars
		{
			check = false;
		}
		if(check == false)
			break;
	}
	if(check == true)
	{
		if(lastName[0]<='z' && lastName[0]>='a')//if first char is lower case
		{
			lastName[0]-=32;//convert first char to UPPER CASE.
		}
		for(int i=1;i<lastName.length();i++)
		{
			if(lastName[i]<='Z' && lastName[i]>='A')//check for upper case chars where it should be lower case
			{
				lastName[i]+=32;//convert chars to lower case
			}
		}
	}
	return check;
}

bool Phone(string phone)        //check whether phone number input is correct or not
{
	bool check = true;
	if(phone.length() != 12)//length error
	{
		check = false;
	}
	if(phone[3] != '-' || phone[7] != '-')//if phone# does not contain "-"
	{
		check = false;
	}

	for(int i=0;i<phone.length();i++)
	{
		if(phone[i]>'9' || phone[i]<'0' && phone[i] != '-')//phone# with non numbers
		{
			check = false;
		}

	}
	return check;
}

bool isEmpty(ifstream &placementIn)
{
    return placementIn.peek() == ifstream::traits_type::eof();
}


int main(int argc, char **argv)
{
    struct sockaddr_in sin;
    socklen_t addrlen;
    char buf[MAX_LINE];
	string temp, begin, firstName, lastName, phone, ID, line;
	bool firstNamead, lastNamead, phonead;
	int recordInt = 0;
    int len;
    int s;
    int new_s;
    /* build address data structure */
    bzero((char *)&sin, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = INADDR_ANY;
    sin.sin_port = htons (SERVER_PORT);



    /* setup passive open */
    if (( s = socket (AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket");
		exit(1);
    }

    if ((bind(s, (struct sockaddr *) &sin, sizeof(sin))) < 0) {
		perror("bind");
		exit(1);
    }
    listen (s, MAX_PENDING);
    addrlen = sizeof(sin);
    cout << "The server is up, waiting for connection" << endl;
    /* wait for connection, then receive and print text */
    while (1)
	{
		if ((new_s = accept(s, (struct sockaddr *)&sin, &addrlen)) < 0)
		{
			perror("accept");
			exit(1);
		}
		cout << "new connection from " << inet_ntoa(sin.sin_addr) << endl;
		//get the input string from the clien
		while (len = recv(new_s, buf, sizeof(buf), 0))
		{
			stringstream ss(buf);
			vector<string> IN;
			while(ss>>temp)
				IN.push_back(temp);//put every words of the information into a stack
			begin = IN[0];        // begin is the the command

//===================================ADD==========================================
	/*  open the Data.txt file and then to find the max number of all records and insert the new information into the txt file  */
            
            
            if(begin == "ADD")                  // ADD THE INFORMATION INTO A FILE
			{
				if(IN.size() != 4)   // the command is not entire,then comes the command
				{
					strcpy(buf, "invalid command 1\n");
				}
				else
				{
					firstName = IN[1];   // stack also store other information
					lastName = IN[2];
					phone = IN[3];

					firstNamead = FirstName(firstName);
					lastNamead = LastName(lastName);
					phonead = Phone(phone);

					if(firstNamead==false || lastNamead==false || phonead== false)
					{
						strcpy(buf, "invalid command 2\n");//check the format is correct or not
					}
					else
					{
						ifstream placementIn;
						placementIn.open("Data.txt");   // open an txt file
						if(placementIn.is_open()){
							int recordInt=RECORD_START;   //the start ID
							int temp;
							while(getline(placementIn, line))
							{
								string recordString;
 								for(int i=0; i<4; i++)
								{
									recordString.push_back(line[i]);
								}
								istringstream(recordString)>>temp;
								if(temp>recordInt){
									recordInt=temp;
								}
							}
							recordInt++;
							ofstream file;
							file.open ("Data.txt", ios::app);
							file<<recordInt<<"\t"<<firstName<<" "<<lastName<<"\t"<<phone<<endl;
							file.close();
							strcpy(buf,"200 OK\n\0");
                            cout<<"200 OK\n\0"<<endl;
							cout<<"The new Record ID is "<<recordInt<<endl;
						}
					}
				}
	send (new_s, buf, strlen(buf) + 1, 0);
			}


//====================DELETE===================================================
/* create another file and move the uncommon information into the new file, then delete the old file and rename the new file  */
    
	else if(begin == "DELETE")     // delete the information
	{
                if(IN.size() != 2)
                {
                    strcpy(buf, "invalid command1\n");// error out
                }
				else
				{
					ID = IN[1];
					ifstream file;
					file.open ("Data.txt");
					ofstream outfile;
					outfile.open("Date.txt");

				if(file.is_open() && outfile.is_open())
					{
						while(getline(file, line))
						{
                            string r;
							for(int i=0; i<4; i++)
							{
					r.push_back(line[i]);
							}

							if(r != ID)
							{
								outfile<<line<<endl;
							}

                        }
						outfile.close();
						file.close();
                    remove("Data.txt");	//delete old Data file
                    rename("Date.txt","Data.txt");	//rename the new file with the orginal file's name
						strcpy(buf, "200 OK\n\0");
                         cout<<"200 OK\n\0"<<endl;
				    }
				}
					send (new_s, buf, strlen(buf) + 1, 0);

	}
//============================================LIST================================
/*  open the Data file and read every line of the information print it out */

			else if(begin == "LIST")
			{
				if(IN.size() != 1)
				{
					strcpy(buf, "invalid command1\n");
				}
				else
				{
					ifstream file;
					file.open ("Data.txt");	//open data file
					if(file.is_open())
					{
						strcpy(buf, "200 OK\n\0");
                         cout<<"200 OK\n\0"<<endl;
						cout<<"The list of records in the book:"<<endl;
						while(getline(file, line)) //List the contents, line by line
						{
							cout<<line<<endl;
						}
						file.close();
					}
				}
		send (new_s, buf, strlen(buf) + 1, 0);
			}

//============================QUIT================================================
    /* close the socket and then exit the program  */
		else if(begin == "QUIT")
			{
				if(IN.size() != 1)
				{
					strcpy(buf, "invalid command1\n");
				}
				else
				{
					strcpy(buf, "200 OK\n\0");
                     cout<<"200 OK\n\0"<<endl;
					send (new_s, buf, strlen(buf) + 1, 0);
                    close(s);
                    exit(0);
				}
			}
//==========================SHUTDOWN=========================================
    /*  stopping to connect to the server */
			else if(begin == "SHUTDOWN")
			{
				if(IN.size() != 1)
				{
					strcpy(buf, "invalid command1\n");
				}
				else
				{
					strcpy(buf, "200 OK\n\0");
                     cout<<"200 OK\n\0"<<endl;
                    send (new_s, buf, strlen(buf) + 1, 0);
					shutdown(new_s,2);
				}
			}
//=============================error=========================================
			else
			{
				strcpy(buf, "invalid command1\n");
                send (new_s, buf, strlen(buf) + 1, 0);
			}

		}
		close(new_s);
    }
}

