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

    return (currentWord.find('_') == std::string::npos) ? 1 : 0; // Win if no '_' left
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

int main() {
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    // Increase the word list for more games
    std::vector<std::string> wordList = { "apple", "banana", "cherry", "date", "elderberry",
                                           "fig", "grape", "honeydew", "kiwi", "lemon" };

    const int numGamesPerWord = 10000; // Number of games to play per word
    const int numChunks = 4;

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
    auto startTime = std::chrono::steady_clock::now();

    #pragma omp parallel for num_threads(numChunks) reduction(+:totalWins, totalGames)
    for (int i = 0; i < numChunks; ++i) {
        auto result = simulateHangmanGames(wordListChunks[i], numGamesPerWord);
        totalWins += result.first;
        totalGames += result.second;
    }

    auto endTime = std::chrono::steady_clock::now();
    double totalTime = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count() / 1000.0;

    std::cout << "Total wins: " << totalWins << std::endl;
    std::cout << "Total games played: " << totalGames << std::endl;
    std::cout << "Win rate: " << (static_cast<double>(totalWins) / totalGames) * 100 << "%" << std::endl;
    std::cout << "Total time for " << totalGames << " games: " << totalTime << " seconds" << std::endl;

    return 0;
}
