//
// Created by Ashton Hess on 3/17/22.
//

#ifndef SERVER_USER_H
#define SERVER_USER_H
#include <iostream>
using namespace std;
#include <string>

class User {
    string userID;
    string password;
public:
    void setUserID(string userID_);
    void setPassword(string password_);
    string getUserID();
    string getPassword();
    User(){}
    User(string userID_, string password_){
        userID = userID_;
        password = password_;
    }
    //add field for User's address for V2;
};


#endif //SERVER_USER_H
