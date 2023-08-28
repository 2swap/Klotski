#pragma once

class C4Board;

#include <unordered_set>
#include <list>
#include <array>

enum C4Result {
    TIE,
    RED,
    YELLOW,
    INCOMPLETE
};

std::vector<char> miai = {'@', '#'};
std::vector<char> priority_list = {'+', '=', '-'};
std::vector<char> claims = {' ', '|'};
std::vector<char> disks = {'1', '2'};

bool is_miai(char c){
    return c == '@' || c == '#';
}

class SteadyState {
public:
    SteadyState();
    SteadyState(const std::array<std::string, C4_HEIGHT>& chars);
    int query_steady_state(const C4Board board) const;
    void mutate();
    C4Result play_one_game(const std::string& boardString, std::string& defeat, const std::string& prior_defeat) const;
    void print() const;
    char steadystate[C4_HEIGHT][C4_WIDTH];


    void write_to_file(const std::string& filename) const {
        std::ofstream file(filename);
        if (file.is_open()) {
            for (int row = 0; row < C4_HEIGHT; ++row) {
                for (int col = 0; col < C4_WIDTH; ++col) {
                    file << steadystate[row][col];
                }
                file << std::endl;
            }
        }
    }

    void read_from_file(const std::string& filename) {
        std::ifstream file(filename);
        if (file.is_open()) {
            for (int row = 0; row < C4_HEIGHT; ++row) {
                for (int col = 0; col < C4_WIDTH; ++col) {
                    char character;
                    if (file.get(character)) {
                        steadystate[row][col] = character;
                    } else {
                        std::cout << "STEADYSTATE CACHE READ ERROR" << std::endl;
                        exit(1);
                    }
                }
            }
        }
    }
};