#include <string>
#include <fstream>
#include <map>
#include <cstring>
#include <vector>
// #include <libssl/md5.h>

using namespace std;

class UserRepository 
{
   private:
       std::map<string, string> _cache;
       std::map<string, bool> _onlineUsers2availability;       
       void populateCache();

   public:
       UserRepository();
       ~UserRepository();
       bool login(string username, string password);
       void add(string username, string password);
       std::vector<string> getOnlineUsers();
       void setUserAvailability(string username);
       bool getUserAvailability(string username);
};
