#include "ScoreRepository.h"
#include <sstream>
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

bool scoreCompare(const pair<string, int>  &p1, const pair<string, int> &p2)
{
    return p1.second > p2.second;
}

ScoreRepository::ScoreRepository() {
	// populating cache with users and their score.
	ifstream file;
	file.open("scoreboard.txt");
	string line;
	while (std::getline(file, line))
	{
		std::istringstream iss(line);
		string token;
		std::getline(iss, token, ',');

		pair<string, int> keyValue;
		keyValue.first = token;

		std::getline(iss, token, ',');
		keyValue.second = atoi(token.c_str());
		this->userToScore.insert(keyValue);
	}

	file.close();

}

void ScoreRepository::updateUserScore(string username, int score){
	this->userToScore[username] += score;

	// since the cache is dirty... let's rewrite the file with the updated data.
	ofstream file;
	file.open("scoreboard.txt", std::ofstream::out);

	for(map<string,int>::iterator it = this->userToScore.begin(); it != this->userToScore.end(); ++it) {
		file << it->first << "," << it->second << std::endl;
	}

	file.close();
}

std::vector<string> ScoreRepository::getScoreBoard(){
	// creates a copy of the data in a list
	vector<pair<string, int> > v(this->userToScore.size());
	copy(this->userToScore.begin(), this->userToScore.end(), v.begin());

	// sort the list.
	std::sort(v.begin(), v.end(), scoreCompare);

	vector<string> scoreBoard;
	std::vector<pair<string,int> >::iterator sit;
	for(sit = v.begin(); sit != v.end(); ++sit) {
		string temp = sit->first + " " + std::to_string(sit->second);
		scoreBoard.push_back(temp);
	}

	return scoreBoard;
}
