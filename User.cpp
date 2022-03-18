//
// Created by Ashton Hess on 3/17/22.
//

#include "User.h"

void User::setUserID(string userID_){
    userID = userID_;
}
void User::setPassword(string password_){
    password = password_;
}
string User::getUserID(){
    return userID;
}
string User::getPassword(){
    return password;
}
