#include <string>
#include <fstream>
#include <map>
#include <cstring>
// #include <libssl/md5.h>

using namespace std;

class UserRepository 
{
    private:
        std::map<string, string> _cache;
        void populateCache();

    public:
        UserRepository();
        ~UserRepository();
        bool login(string username, string password);
        void add(string username, string password);
};