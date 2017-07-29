#include "User.h"

User::User(string username, string password){
    this->username = username;
    this->_password = password;
    this->isOnline = false;
    this->isFreeForPlaying = false;
    this->_score = 0;
}

bool User::canPlay(){
    return this->isOnline && this->isFreeForPlaying;
}

int User::getScore(){
    return this->_score;
}

void User::update(int score){
    this->_score += score;
}

bool User::validatePassword(string password){
    return this->_password == password;
}