#include "UserRepository.h"
#include <sstream>
#include <iostream>
#include <vector>
#include "User.h"

UserRepository::UserRepository(){
    UserRepository::populateCache();
}

UserRepository::~UserRepository(){
    
}

bool UserRepository::login(string username, string password){
	if (this->_cache.count(username) > 0){
		User user = this->_cache[username];
		bool result = user.validatePassword(password);
		if (result){
			user.isOnline = true;
			user.isFreeForPlaying = true;
		}
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
	ifstream file;
	file.open("users.txt");
	string line;
	while (std::getline(file, line))
	{
		std::istringstream iss(line);
		string username;
		string password;
		std::getline(iss, username, ',');
		
		pair<string, User> keyValue;
		keyValue.first = username;
		std::getline(iss, password, ',');
		keyValue.second = new User(username, password);
		this->_cache.insert(keyValue);
	}

	file.close();
}