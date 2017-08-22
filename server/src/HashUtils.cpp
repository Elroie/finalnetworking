#include "HashUtils.h"

using namespace std;

string HashUtils::encrypt(string password){
    // Make sure the key is at least as long as the message
    std::string tmp(this->_secretKey);
    while (this->_secretKey.size() < password.size())
    	this->_secretKey += tmp;

    // And now for the encryption part
    for (std::string::size_type i = 0; i < password.size(); ++i)
    	password[i] ^= this->_secretKey[i];
    return password;
}

string HashUtils::decrypt(string hash){
	return encrypt(hash); // :)
}
