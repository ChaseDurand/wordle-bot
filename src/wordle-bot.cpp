#include <iostream>
#include <fstream>
#include <algorithm>
#include <map>
#include <thread>
#include "lists.h"
#include "ansi_colors.h"
// Size of each word
#define WORD_SIZE 5

// Keep track of letter colors.
class TileColors {
    public:
        TileColors(){
            greens = std::vector<char>(WORD_SIZE, '_');
        }
    private:
        // Greens, list of 5 letters, '_' for default.
        std::vector<char> greens;
        // Yellows, map of letters and positions.
        std::map<char, std::vector<int>> yellows;
        // Greys, map of letters and positions.
        std::map<char, std::vector<int>> greys;
    public:
        // Return true if guess is correct and game is solved.
        bool isSolved(){
            return std::count(greens.begin(), greens.end(),'_') == 0;
        }
        
        // Add a green tile from character and position.
        void addGreen(const char& l, const int& p){
            greens[p] = l;
            return;
            // greens[l].push_back(p);
        }

        // Add a yellow tile from character and position.
        void addYellow(const char& l, const int& p){
            yellows[l].push_back(p);
        }

        // Add a grey tile from character and position.
        void addGrey(const char& l, const int& p){
            greys[l].push_back(p);
        }

        
        // Return number of green tiles of a given character.
        int getGreenCount(const char& c) const{
            return count_if(greens.begin(), greens.end(),
                [&c](const char& g){return g == c;});
        }

        // Return number of yellow tiles of a given character.
        int getYellowCount(const char& c) const{
            return yellows.count(c);
        }

        // Return number of grey tiles of a given character.
        int getGreyCount(const char& c) const{
            return greys.count(c);
        }

        // Clear yellow tiles.
        void clearYellows(){
            yellows.clear();
            return;
        }

        // Clear grey tiles.
        void clearGreys(){
            greys.clear();
            return;
        }

        // Return the character in the green vector at position i.
        char getGreenAt(const int& i) const{
            return greens[i];
        }

        // Return iterator at beginning of green vector.
        std::vector<char>::const_iterator greensBegin() const{
            return greens.begin();
        }

        // Return iterator at end of green vector.
        std::vector<char>::const_iterator greensEnd() const{
            return greens.end();
        }

        // Return iterator at beginning of yellow map.
        std::map<char, std::vector<int>>::const_iterator yellowsBegin() const{
            return yellows.cbegin();
        }

        // Return iterator at end of yellow map.
        std::map<char, std::vector<int>>::const_iterator yellowsEnd() const{
            return yellows.end();
        }

        // Return iterator at beginning of grey map.
        std::map<char, std::vector<int>>::const_iterator greysBegin() const{
            return greys.begin();
        }

        // Return iterator at end of grey map.
        std::map<char, std::vector<int>>::const_iterator greysEnd() const{
            return greys.end();
        }
};

// Add a word to a letter distribution table.
void addToDist(const std::string& w,
    std::vector<std::vector<int>>& answerDistribution){
    for(int i = 0; i < WORD_SIZE; ++i){
        // Shift lowercase a to 0
        answerDistribution[i][w[i]-97]++;
    }
    return;
}

// Get a word's score based on a letter distribution table.
int getGuessScore(const std::string& w,
    const std::vector<std::vector<int>>& answerDistribution){
    int sum = 0;
    for(int i = 0; i < WORD_SIZE; ++i){
        sum += answerDistribution[i][w[i]-97];
    }
    return sum;
}

// Get best guess given letter distribution table and list of valid words.
std::string getBestGuess(const std::vector<std::vector<int>>& dist,
    const std::vector<std::string>& guesses){
    std::string guess = "";
    int maxScore = -1;
    int score;
    for(const std::string& w : guesses){
        score = getGuessScore(w, dist);
        if(score > maxScore){
            guess = w;
            maxScore = score;
        }
    }
    return guess;
}

// Compare a word against an answer and record greens/yellows/greys.
void checkGuess(const std::string& guess, const std::string& answer,
    TileColors& tc){
    // Keep track of letters not colored for accurate letter counting.
    std::vector<char> lettersRemaining(answer.begin(), answer.end());
    // Use color codes to store color mask.
    std::vector<std::string> highlightMask(WORD_SIZE, RESET);
    // Identify greens.
    for(int i = 0; i < WORD_SIZE; ++i){
        if(guess[i] == answer[i]){
            lettersRemaining.erase(std::find(lettersRemaining.begin(), lettersRemaining.end(), guess[i]));
            highlightMask[i] = GREEN;
            tc.addGreen(guess[i], i);
        }
    }
    // Identify yellows.
    for(int i = 0; i < WORD_SIZE; ++i){
        if(highlightMask[i] != GREEN){
            if(std::find(lettersRemaining.begin(), lettersRemaining.end(), guess[i]) != lettersRemaining.end()){
                lettersRemaining.erase(std::find(lettersRemaining.begin(), lettersRemaining.end(), guess[i]));
                highlightMask[i] = YELLOW;
                tc.addYellow(guess[i], i);
            }
        }
    }
    // Identify greys and print guess result.
    for(int i = 0; i < WORD_SIZE; ++i){
        if (highlightMask[i] == RESET){
            tc.addGrey(guess[i], i);
        }
        std::cout << highlightMask[i] << guess[i] << RESET;
    }
    std::cout << std::endl;
    return;
}

// Check word against greens, return true if invalid.
bool checkGreens(const std::string& w, const TileColors& tc){
    for(int i = 0; i < WORD_SIZE; ++i){
        if (tc.getGreenAt(i) != '_'){
            if (tc.getGreenAt(i) != w[i]){
                return true;
            }
        }
    }
    return false;
}

// Check word against yellows, return true if invalid.
bool checkYellows(const std::string& w, const TileColors& tc){
    for(auto iter = tc.yellowsBegin(); iter != tc.yellowsEnd(); ++iter){
        // For every yellow character's position.
        for(const int& yellowPositions : iter->second){
            if(iter->first == w[yellowPositions]){
                // Word has a letter in yellow position, not valid.
                return true;
            }
        }
        int occurances = 0;
        for(int i = 0; i < WORD_SIZE; ++i){
            if(w[i] == iter->first){
                occurances++;
            }
        }
        // If we have greys of the same letter,
        // then we know the number of occurances.
        if(tc.getGreyCount(iter->first)){
            int exactCount = iter->second.size() +
                std::count(tc.greensBegin(), tc.greensEnd(), iter->first);
            if (occurances != exactCount){
                return true;
            }
        }
        else{
            // Otherwise, we only know minimum number of occurances.
            int minCount = iter->second.size() +
                std::count(tc.greensBegin(), tc.greensEnd(), iter->first);
            if (occurances < minCount){
                return true;
            }
        }
    }
    return false;
}

// Check word against greys, return true if invalid.
bool checkGreys(const std::string& w, const TileColors& tc){
    for(auto iter = tc.greysBegin(); iter != tc.greysEnd(); ++iter){     
        // If we have grey and no green/yellow,
        // then we know there are no occurances of this letter.
        int greenCount = 0;
        for(int i = 0; i < WORD_SIZE; ++i){
            if(tc.getGreenAt(i) == iter->first){
                greenCount++;
            }
        }
        if( (greenCount == 0) && (tc.getYellowCount(iter->first) == 0) ){
            // Check every position.
            for(int i = 0; i < WORD_SIZE; ++i){
                if(iter->first == w[i]){
                    return true;
                }
            }

        }
        else{
            // There are occurances of this letter, but not in this position.
            for(const int& greyPositions : iter->second){
                if(iter->first == w[greyPositions]){
                    // Word has a letter in grey position.
                    return true;
                }
            }
        }
    }
    return false;
}

// Check word against colors map. Return true if valid, false if not valid.
bool checkWord(const std::string& w, const TileColors& tc){
    if(checkGreens(w, tc)) return true;
    if(checkYellows(w, tc)) return true;
    if(checkGreys(w, tc)) return true;
    return false;
};

// Given a list and color results, remove impossible values.
void reduceList(std::vector<std::string>& list, const TileColors& tc){
    list.erase(std::remove_if(list.begin(), list.end(), [&tc](std::string& w){
        return checkWord(w, tc);
    }), list.end());
    return;
}

// Log round results to file.
void logGameResults(const std::vector<std::pair<std::string, int>>& r){
    std::cout << "Writing results to gameResults.csv" << std::endl;
    std::ofstream gameResults;
    gameResults.open("gameResults.csv", std::ios_base::app);
    for(const std::pair<std::string, int>& i : r){
        gameResults << i.first << ',' << i.second << '\n';
    }
    gameResults.close();
    return;
}

// Given an answer, answer list, and total list, play a game until winning.
// Return answer and rounds to win.
std::pair<std::string, int> playGame(const std::string& answer,
    std::vector<std::string> answerList,
    std::vector<std::string> totalList){
    int round = 0;
    std::string guess = "";
    TileColors tc;
    std::cout << "Starting game. Target word: " << answer << std::endl;

    // Play rounds until answer is guessed.
    while(!tc.isSolved()){
        round++;

        // Remove invalid guesses if not in initial state.
        if (guess != ""){
            reduceList(answerList, tc);
            reduceList(totalList, tc);
        }
        
        // Reset yellows.
        tc.clearYellows();
        // Reset greys.
        tc.clearGreys();

        // Calculate new distribution of letters per position of answers.
        std::vector<std::vector<int>> answerCharDist(WORD_SIZE,
                                      std::vector<int>(26, 0)  );
        for(const std::string& word : answerList){
            addToDist(word, answerCharDist);
        }

        // Calculate scores from distribution and return best guess.
        guess = getBestGuess(answerCharDist, totalList);

        // Print round and color if round exceeds 6.
        if(round > 6){
            std::cout << RED;
        }
        std::cout << "Round " << round << ": " << "guessing " << guess
            << RESET << std::endl;

        checkGuess(guess, answer, tc);
    }
    return std::pair<std::string, int>(answer,round);
}

// Ensure all words in list match WORD_SIZE and exit if not.
void validateList(const std::vector<std::string>& l){
    for(const std::string& w : l){
        if (WORD_SIZE != w.size()){
            std::cout << "Error: Word size mismatch." << std::endl
                << "    Target length: " << WORD_SIZE << std::endl
                << "    Actual length: " << w.size() << std::endl
                << "    Invalid word: " << w << std::endl;
            exit(EXIT_FAILURE);
        }
    }
    return;
}

int main() {
    std::cout << "Running..." << std::endl;

    validateList(answers);
    validateList(dictionary);

    // Create total list from answers and dictionary lists.
    std::vector<std::string> totalList = answers;
    totalList.insert(totalList.end(), dictionary.begin(), dictionary.end());

    std::vector<std::pair<std::string, int>> gameResults;
    gameResults.reserve(answers.size());

    // Play a game for every answer in answers list.
    for(const std::string& a : answers){
        gameResults.push_back(playGame(a, answers, totalList));
    }

    // Output the game result (answer,rounds) to file.
    logGameResults(gameResults);
    return 0;
}