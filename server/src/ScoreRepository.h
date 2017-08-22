#include <string>
#include <map>
#include <fstream>

using namespace std;

class ScoreRepository {
private:
	map<string, int> userToScore;

public:
	ScoreRepository();
	void updateUserScore(string username, int score);
	std::vector<string> getScoreBoard();
};
