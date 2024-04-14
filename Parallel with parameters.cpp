#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <ctime>
#include <chrono>
#include <set>
#include <cstdlib>
#include <omp.h>

int simulateHangmanGame(const std::string& wordToGuess) {
    int attemptsLeft = 6;
    std::set<char> guessedLetters;
    std::string currentWord(wordToGuess.size(), '_');

    while (attemptsLeft > 0 && currentWord.find('_') != std::string::npos) {
        char guess = 'a' + (std::rand() % 26); // Random letter guess
        if (guessedLetters.find(guess) == guessedLetters.end()) {
            guessedLetters.insert(guess);

            bool correctGuess = false;
            for (size_t i = 0; i < wordToGuess.size(); ++i) {
                if (wordToGuess[i] == guess) {
                    currentWord[i] = guess;
                    correctGuess = true;
                }
            }

            if (!correctGuess) {
                attemptsLeft--;
            }
        }
    }

    return (currentWord.find('') == std::string::npos) ? 1 : 0; // Win if no '' left
}

std::pair<int, int> simulateHangmanGames(const std::vector<std::string>& wordListChunk, int numGamesPerWord) {
    int wins = 0;
    int totalGames = static_cast<int>(wordListChunk.size()) * numGamesPerWord;

    for (const auto& word : wordListChunk) {
        for (int i = 0; i < numGamesPerWord; ++i) {
            wins += simulateHangmanGame(word);
        }
    }

    return { wins, totalGames };
}

double measureExecutionTime(const std::vector<std::string>& wordList, int numGamesPerWord, int numChunks) {
    auto startTime = std::chrono::steady_clock::now();

    std::vector<std::vector<std::string>> wordListChunks(numChunks);

    // Split word list into chunks
    for (int i = 0; i < numChunks; ++i) {
        int chunkSize = static_cast<int>(wordList.size()) / numChunks;
        int startIdx = i * chunkSize;
        int endIdx = (i == numChunks - 1) ? static_cast<int>(wordList.size()) : (i + 1) * chunkSize;
        wordListChunks[i].assign(wordList.begin() + startIdx, wordList.begin() + endIdx);
    }

    int totalWins = 0;
    int totalGames = 0;

    #pragma omp parallel for num_threads(numChunks) reduction(+:totalWins, totalGames)
    for (int i = 0; i < numChunks; ++i) {
        auto result = simulateHangmanGames(wordListChunks[i], numGamesPerWord);
        totalWins += result.first;
        totalGames += result.second;
    }

    auto endTime = std::chrono::steady_clock::now();
    double totalTime = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count() / 1000.0;

    return totalTime;
}

int main() {
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    // Problem sizes (N) and number of processing elements (P) to test
    std::vector<int> problemSizes = { 1000, 2000, 3000, 4000 };
    std::vector<int> numProcessingElements = { 2, 4, 6, 8 };

    std::cout << "Problem Size (N), Number of Processing Elements (P), Execution Time (seconds)" << std::endl;

    for (int N : problemSizes) {
        for (int P : numProcessingElements) {
            std::vector<std::string> wordList(N, "apple"); // Use a simple word for simulation
            int numGamesPerWord = 1000;
            double executionTime = measureExecutionTime(wordList, numGamesPerWord, P);

            std::cout << N << ", " << P << ", " << executionTime << std::endl;
        }
    }

    return 0;
}