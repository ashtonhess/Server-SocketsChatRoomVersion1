//
// Created by Ashton Hess on 3/17/22. -> Migrating so I can use my Mac. Now using makefiles instead of VS.
// Pawprint: AAHB8F
//
//
// Program description:
// I implemented a simple chat room in c++ with both a server and client that both use
//the socket API. The client program provides the following commands to the user: 'login'(allow users to join the chat room),
// 'newuser'(create a new user account), 'send'(send a message to other clients; actually send the message to the server and
// the server forwards the message to other clients), and 'logout'(quit the chat room).
//
// The server runs the chat room service and echoes messages back to the client.
// Storage of all users that are created is implemented to store a list in users.txt.
//
//
// How to run:
// First, you will need a C++14 compiler installed.
// Second, navigate to each program's folder in terminal.
// Third, run the command "make" in each terminal. This will create the object files and the executables.
// Last, run the command "./server" in the server terminal, and "./client" (appending the server address argument) in the client's terminal.
//
// Note: To clean, run make clean in both project directories.
//
//Server output:
/*
ashtonhess@Bushes-Pro-2 server % ./server
Ashton's chat room server. Version One.

New user account created.
Tom login.
Tom: Hello, is anybody out there?
Tom: Bye for now.
Tom logout.
Closed socket. Client disconnected.

(Program is still running down here. Just waiting for a client to connect again.)
*/
#include <iostream>
using namespace std;
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <fstream>
#include <algorithm>
#include <sstream>
#include <vector>
#include <iterator>
#include <cstring>

#include "User.h"

#define SERVER_PORT 11328
#define MAX_PENDING 5
#define MAX_LINE 256

int login(string userID, string password);
int newUser(string userID, string password);
int sendMessage(char*message);
int logout(string userID);
vector<User> readUsersFromFile(const string& fileName);
void printUsers(vector<User> usersVec);
void addUserToFile(const string& fileName, User userToAdd);
vector<string> split (const string &s, char delim);

//SERVER
int main(int argc, char*argv[]){
try{
    string fileName = "users.txt";
    const char *welcomeMsg = "Ashton's chat room server. Version One.\n";
    vector<User> users;
    //Ensure error checks on size of users after reading in file data.
    users = readUsersFromFile(fileName);
    //File descriptor for stocket.
    int serverFD;
    //stores the accepted socket connection
    int currentSocket;
    //setting up my socket.
    //creating FD (file descriptor) for socket.
    if ((serverFD = socket(AF_INET,SOCK_STREAM,0)) == 0){
        perror("> Error: setting server file descriptor.\n");
        return 1;
    }
    //attaching socket to SERVER_PORT.
    int opt = 1;
    if(setsockopt(serverFD, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))){
        cout<<"> Error: setsockopt()"<<endl;
        return 1;
    }
    struct sockaddr_in addy;
    addy.sin_family = AF_INET;
    addy.sin_addr.s_addr = INADDR_ANY;
    addy.sin_port = htons(SERVER_PORT);//host byte order->network byte order
    //Binding socket
    //Must use scope resolution operator '::' here to call bind() from the socket import,
    // since I'm using the std namespace. This is because std::bind() also exists.
    if (::bind(serverFD, (struct sockaddr *)&addy, sizeof(addy)) < 0 ){
        cout<<"> Error: binding socket."<<endl;
        return 1;
    }
    //listening on the socket for a new connection. Next, accept a socket connection and store it
    if (listen(serverFD, MAX_PENDING) < 0) {
        cout<<"> Error: listening on socket."<<endl;
        return 1;
    }
    //printing message to server user.
    cout<<welcomeMsg<<endl;
    string userID;
    bool logInStatus=false;
    bool while1=true;
    bool while2=true;
    while(while1) {
        while2=true;
        //accepting and storing a socket connection
        if ((currentSocket = accept(serverFD, (struct sockaddr *) &addy, (socklen_t *) &addy)) < 0) {
            cout<<"> Error: accepting the socket."<<endl;
            return 1;
        }
        logInStatus = false;
        while(while2){
            char buffer[MAX_LINE] ="";
            char placeholder[MAX_LINE]="";
            long int i = recv(currentSocket, buffer, MAX_LINE, 0);
            //checking received value for errors.
            if (i==-1){
                close(currentSocket);
                cout<<"Closed socket. Client disconnected."<<endl;
                break;
            }
            //uncomment to see what is being received each time.
            //cout<<buffer<<endl;
            string storedBuffer;
            if (!storedBuffer.empty()){
                storedBuffer= buffer;
            }
            //breaking up the string into pieces (see split function)
            vector<string> delimitVec;
            delimitVec = split(buffer,' ');
            //checking what command is received
            if (delimitVec.at(0)=="login"){
                    int loginResult= login(delimitVec.at(1),delimitVec.at(2));
                    //return 0 for user does not exist; return 1 for login success; return 2 for wrong password.
                    if (loginResult!=1){
                        const char *loginFail= "> Denied. User name or password incorrect.";
                        strcpy(buffer, loginFail);
                        send(currentSocket, buffer, strlen(buffer), 0);
                        userID = delimitVec.at(1);
                    }else{
                        logInStatus=true;
                        const char *logInSuccessMsg= "> login confirmed";
                        strcpy(buffer, logInSuccessMsg);
                        send(currentSocket, buffer, strlen(buffer), 0);
                        userID = delimitVec.at(1);
                        cout<<userID<<" login."<<endl;
                    }
            }else if (delimitVec.at(0)=="newuser"){
                //Create a new user account; returns 0 if userID account already exists; returns 1 if user successfully added.
                int newUserResult = newUser(delimitVec.at(1), delimitVec.at(2));
                if (newUserResult==1){
                    cout<<"New user account created."<<endl;//---------------------------------------------------------------NEEDED--------------------------------------------------------------------------
                    const char *userAcctCreatedSuccess= "> New user account created. Please login.";
                    strcpy(buffer, userAcctCreatedSuccess);
                    send(currentSocket, buffer, strlen(buffer), 0);
                }else{
                    const char *userAcctExists= "> Denied. User account already exists.";
                    strcpy(buffer, userAcctExists);
                    send(currentSocket, buffer, strlen(buffer), 0);
                }
            }else if (delimitVec.at(0)=="send"){
                if (logInStatus==true){
                    //creating message to send back to client
                    string totalMessage=userID+": ";
                    for (auto t=delimitVec.begin()+1; t!=delimitVec.end(); ++t)
                    {
                        totalMessage=totalMessage+*t+' ';
                    }
                    cout<<totalMessage<<endl;
                    string totalMessageToSend="> "+totalMessage;
                    const char *sendMsg= totalMessageToSend.c_str();
                    strcpy(buffer, sendMsg);
                    send(currentSocket, buffer, strlen(buffer), 0);
                }else{
                    const char *loginBefore= "> Denied. Please login first.";
                    strcpy(buffer, loginBefore);
                    send(currentSocket, buffer, strlen(buffer), 0);
                }
            }else if (delimitVec.at(0)=="logout") {
                if (logInStatus == true) {
                    logout(userID);
                    string totalLogOutMsg="> "+userID+" left.";
                    userID = "";
                    const char *logoutMsg= totalLogOutMsg.c_str();
                    strcpy(buffer, logoutMsg);
                    send(currentSocket, buffer, strlen(buffer), 0);
                    close(currentSocket);//closing the socket
                }else{
                    const char *logoutNotLoggedIn= "> Denied. Please login first.";
                    strcpy(buffer, logoutNotLoggedIn);
                    send(currentSocket, buffer, strlen(buffer), 0);
                }
            }
        } //END INNER WHILE
    }//END OUTER WHILE
}catch(std::out_of_range){
    //catching any unexpected exceptions that can occur with the vectors.
}
return 0;
}
//This is a helper function that splits a string up into pieces based on the delimiter.
//It then stores these pieces in a string vector.
vector<string> split (const string &s, char delim) {
    vector<string> result;
    stringstream ss (s);
    string item;
    while (getline (ss, item, delim)) {
        result.push_back (item);
    }
    return result;
}
//This function prints the entire vector of User objects.
void printUsers(vector<User> usersVec){
    for (int i = 0; i < usersVec.size(); ++i) {
        string userID = usersVec.at(i).getUserID();
        string password = usersVec.at(i).getPassword();
        cout<<"User "<<i<<": "<<userID<<" "<<password<<endl;
    }
}
//This function reads all User objects in from a file.
//Need to seperate FileIO into a new class eventually.
vector<User> readUsersFromFile(const string& fileName) {
    vector<User> errorVector = {};
    std::ifstream ifs;
    ifs.open(fileName, ios::in | ios::binary);
    if (!ifs) {
        cerr << "Cannot open file: " << fileName << "." << endl;
        return errorVector;
    }
    string data;
    if (ifs.is_open()) {
        vector<User> returnVector;
        while (getline(ifs, data)) {
            //cout<<"Data before erase: "<<data<<endl;
            data.erase(remove(data.begin(), data.end(), '('), data.end());
            //data.erase(remove(data.begin(),data.end(),')'), data.end());
            data.erase(remove(data.begin(), data.end(), ' '), data.end());
            //cout<<"Data after erase: "<<data<<endl;
            stringstream ss(data);
            string userID;
            getline(ss, userID, ',');
            string password;
            getline(ss, password, ')');
            //cout<<"userID: "<<userID<<endl;
            //cout<<"password: "<<password<<endl;
            User *newUser = new User(userID, password);
            returnVector.push_back(*newUser);
        }
        return returnVector;
    }else{
        return errorVector;
    }
}
//return 0 for user does not exist; return 1 for login success; return 2 for wrong password.
int login(string userID, string password){
    vector<User> allUsersVec;
    allUsersVec=readUsersFromFile("users.txt");
    for (auto it = begin (allUsersVec); it != end (allUsersVec); ++it) {
        if(it->getUserID()==userID&&it->getPassword()==password){
            //cout<<it->getUserID()<<" login."<<endl;//---------------------------------------------------------------NEEDED--------------------------------------------------------------------------
            return 1;
        }else if(it->getUserID()==userID&&it->getPassword()!=password){
            return 2;
        }
    }
    return 0;
}
//Create a new user account; returns 0 if userID account already exists; returns 1 if user successfully added.
int newUser(string userID, string password){
    if (login(userID, password)==1 || login(userID,password)==2){
        return 0;
    }else{
        User*newUser = new User(userID, password);
        addUserToFile("users.txt", *newUser);
        return 1;
    }
}
//Send a message to other clients; specifically: send the message to the server and the server forwards the message to other clients.
//Message size between 1 and 256 characters.
int sendMessage(char*message){

    return 0;
}
//Quit the chatroom.
int logout(string userID){
    cout<<userID<<" logout."<<endl;
    return 0;
}

//This function adds a user object to the .txt file specified.
void addUserToFile(const string& fileName, User userToAdd){
    string userString;
    userString="("+userToAdd.getUserID()+","+" "+userToAdd.getPassword()+")";
    ofstream outFile;
    outFile.open(fileName, std::ios_base::app);
    outFile<<endl<<userString;
}




/*//Buffer to hold current message.
   char msgBuf[MAX_LINE] = {};*/

//TESTING:
//cout<<"Waiting for a client to connect..."<<endl;

//cout<<"Waiting for a client to connect..."<<endl;

//cout<<"> Denied. User name or password incorrect."<<endl;

//  cout<<it->getUserID()<<" logout."<<endl;//---------------------------------------------------------------NEEDED--------------------------------------------------------------------------


//printf("%s\n", msgBuf);
//const char *welcomeMsg = "Ashton's chat room server. Version One.\n";
//send(currentSocket, welcomeMsg, strlen(welcomeMsg), 0);
//        cout<<welcomeMsg;

//cout<<"> Denied. User account already exists."<<endl;

    //testing for login function
//    int loginResult;
//    loginResult=login("Tomasdf", "Tom12");
//    cout<<loginResult<<endl;
//addUserToFile(fileName,*testUser);
//newUser(testUser->getUserID(), testUser->getPassword());
//users = readUsersFromFile(fileName);
//printUsers(users);
//cout<<endl;
//valRead = read(newSocket, msgBuf, MAX_LINE);
//User*testUser = new User("testUserIDNEW","testPasswordNEW");
//logout(userID);
//userID = "";
//cout<<endl<<"userString to append to file: "<<userString;
//newUser->setUserID(userID);
//newUser->setPassword(password);
//cout << "user: " << newUser->getUserID() << " " << newUser->getPassword() << endl;
//old try...
//            buffer[i]=0;
//            vector<string> delimitVec = split(buffer,' ');
//            if(delimitVec.at(0)=="login"){
//                if(isLoggedIn==false){
//                int loginInt = login(delimitVec.at(1),delimitVec.at(2));
//                //return 0 for user does not exist
//                //return 1 for login success
//                //return 2 for wrong password
//                if(loginInt==0){
//                    strncpy(placeholder, "User does not exist.",sizeof(placeholder));
//                }else if (loginInt==2){
//                    strncpy(placeholder,"Wrong password.",sizeof(placeholder));
//                }else{
//                    isLoggedIn=true;
//                    strncpy(placeholder,"Login success.", sizeof(placeholder));
//                    //string userIDChar = delimitVec.at(1);
//                    userID=delimitVec.at(1);
//                    cout<<userID<<" logged in.";
//                }
//            }else{
//                    cout<<"A user is already logged in."<<endl;
//                }
//            }else if(delimitVec.at(0)=="newuser"){
//                int newUserInt = newUser(delimitVec.at(1),delimitVec.at(2));
//                //returns 0 if userID account already exists
//                //returns 1 if user successfully added.
//                if(newUserInt==0){
//                    cout<<"Creating new user failed. Account exists."<<endl;
//                    strncpy(placeholder,"Creating new user failed. Account exists.",sizeof(placeholder));
//                    break;
//                }else if(newUserInt==1){
//                    strncpy(placeholder,"Creating new user failed. Account exists.",sizeof(placeholder));
//                    break;
//                }
//            }else if(delimitVec.at(0)=="send"){
//                if(isLoggedIn){
//                   // char*msg =
//                }
//            }
//NO WORK:
//    int searchResult=0;
//    vector<User> allUsersVec;
//    string userIDString;
//    userIDString=userID;
//    allUsersVec=readUsersFromFile("users.txt");
//    for (auto it = begin (allUsersVec); it != end (allUsersVec); ++it) {
//        if(it->getUserID()==userIDString){
//            searchResult=1;
//        }
//    }
//    if (searchResult==1){
//        //user already exists
//        return 0;
//    }else{
//        User*newUser = new User(userID, password);
////        newUser->setUserID(userID);
////        newUser->setPassword(password);
//        addUserToFile("users.txt", *newUser);
//        return 1;
//    }
//    User*userToFind= new User();
//    userToFind->setUserID(uName);
//    userToFind->setPassword(pass);
//    vector<User>::iterator it;
//    User foundUser;
//    it=find(allUsersVec.begin(),allUsersVec.end(),*userToFind);
//
//    cout<<"Found user: "<<it->getUserID()<<" "<<it->getPassword()<<endl;
//GIVEN:
//----------------------------------------------------
    // Initialize Winsock.
//    WSADATA wsaData;
//    int iResult = WSAStartup( MAKEWORD(2,2), &wsaData );
//    if ( iResult != NO_ERROR ){
//        printf("Error at WSAStartup()\n");
//        return;
//    }
    // Create a socket.
//    SOCKET listenSocket;
//    listenSocket = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
//    if ( listenSocket == INVALID_SOCKET ) {
//        printf( "Error at socket(): %ld\n", WSAGetLastError() );
//        WSACleanup();
//        return;
//    }
    // Bind the socket.
//    sockaddr_in addr;
//    addr.sin_family = AF_INET;
//    addr.sin_addr.s_addr = INADDR_ANY; //use local address
//    addr.sin_port = htons(SERVER_PORT);
//    if ( bind( listenSocket, (SOCKADDR*) &addr, sizeof(addr) ) == SOCKET_ERROR ) {
//        printf( "bind() failed.\n" );
//        closesocket(listenSocket);
//        WSACleanup();
//        return;
//    }
    // Listen on the Socket.
//    if ( listen( listenSocket, MAX_PENDING ) == SOCKET_ERROR ){
//        printf( "Error listening on socket.\n");
//        closesocket(listenSocket);
//        WSACleanup();
//        return;
//    }
    // Accept connections.
//    SOCKET s;
//    printf( "Waiting for a client to connect...\n" );
//    while(1){
//        s = accept( listenSocket, NULL, NULL );
//        if( s == SOCKET_ERROR){
//            printf("accept() error \n");
//            closesocket(listenSocket);
//            WSACleanup();
//            return;
//        }
//        printf( "Client Connected.\n");
//        // Send and receive data.
//        char buf[MAX_LINE];
//        int len = recv( s, buf, MAX_LINE, 0 );
//        buf[len] = 0;
//        send( s, buf, strlen(buf), 0 );
//        closesocket(s);
//        printf( "Client Closed.\n");
//    }
//    closesocket(listenSocket);