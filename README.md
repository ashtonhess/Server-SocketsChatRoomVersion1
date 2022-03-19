# Server-SocketsChatRoomVersion1
# Client-SocketsChatRoomVersion1
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
//Client output:
/*
ashtonhess@Bushes-Pro-2 client % ./client 127.0.0.1
Ashton's chat room client. Version One.
> newuser Mike Mike11
> New user account created. Please login.
> newuser Mike Mike11
> Denied. User account already exists.
> send
> Denied. send incorrect arguments.
> login Tom Tom12
> Denied. User name or password incorrect.
> login Tom Tom11
> login confirmed
> send Hello, is anybody out there?
> Tom: Hello, is anybody out there?
> send Bye for now.
> Tom: Bye for now.
> logout
> Tom left.
ashtonhess@Bushes-Pro-2 client %
 */
