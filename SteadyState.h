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

};