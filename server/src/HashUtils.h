#include <string>
#include <iostream>

using namespace std;

class HashUtils {
private:
	string _secretKey =
			"SDFfgfDVN435HUTcvbRT56DSCFG226J7Hjfd7dsdvcjxbzksdjwhekjhsdfkjsd7sfvjh4343esdfbkjhjkcxmnc65654nvcmbnckljfdkdsdjaswerewrioeutiou84=";

public:
	string encrypt(string password);
	string decrypt(string hash);
};