#include "UserRepository.h"
#include <sstream>
#include <iostream>
#include <vector>
#include <strings.h>
#include <stdlib.h>

using namespace std;

map<string, string>* UserRepository::userToPassword = NULL;

/**
 * Caches the file system calls
 */
map<string, string>* UserRepository::getAll() {
	if (userToPassword == NULL) {
		load();
	}

	return userToPassword;
}

/**
 * Loads the text files to the map
 */
bool UserRepository::load() {
	map<string, string>* newUserToPassword = new map<string, string>();

	ifstream users;
	ifstream passwords;

	passwords.open("passwords");
	users.open("users");

	string password;
	string name;

	while (users && passwords) {
		getline(users, name);
		getline(passwords, password);

		(*newUserToPassword)[name] = password;
	}

	users.close();
	passwords.close();

	map<string, string>* oldUserToPassword = userToPassword;
	userToPassword = newUserToPassword;

	if (oldUserToPassword != NULL) {
		delete oldUserToPassword;
	}

	return true;
}

/**
 * Check if user exists
 */
bool UserRepository::contains(string name) {
	map<string, string>* all = getAll();
	return all->find(name) != all->end();
}

/**
 * Create a new user
 */
bool UserRepository::create(string name, string password) {
	if (contains(name)) {
		return false;
	}

	ofstream users;
	ofstream passwords;

	users.open("users.txt", std::ios_base::app);
	passwords.open("passwords.txt", std::ios_base::app);

	users << endl << name;
	passwords << endl << password;

	users.close();
	passwords.close();

	load();

	return true;
}

/**
 * Check if there is a match with a username and password
 */
bool UserRepository::login(string name, string password) {
	if (!contains(name)) {
		return false;
	}

	return (userToPassword->find(name)->second == password);
}

/**
 * Get all the users registered
 */
vector<string> UserRepository::getAllNames() {
	vector<string> names;

	for (std::map<string, string>::const_iterator it = getAll()->begin();
			it != getAll()->end(); it++) {
		string key = it->first;
		names.push_back(key);
	}

	return names;
}

/**
 * Get all the users registered in a textual representation
 */
string UserRepository::getAllUsers() {
	string result = "";

	vector<string> names = getAllNames();

	if (names.size() == 0) {
		return "There is no users.";
	}

	for (unsigned int i = 0; i < names.size(); i++) {
		if (result != "") {
			result += ",";
		}

		result += names[i];
	}

	return "Users: " + result;
}

// UserRepository::UserRepository(){
//    UserRepository::populateCache();
// }

// bool UserRepository::login(string username, string password){
// 	if (this->_cache.count(username) > 0){
// 		pair<string, bool> val;
// 		val.first = username;
// 		val.second = true;
// 		this->_onlineUsers2availability.insert(val);
// 		return this->_cache[username] == password;
// 	}

// 	return false;
// }

// void UserRepository::add(string username, string password){
// 	ofstream file;
// 	file.open("users.txt", std::ofstream::out | std::ofstream::app);

// 	file << username << "," << password << std::endl;

// 	file.close();
// }


// void UserRepository::populateCache(){
//    char *username;
// 	char *password;
// 	ifstream file;
// 	file.open("users.txt");
// 	string line;
// 	while (std::getline(file, line))
// 	{
// 		std::istringstream iss(line);
// 		string token;
// 		std::getline(iss, token, ',');
		
// 		pair<string, string> keyValue;
// 		keyValue.first = token;
// 		std::getline(iss, token, ',');
// 		keyValue.second = token;
// 		this->_cache.insert(keyValue);
// 	}

// 	file.close();
// }

// std::vector<string> UserRepository::getOnlineUsers(){
// 	vector<string> v;
// 	for(map<string,bool>::iterator it = this->_onlineUsers2availability.begin();
// 	 it != this->_onlineUsers2availability.end(); ++it) {
// 		v.push_back(it->first);
// 		cout << it->first << "\n";
// 	}

// 	return v;
// }

// bool UserRepository::getUserAvailability(string username){
// 	return this->_onlineUsers2availability[username];
// }

// void UserRepository::setUserAvailability(string username){
// 	this->_onlineUsers2availability[username] = false;
// }