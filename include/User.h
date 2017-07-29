#include <string>

using namespace std;

class User
{
    private:
        string _password;
        int _score;
    
    public:
        User(string username, string password);
        string username;
        bool isOnline;
        bool isFreeForPlaying;
        
        bool canPlay();
        bool validatePassword(string password);
        int getScore();
        void update(int score);
};