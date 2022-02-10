#include <iostream>
#include <fstream>
#include <algorithm>
#include <map>
#include <thread>
#include "lists.h"
#include "ansi_colors.h"
#define WORD_SIZE 5

// Standardize solution/answer language

// Add a word to a letter distribution table
void addToDist(std::string word,
    std::vector<std::vector<int>> &solutionDistribution){
    for(int i = 0; i < WORD_SIZE; ++i){
        solutionDistribution[i][word[i]-97]++;
    }
    return;
}

// Get a word's score based on a letter distribution table
int getDistScore(std::string word,
    std::vector<std::vector<int>> &solutionDistribution){
    int sum = 0;
    for(int i = 0; i < WORD_SIZE; ++i){
        sum += solutionDistribution[i][word[i]-97];
    }
    return sum;
}

// Compare a word against an answer and record greens/yellows/greys
void checkGuess(std::string guess, std::string answer,
    std::vector<char> &grn, std::map<char, std::vector<int>> &ylw,
    std::map<char, std::vector<int>> &gry){
    // Keep track of letters not colored for accurate letter counting.
    std::vector<char> lettersRemaining(answer.begin(), answer.end());
    // Use color codes to store color mask
    std::vector<std::string> highlightMask(WORD_SIZE, RESET);
    // Identify greens
    for(int i = 0; i < WORD_SIZE; ++i){
        if(guess[i] == answer[i]){
            lettersRemaining.erase(std::find(lettersRemaining.begin(), lettersRemaining.end(), guess[i]));
            highlightMask[i] = GREEN;
            grn[i] = guess[i];
        }
    }
    // Identify yellows
    for(int i = 0; i < WORD_SIZE; ++i){
        if(highlightMask[i] != GREEN){
            if(std::find(lettersRemaining.begin(), lettersRemaining.end(), guess[i]) != lettersRemaining.end()){
                lettersRemaining.erase(std::find(lettersRemaining.begin(), lettersRemaining.end(), guess[i]));
                highlightMask[i] = YELLOW;
                ylw[guess[i]].push_back(i);
            }
        }
    }
    // Identify greys and print guess result
    for(int i = 0; i < WORD_SIZE; ++i){
        if (highlightMask[i] == RESET){
            gry[guess[i]].push_back(i);
        }
        std::cout << highlightMask[i] << guess[i] << RESET;
    }
    std::cout << std::endl;
    return;
}

// Given a list and color results, remove impossible values
void reduceList(std::vector<std::string> &list, std::vector<char> &grn,
    std::map<char, std::vector<int>> &ylw, std::map<char,
    std::vector<int>> &gry){
    int initialSize = list.size();
    int removed = 0;
    // TODO separate this lambda
    list.erase(std::remove_if(list.begin(), list.end(), [grn, ylw, gry](std::string word){
        // Filter by greens
        for(int i = 0; i < WORD_SIZE; ++i){
            if (grn[i] != '_'){
                if (grn[i] != word[i]){
                    // word is not valid
                    return true;
                }
            }
        }
        // Filter by yellows
        // For every yellow character
        for(auto iter = ylw.begin(); iter != ylw.end(); ++iter){
            // For every yellow character's position
            for(int yellowPositions : iter->second){
                if(iter->first == word[yellowPositions]){
                    // Word has a letter in yellow position, not valid
                    return true;
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
                    return true;
                }
            }
            else{
                // Otherwise, we only know minimum
                int minCount = iter->second.size() + std::count(grn.begin(), grn.end(), iter->first);
                //if occcurances is less than minCount, remove word
                if (occurances < minCount){
                    return true;
                }
            }
        }
        // Filter by greys
        for(auto iter = gry.begin(); iter != gry.end(); ++iter){     
            // If we have grey and no green/yellow, we know there are no occurances of this letter
            int greenCount = 0;
            for(int i = 0; i < WORD_SIZE; ++i){
                if(grn[i] == iter->first){
                    greenCount++;
                }
            }
            int yellowCount = ylw.count(iter->first);
            if( (greenCount == 0) && (yellowCount == 0)){
                // Check every position
                for(int i = 0; i < WORD_SIZE; ++i){
                    if(iter->first == word[i]){
                        return true;
                    }
                }

            }
            else{
                // Otherwise, there are occurances of this letter, but not in this position
                for(int greyPositions : iter->second){
                    if(iter->first == word[greyPositions]){
                        // Word has a letter in grey position
                        return true;
                    }
                }
            }
        }
        return false;
    }), list.end());
    return;
}

// Return true if we have all green letters
bool isSolved(std::vector<char> greens){
    return std::count(greens.begin(), greens.end(),'_') == 0;
}

// Given an answer, answer list, and total list, play a game until winning.
// Output the game result (answer,rounds) to file.
void playGame(std::string answer, std::vector<std::string> answerSpace, std::vector<std::string> totalSpace){
    int round = 0;
    std::cout << "Starting game. Target word: " << answer << std::endl;
    std::vector<std::string> solutionSpace = solutions;
    // TODO convert all colors to same struct
    // greens
    // list of letter and position
    // list of 5 letters
    std::vector<char> greens {'_','_','_','_','_'};
    // yellows
    // list of letters and positions
    std::map<char, std::vector<int>> yellows;
    // greys
    // list of letters and positions
    std::map<char, std::vector<int>> greys;

    // Calculate distribution of letters per position of answers
    std::vector<std::vector<int>> answerCharDist(WORD_SIZE, std::vector<int>(26, 0));
    for(std::string word : solutionSpace){
        // Add to distribution
        addToDist(word, answerCharDist);
    }

    // Record word and score, keeping track of max to determine best guess
    std::map<std::string, int> wordScore = {};
    int maxSum = 0;
    int sum = 0;
    std::string maxWord = "";
    for(std::string word : totalSpace){
        sum = getDistScore(word, answerCharDist);
        wordScore.insert(std::pair<std::string, int>(word, sum));
        if (sum > maxSum){
            maxSum = sum;
            maxWord = word;
        }
    }
    
    // Keep guessing until game is solved
    while(!isSolved(greens)){
        // TODO move to playRound function
        if(++round > 6){
            std::cout << RED;
        }
        std::cout << "Round " << round << ": " << "guessing " << maxWord
            << RESET << std::endl;
        checkGuess(maxWord, answer, greens, yellows, greys);
        if(isSolved(greens)){
            break;
        }
        // Output greens and yellows for debugging
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

        std::cout << "Greys: ";
        for(auto iter = greys.begin(); iter != greys.end(); ++iter){
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
        // TODO replace with erase+remove_if
        if(std::find(solutionSpace.begin(), solutionSpace.end(), maxWord) != solutionSpace.end()){
            solutionSpace.erase(std::find(solutionSpace.begin(), solutionSpace.end(), maxWord));
        }
        if(std::find(totalSpace.begin(), totalSpace.end(), maxWord) != totalSpace.end()){
            totalSpace.erase(std::find(totalSpace.begin(), totalSpace.end(), maxWord));
        } 

        // Reset yellows
        // TODO check if carrying this over helps
        // Possibly carry over and remove new greens to see what remains?
        yellows.clear();

        // Calculate new distribution of letters per position of answers
        std::vector<std::vector<int>> answerCharDist(WORD_SIZE,
                                      std::vector<int>(26, 0)  );
        for(std::string word : solutionSpace){
            addToDist(word, answerCharDist);
        }

        // Calculate score from new distribution to find best guess
        maxSum = 0;
        sum = 0;
        maxWord = "";
        for(std::string word : totalSpace){
            // std::cout << word << std::endl;
            sum = getDistScore(word, answerCharDist);
            wordScore.insert(std::pair<std::string, int>(word, sum));
            if (sum > maxSum){
                maxSum = sum;
                maxWord = word;
            }
        }
    }

    // Write results to file and terminal
    std::ofstream gameResults;
    gameResults.open("gameResults.csv", std::ios_base::app);
    std::cout << answer << ',' << round << '\n';
    gameResults << answer << ',' << round << '\n';
    gameResults.close();
    return;
}

int main() {
    std::cout << "Running..." << std::endl;
    
    // Merge solution and dictionary to get total guess list
    std::vector<std::string> totalList = solutions;
    totalList.insert(totalList.end(), dictionary.begin(), dictionary.end());
    
    // Calculate new distribution of letters per position of answers
    std::vector<std::vector<int>> answerCharDist(WORD_SIZE,
                                    std::vector<int>(26, 0)  );
    for(std::string word : solutions){
        addToDist(word, answerCharDist);
    }
    
    // Modify to test on subset of answer list
    std::vector<std::string> solutionSubset(solutions.begin(), solutions.end());
    for(std::string answer : solutionSubset){
        playGame(answer, solutions, totalList);
    }
    return 0;
}