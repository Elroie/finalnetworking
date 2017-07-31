#include "UserRepository.h"
#include <sstream>
#include <iostream>
#include <vector>
#include <string>
#include <stdlib.h>

using namespace std;

UserRepository::UserRepository(){
   UserRepository::populateCache();
}

bool UserRepository::login(string username, string password){
	if (this->_cache.count(username) > 0){
		pair<string, bool> val;
		val.first = username;
		val.second = true;
		this->_onlineUsers2availability.insert(val);
		return this->_cache[username] == password;
	}

	return false;
}

void UserRepository::add(string username, string password){
	ofstream file;
	file.open("users.txt", std::ofstream::out | std::ofstream::app);

	file << username << "," << password << std::endl;

	file.close();
}


void UserRepository::populateCache(){
   char *username;
	char *password;
	ifstream file;
	file.open("users.txt");
	string line;
	while (std::getline(file, line))
	{
		std::istringstream iss(line);
		string token;
		std::getline(iss, token, ',');
		
		pair<string, string> keyValue;
		keyValue.first = token;
		std::getline(iss, token, ',');
		keyValue.second = token;
		this->_cache.insert(keyValue);
	}

	file.close();
}

std::vector<string> UserRepository::getOnlineUsers(){
	vector<string> v;
	for(map<string,bool>::iterator it = this->_onlineUsers2availability.begin();
	 it != this->_onlineUsers2availability.end(); ++it) {
		v.push_back(it->first);
		cout << it->first << "\n";
	}

	return v;
}

bool UserRepository::getUserAvailability(string username){
	return this->_onlineUsers2availability[username];
}

void UserRepository::setUserAvailability(string username){
	this->_onlineUsers2availability[username] = false;
}