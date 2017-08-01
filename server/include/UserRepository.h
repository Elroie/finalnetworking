#ifndef USERREPOSITORY_H_
#define USERREPOSITORY_H_

// #include <strings.h>
// #include <fstream>
// #include <map>
// #include <cstring>
// #include <vector>
// #include <sstream>
// #include <iostream>
// #include <stdlib.h>


#include <map>
#include <iostream>
#include <strings.h>
#include <vector>
#include <fstream>

using namespace std;

class UserRepository {
private:
	static map<string, string>* userToPassword;

	/**
	 * Caches the file system calls
	 */
	static map<string, string>* getAll();
public:
	/**
	 * Loads the text files to the map
	 */
	static bool load();

	/**
	 * Check if user exists
	 */
	static bool contains(string name);

	/**
	 * Create a new user
	 */
	static bool create(string name, string password);

	/**
	 * Check if there is a match with a username and password
	 */
	static bool login(string name, string password);

	/**
	 * Get all the users registered
	 */
	static vector<string> getAllNames();

	/**
	 * Get all the users registered in a textual representation
	 */
	static string getAllUsers();
//    private:
//        std::map<string, string> _cache;
//        std::map<string, bool> _onlineUsers2availability;       
//        void populateCache();

//    public:
//        UserRepository();
       
//        bool login(string username, string password);
//        void add(string username, string password);
//        std::vector<string> getOnlineUsers();
//        void setUserAvailability(string username);
//        bool getUserAvailability(string username);
};

#endif