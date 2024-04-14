#include <iostream>
#include <vector>
#include <string>
#include <random>
#include <set>
#include <chrono>
#include <unordered_map>
#include <algorithm>

// Function to simulate a single hangman game
int simulate_hangman_game(const std::string& word_to_guess) {
    int attempts_left = 6;
    std::set<char> guessed_letters;
    std::string current_word(word_to_guess.size(), '_');

    // Letter frequency in English language
    std::string letter_frequency = "etaoinsrhdlucmfywgpbvkxqjz";

    // Map to track letter frequencies in the remaining unknown part of the word
    std::unordered_map<char, int> remaining_letters_frequency;
    for (char c : letter_frequency) {
        remaining_letters_frequency[c] = 0;
    }

    while (attempts_left > 0 && current_word.find('_') != std::string::npos) {
        char guess;

        // Randomly choose a letter with some probability
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> dis(0.0, 1.0);
        if (dis(gen) < 0.3) { // Adjust this probability to balance randomness and strategy
            // Guess randomly
            guess = 'a' + (std::rand() % 26);
        } else {
            // Guess strategically based on adjusted letter frequency
            guess = letter_frequency.front();
            letter_frequency.erase(letter_frequency.begin());
        }

        guessed_letters.insert(guess);

        bool correct_guess = false;
        for (size_t i = 0; i < word_to_guess.size(); ++i) {
            if (word_to_guess[i] == guess) {
                current_word[i] = guess;
                correct_guess = true;
            }
        }

        if (!correct_guess) {
            attempts_left--;
        }

        // Update letter frequencies based on revealed letters
        for (char c = 'a'; c <= 'z'; ++c) {
            if (guessed_letters.find(c) == guessed_letters.end()) {
                for (size_t i = 0; i < word_to_guess.size(); ++i) {
                    if (word_to_guess[i] == c && current_word[i] == '_') {
                        remaining_letters_frequency[c]++;
                    }
                }
            }
        }

        // Sort remaining letters by frequency
        std::sort(letter_frequency.begin(), letter_frequency.end(), [&](char a, char b) {
            return remaining_letters_frequency[a] > remaining_letters_frequency[b];
        });
    }

    return (current_word.find('_') == std::string::npos) ? 1 : 0; // Win or Loss
}

// Function to simulate multiple hangman games
std::pair<int, int> simulate_hangman_games(const std::vector<std::string>& word_list) {
    int wins = 0;
    for (const std::string& word : word_list) {
        wins += simulate_hangman_game(word);
    }
    return std::make_pair(wins, word_list.size());
}

int main() {
    std::vector<std::string> word_list(10000, "apple"); // Reduced number of games
    int num_games = word_list.size();

    auto start_time = std::chrono::steady_clock::now();
    std::pair<int, int> result = simulate_hangman_games(word_list);
    auto end_time = std::chrono::steady_clock::now();

    int total_wins = result.first;
    int total_games = result.second;

    double total_time_seconds = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count() / 1000.0;

    std::cout << "Total wins: " << total_wins << std::endl;
    std::cout << "Total games played: " << total_games << std::endl;
    std::cout << "Win rate: " << static_cast<double>(total_wins) / total_games * 100 << "%" << std::endl;
    std::cout << "Total time for " << total_games << " games: " << total_time_seconds << " seconds" << std::endl;

    return 0;
}