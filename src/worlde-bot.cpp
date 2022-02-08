#include <iostream>
#include <fstream>
#include <algorithm>
#include <map>
#include "lists.h"
#define WORD_SIZE 5

//the following are UBUNTU/LINUX, and MacOS ONLY terminal color codes.
#define RESET   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */
#define BLUE    "\033[34m"      /* Blue */
#define MAGENTA "\033[35m"      /* Magenta */
#define CYAN    "\033[36m"      /* Cyan */
#define WHITE   "\033[37m"      /* White */
#define BOLDBLACK   "\033[1m\033[30m"      /* Bold Black */
#define BOLDRED     "\033[1m\033[31m"      /* Bold Red */
#define BOLDGREEN   "\033[1m\033[32m"      /* Bold Green */
#define BOLDYELLOW  "\033[1m\033[33m"      /* Bold Yellow */
#define BOLDBLUE    "\033[1m\033[34m"      /* Bold Blue */
#define BOLDMAGENTA "\033[1m\033[35m"      /* Bold Magenta */
#define BOLDCYAN    "\033[1m\033[36m"      /* Bold Cyan */
#define BOLDWHITE   "\033[1m\033[37m"      /* Bold White */


void getDist(std::string word, std::vector<std::vector<int>> &solutionDistribution){
    for(int i = 0; i < WORD_SIZE; ++i){
        solutionDistribution[i][word[i]-97]++;
    }
    return;
}

int getScore(std::string word, std::vector<std::vector<int>> &solutionDistribution){
    int sum = 0;
    for(int i = 0; i < WORD_SIZE; ++i){
        sum += solutionDistribution[i][word[i]-97];
    }
    return sum;
}

void checkGuess(std::string guess, std::string answer, std::vector<char> &grn, std::map<char, std::vector<int>> &ylw, std::map<char, std::vector<int>> &gry){
    std::vector<char> lettersNotLit = {};
    std::vector<std::string> highlightMask = {WHITE, WHITE, WHITE, WHITE, WHITE};
    for(int i = 0; i < WORD_SIZE; ++i){
        lettersNotLit.push_back(answer[i]);
    }
    // Identify greens
    for(int i = 0; i < WORD_SIZE; ++i){
        if(guess[i] == answer[i]){
            lettersNotLit.erase(std::find(lettersNotLit.begin(), lettersNotLit.end(), guess[i]));
            highlightMask[i] = GREEN;
            grn[i] = guess[i];
        }
    }
    // Identify yellows
    for(int i = 0; i < WORD_SIZE; ++i){
        if(highlightMask[i] != GREEN){
            if(std::find(lettersNotLit.begin(), lettersNotLit.end(), guess[i]) != lettersNotLit.end()){
                lettersNotLit.erase(std::find(lettersNotLit.begin(), lettersNotLit.end(), guess[i]));
                highlightMask[i] = YELLOW;
                ylw[guess[i]].push_back(i);
            }
        }
    }
    // Identify greys
    for(int i = 0; i < WORD_SIZE; ++i){
        if (highlightMask[i] == WHITE){
            gry[guess[i]].push_back(i);
        }
        std::cout << highlightMask[i] << guess[i] << RESET;
    }
    std::cout << std::endl;
    return;
}

void reduceList(std::vector<std::string> &list, std::vector<char> &grn, std::map<char, std::vector<int>> &ylw, std::map<char, std::vector<int>> &gry){
    int initialSize = list.size();
    int removed = 0;
    // Filter by greens
    std::vector<std::string> removedWords = {};
    for(std::string word : list){
        for(int i = 0; i < WORD_SIZE; ++i){
            if (grn[i] != '_'){
                if (grn[i] != word[i]){
                    // word is not valid
                    // remove and continue
                    removed++;
                    removedWords.push_back(word);
                    i = WORD_SIZE;
                }
            }
        }
    }
    for (std::string wordToRemove : removedWords){
        list.erase(std::find(list.begin(), list.end(), wordToRemove));
    }
    removedWords.clear();

    // Filter by yellows
    for(std::string word : list){

        // For every yellow character
        for(auto iter = ylw.begin(); iter != ylw.end(); ++iter){

            // For every yellow character's position
            for(int yellowPositions : iter->second){
                if(iter->first == word[yellowPositions]){
                    // Word has a letter in yellow position

                    removed++;
                    removedWords.push_back(word);
                }
            }
            int occurances = 0;
            for(int i = 0; i < WORD_SIZE; ++i){
                if(word[i] == iter->first){
                    occurances++;
                }
            }
            // If we have greys of the same letter, then we know the number of that letter
            if(gry.count(iter->first)){
                int exactCount = iter->second.size() + std::count(grn.begin(), grn.end(), iter->first);
                if (occurances != exactCount){
                    removed++;
                    removedWords.push_back(word);
                }
            }
            else{
                // Otherwise, we only know minimum
                int minCount = iter->second.size() + std::count(grn.begin(), grn.end(), iter->first);
                //if occcurances is less than minCount, remove word
                if (occurances < minCount){
                    removed++;
                    removedWords.push_back(word);
                }
            }
        }
    }
    for (std::string wordToRemove : removedWords){
        if(std::find(list.begin(), list.end(), wordToRemove) != list.end()){
            list.erase(std::find(list.begin(), list.end(), wordToRemove));
        }
        else{
            removed--;
        }
    }
    removedWords.clear();

    // Filter by greys
    // For every word
    for(std::string word : list){
        // For every grey character
        for(auto iter = gry.begin(); iter != gry.end(); ++iter){
            // For every grey character's position
            for(int greyPositions : iter->second){
                if(iter->first == word[greyPositions]){
                    // Word has a letter in grey position
                    removed++;
                    removedWords.push_back(word);
                }
            }
        }
    }

    for (std::string wordToRemove : removedWords){
        if(std::find(list.begin(), list.end(), wordToRemove) != list.end()){
            list.erase(std::find(list.begin(), list.end(), wordToRemove));
        }
        else{
            removed--;
        }
    }

    std::cout << "Removed " << removed << " words from list of " << initialSize<< std::endl;
    return;
}

bool isSolved(std::vector<char> greens){
    return std::count(greens.begin(), greens.end(),'_') == 0;
}

int main() {
    std::cout << "Running..." << std::endl;
    std::vector<std::string> totalList = solutions;
    totalList.insert(totalList.end(), dictionary.begin(), dictionary.end());
    std::map<std::string, int> wordScore = {};

    std::ofstream gameResults;
    //gameResults.open("gameResults.txt", std::ios_base::app);

    std::vector<std::string> tmpSolutions(solutions.begin(), solutions.begin()+3);
    for(std::string answer : tmpSolutions){
        // For every answer, play a game.
        int round = 0;
        std::cout << "Starting game. Target word: " << answer << std::endl;
        std::vector<std::string> solutionSpace = solutions;
        std::vector<std::string> totalSpace = totalList;

        // greens
        // list of letter and position
        // list of 5 letters
        std::vector<char> greens {'_','_','_','_','_'};

        // yellows
        // list of letters and positions
        // std::vector<std::pair<char,int>> yellows = {};
        std::map<char, std::vector<int>> yellows;

        // greys
        // list of letters
        // std::vector<char> greys = {};
        std::map<char, std::vector<int>> greys;

        // Calculate distribution of letters per position of answers
        std::vector<std::vector<int>> answerCharDist(WORD_SIZE, std::vector<int>(26, 0));
        for(std::string word : solutionSpace){
            // Add to distribution
            getDist(word, answerCharDist);
        }

        // Determine best guess
        // For each word in total list, calculate "score" to find best first guess
        int maxSum = 0;
        int sum = 0;
        std::string maxWord = "";
        for(std::string word : totalSpace){
            // std::cout << word << std::endl;
            sum = getScore(word, answerCharDist);
            wordScore.insert(std::pair<std::string, int>(word, sum));
            if (sum > maxSum){
                maxSum = sum;
                maxWord = word;
                // std::cout << word << ": " << sum << std::endl;
            }
        }

        
        while(!isSolved(greens)){
            std::cout << "Round " << ++round << ", "<< "guessing " << maxWord << std::endl;
            checkGuess(maxWord, answer, greens, yellows, greys);

            if(isSolved(greens)){
                break;
            }

            std::cout << "Greens: ";
            for(int i = 0 ; i < WORD_SIZE; ++i){
                std::cout << greens[i];
            }
            std::cout << std::endl;
            std::cout << "Yellows: ";
            for(auto iter = yellows.begin(); iter != yellows.end(); ++iter){
                std::cout << iter->first << ": ";
                for(int i : iter->second){
                    std::cout << i << ", ";
                }
            }
            std::cout << std::endl;


            // Reduce solution space
            reduceList(solutionSpace, greens, yellows, greys);

            // Reduce dictionary space
            reduceList(totalSpace, greens, yellows, greys);


            // Remove guess from solution and dictionary spaces
            if(std::find(solutionSpace.begin(), solutionSpace.end(), maxWord) != solutionSpace.end()){
                solutionSpace.erase(std::find(solutionSpace.begin(), solutionSpace.end(), maxWord));
            }
            if(std::find(totalSpace.begin(), totalSpace.end(), maxWord) != totalSpace.end()){
                totalSpace.erase(std::find(totalSpace.begin(), totalSpace.end(), maxWord));
            } 


            // Reset yellows
            yellows.clear();

            // Calculate distribution of letters per position of answers
            std::vector<std::vector<int>> answerCharDist(WORD_SIZE, std::vector<int>(26, 0));
            for(std::string word : solutionSpace){
                // Add to distribution
                getDist(word, answerCharDist);
            }

            // Determine best guess
            // For each word in total list, calculate "score" to find best first guess
            maxSum = 0;
            sum = 0;
            maxWord = "";
            for(std::string word : totalSpace){
                // std::cout << word << std::endl;
                sum = getScore(word, answerCharDist);
                wordScore.insert(std::pair<std::string, int>(word, sum));
                if (sum > maxSum){
                    maxSum = sum;
                    maxWord = word;
                    // std::cout << word << ": " << sum << std::endl;
                }
            }
        }
        gameResults.open("gameResults.txt", std::ios_base::app);
        std::cout << answer << ',' << round << '\n';
        gameResults << answer << ',' << round << '\n';
        gameResults.close();


    }
    //
    return 0;
}