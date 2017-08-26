
#include "UserRepository.h"
#include "ScoreRepository.h"
#include "HashUtils.h"
#include <sstream>
#include <iostream>
#include <vector>

UserRepository::UserRepository(){
   UserRepository::populateCache();
}

UserRepository::~UserRepository(){

}

//bool UserRepository::login(string username, string password){
//	if (this->_cache.count(username) > 0){
//		pair<string, bool> val;
//		val.first = username;
//		val.second = true;
//		this->_onlineUsers2availability.insert(val);
//		return this->_cache[username] == password;
//	}
//
//	return false;
//}
//
//void UserRepository::add(string username, string password){
//	ofstream file;
//	file.open("users.txt", std::ofstream::out | std::ofstream::app);
//
//	file << username << "," << password << std::endl;
//
//	file.close();
//
//	pair<string, string> user;
//	user.first = username;
//	user.second = password;
//	this->_cache.insert(user);
//
//	// create a record in scoreBoard.
//	ScoreRepository scoreRepository;
//	scoreRepository.updateUserScore(username, 0);
//}

bool UserRepository::login(string username, string password){
	if (this->_cache.count(username) > 0){
		HashUtils hashUtils;
		pair<string, bool> val;
		val.first = username;
		val.second = true;
		this->_onlineUsers2availability.insert(val);
		return this->_cache[username] == hashUtils.encrypt(password);
	}

	return false;
}

void UserRepository::add(string username, string password){
	ofstream file;
	HashUtils hashUtils;
	file.open("users.txt", std::ofstream::out | std::ofstream::app);

	string hash = hashUtils.encrypt(password);
	file << username << "," << hash << std::endl;

	file.close();

	pair<string, string> user;
	user.first = username;
	user.second = hash;
	this->_cache.insert(user);

	// create a record in scoreBoard.
	ScoreRepository scoreRepository;
	scoreRepository.updateUserScore(username, 0);
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

void UserRepository::setUserAvailability(string username, bool isAvailable){
	this->_onlineUsers2availability[username] = isAvailable;
}
