#include <iostream>
#include <algorithm>
#include <map>
#include "lists.h"
#define WORD_SIZE 5

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

int main() {
    std::cout << "Running..." << std::endl;
    std::vector<std::string> totalList = solutions;
    totalList.insert(totalList.end(), dictionary.begin(), dictionary.end());
    std::map<std::string, int> wordScore = {};

    // Calculate distribution of letters per position of answers
    std::vector<std::vector<int>> answerCharDist(WORD_SIZE, std::vector<int>(26, 0));
    for(std::string word : solutions){
        // Add to distribution
        getDist(word, answerCharDist);
    }

    // For each word in total list, calculate "score"
    int maxSum = 0;
    int sum = 0;
    std::string maxWord = "";
    for(std::string word : totalList){
        std::cout << word << std::endl;
        
        sum = getScore(word, answerCharDist);
        wordScore.insert(std::pair<std::string, int>(word, sum));
        if (sum > maxSum){
            maxSum = sum;
            maxWord = word;
            std::cout << word << ": " << sum << std::endl;
        }
    }


    int minSum = maxSum;
    std::string minWord = "";
    for (auto iter = wordScore.begin(); iter != wordScore.end(); ++iter){
        if (iter->second < minSum){
            minSum = iter->second;
            minWord = iter->first;
        }
        std::cout << '\t' << iter->first << '\t' << iter->second << '\n';
    }
    std::cout << "Min word: " << minWord << ": " << minSum << std::endl;
    std::cout << "Max word: " << maxWord << ": " << maxSum << std::endl;
    
    return 0;
}