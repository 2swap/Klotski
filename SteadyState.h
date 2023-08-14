#pragma once

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

const int SS_WIDTH = 7;
const int SS_HEIGHT = 6;

class SteadyState {
public:
    SteadyState();
    SteadyState(const std::array<std::string, SS_HEIGHT>& chars);
    int query_steady_state(const int (&b)[SS_HEIGHT][SS_WIDTH]) const;
    void mutate();
    C4Result play_one_game(const std::string& boardString) const;
    void print() const;
    char steadystate[SS_HEIGHT][SS_WIDTH];


    void write_to_file(const std::string& filename) const {
        std::ofstream file(filename);
        if (file.is_open()) {
            for (int row = 0; row < SS_HEIGHT; ++row) {
                for (int col = 0; col < SS_WIDTH; ++col) {
                    file << steadystate[row][col];
                }
                file << std::endl;
            }
        }
    }

    void read_from_file(const std::string& filename) {
        std::ifstream file(filename);
        if (file.is_open()) {
            for (int row = 0; row < SS_HEIGHT; ++row) {
                for (int col = 0; col < SS_WIDTH; ++col) {
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