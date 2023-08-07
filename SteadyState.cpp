#pragma once

#include <list>
#include <unordered_set>

const int SS_WIDTH = 7;
const int SS_HEIGHT = 6;

// Method to populate char** array from std::array of strings
void populate_char_array(const std::array<std::string, SS_HEIGHT>& source, char dest[SS_HEIGHT][SS_WIDTH]);

class SteadyState {
public:
    std::unordered_set<char> miai = {'@', '#'};
    std::list         <char> priority_list = {'+', '=', '-'};
    std::unordered_set<char> claims = {' ', '|'};

    SteadyState() {
        // Initialize the character array with empty cells
        for (int row = 0; row < SS_HEIGHT; ++row) {
            for (int col = 0; col < SS_WIDTH; ++col) {
                steadystate[row][col] = ' ';
            }
        }
    }

    SteadyState(const std::array<std::string, SS_HEIGHT>& chars) {
        // Initialize the character array with the provided strings
        populate_char_array(chars, steadystate);
    }

    int query_steady_state(const int (&b)[SS_HEIGHT][SS_WIDTH]) {
        // Given a board, use the steady state to determine where to play.
        // Return the x position of the row to play in.


        // First Priority: Obey Miai
        std::unordered_map<char, int> alph;
        for (int x = 0; x < SS_WIDTH; ++x) {
            for (int y = 0; y < SS_HEIGHT; ++y) {
                if(b[y][x] != 0) continue;
                char letter = steadystate[y][x];
                if (miai.find(letter) != miai.end()) {
                    alph[letter] = alph[letter] + 1;
                }
            }
        }

        for (const auto& entry : alph) {
            char key = entry.first;
            int value = entry.second;

            if (value > 2) {
                return -1;
            }

            if (value == 1) {
                for (int x = 0; x < SS_WIDTH; ++x) {
                    for (int y = 0; y < SS_HEIGHT; ++y) {
                        if (b[y][x] == 0 && steadystate[y][x] == key) {
                            // Forfeit if there is an unpaired unplayable miai
                            if (y != SS_HEIGHT - 1 && b[y + 1][x] == 0) {
                                return -2;
                            }
                            return x+1;
                        }
                    }
                }
                return -3;
            }
        }

        //Second Priority: Claimeven and Claimodd
        std::vector<char> priorities(SS_WIDTH, 'x');
        for (int x = 0; x < SS_WIDTH; ++x) {
            for (int y = SS_HEIGHT - 1; y >= 0; --y) {
                char ss = steadystate[y][x];
                if (b[y][x] == 0) {
                    priorities[x] = ss;
                    if (ss == ' ' && y % 2 == 0) return x+1;
                    if (ss == '|' && y % 2 == 1) return x+1;
                    break;
                }
            }
        }

        // Third Priority: Priority Markings
        int x = -1;
        for (char i : priority_list) {
            auto it = std::find(priorities.begin(), priorities.end(), i);
            if (it != priorities.end()) {
                x = static_cast<int>(std::distance(priorities.begin(), it));
                break;
            }
        }

        int y = -1;
        for (int i = 0; i < SS_HEIGHT; ++i) {
            if (b[i][x] == 0) {
                y = i;
                break;
            }
        }

        if (y == -1 || x == -1) {
            return -4;
        }

        return x+1;
    }

private:
    char steadystate[SS_HEIGHT][SS_WIDTH];
};

// Method to populate char** array from std::array of strings
void populate_char_array(const std::array<std::string, SS_HEIGHT>& source, char dest[SS_HEIGHT][SS_WIDTH]) {
    for (int i = 0; i < SS_HEIGHT; ++i) {
        for(int j = 0; j < SS_WIDTH; ++j){
            dest[i][j] = source[i][j];
        }
    }
}

SteadyState createSteadyState(const int (&b)[SS_HEIGHT][SS_WIDTH]) {
    SteadyState steady_state;
    //TODO later
    return steady_state;
}

void steady_state_unit_tests_problem_1() {
    // Define the initial board configuration
    std::array<std::string, SS_HEIGHT> ss_list = {
        "       ",
        "       ",
        " #1  ++",
        " 12  ==",
        "#21  --",
        "212  @@"
    };
    SteadyState ss(ss_list);
    int actual = -1;







    int b1[SS_HEIGHT][SS_WIDTH] = {
        {0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0},
        {0, 1, 1, 0, 0, 0, 0},
        {0, 1, 2, 0, 0, 0, 0},
        {0, 2, 1, 0, 0, 0, 0},
        {2, 1, 2, 0, 0, 0, 0}
    };
    actual = ss.query_steady_state(b1);
    std::cout << actual << std::endl;
    assert(actual == 1);
    std::cout << "Passed test 1!" << std::endl;


    int b2[SS_HEIGHT][SS_WIDTH] = {
        {0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0},
        {0, 0, 1, 0, 0, 0, 0},
        {0, 1, 2, 0, 0, 0, 0},
        {1, 2, 1, 0, 0, 0, 0},
        {2, 1, 2, 0, 0, 0, 0}
    };
    actual = ss.query_steady_state(b2);
    std::cout << actual << std::endl;
    assert(actual == 2);
    std::cout << "Passed test 2!" << std::endl;


    int b3[SS_HEIGHT][SS_WIDTH] = {
        {0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0},
        {0, 0, 1, 0, 0, 0, 0},
        {0, 1, 2, 0, 0, 0, 0},
        {0, 2, 1, 0, 0, 0, 0},
        {2, 1, 2, 0, 0, 1, 0}
    };
    actual = ss.query_steady_state(b3);
    std::cout << actual << std::endl;
    assert(actual == 7);
    std::cout << "Passed test 3!" << std::endl;


    int b4[SS_HEIGHT][SS_WIDTH] = {
        {0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0},
        {0, 0, 1, 0, 0, 0, 0},
        {0, 1, 2, 0, 0, 0, 0},
        {0, 2, 1, 0, 0, 0, 0},
        {2, 1, 2, 0, 0, 0, 1}
    };
    actual = ss.query_steady_state(b4);
    std::cout << actual << std::endl;
    assert(actual == 6);
    std::cout << "Passed test 4!" << std::endl;


    int b5[SS_HEIGHT][SS_WIDTH] = {
        {0, 0, 0, 0, 0, 0, 0},
        {0, 0, 1, 0, 0, 0, 0},
        {0, 0, 1, 0, 0, 0, 0},
        {0, 1, 2, 0, 0, 0, 0},
        {0, 2, 1, 0, 0, 0, 0},
        {2, 1, 2, 0, 0, 0, 0}
    };
    actual = ss.query_steady_state(b5);
    std::cout << actual << std::endl;
    assert(actual == 3);
    std::cout << "Passed test 5!" << std::endl;


    int b6[SS_HEIGHT][SS_WIDTH] = {
        {0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0},
        {0, 0, 1, 0, 0, 0, 0},
        {0, 1, 2, 0, 0, 0, 0},
        {0, 2, 1, 0, 0, 0, 0},
        {2, 1, 2, 1, 0, 0, 0}
    };
    actual = ss.query_steady_state(b6);
    std::cout << actual << std::endl;
    assert(actual == 4);
    std::cout << "Passed test 6!" << std::endl;


    int b7[SS_HEIGHT][SS_WIDTH] = {
        {0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0},
        {0, 0, 1, 0, 0, 0, 0},
        {0, 1, 2, 0, 0, 0, 0},
        {0, 2, 1, 0, 0, 1, 0},
        {2, 1, 2, 0, 0, 1, 2}
    };
    actual = ss.query_steady_state(b7);
    std::cout << actual << std::endl;
    assert(actual == 6);
    std::cout << "Passed test 7!" << std::endl;


    int b8[SS_HEIGHT][SS_WIDTH] = {
        {0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0},
        {0, 0, 1, 0, 0, 0, 0},
        {0, 1, 2, 0, 0, 0, 2},
        {0, 2, 1, 0, 0, 0, 1},
        {2, 1, 2, 0, 0, 2, 1}
    };
    actual = ss.query_steady_state(b8);
    std::cout << actual << std::endl;
    assert(actual == 7);
    std::cout << "Passed test 8!" << std::endl;
}

void steady_state_unit_tests_problem_2() {
    // Define the initial board configuration
    std::array<std::string, SS_HEIGHT> ss_list = {
        "   |@  ",
        "   |2  ",
        "  2|2  ",
        "  1|1  ",
        "  1|21@",
        "  12122"
    };
    SteadyState ss(ss_list);
    int actual = -1;







    int b1[SS_HEIGHT][SS_WIDTH] = {
        {0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 2, 0, 0},
        {0, 0, 2, 0, 2, 0, 0},
        {0, 0, 1, 0, 1, 0, 0},
        {0, 0, 1, 0, 2, 1, 1},
        {0, 0, 1, 2, 1, 2, 2}
    };
    actual = ss.query_steady_state(b1);
    std::cout << actual << std::endl;
    assert(actual == 5);
    std::cout << "Passed test 1!" << std::endl;


    int b2[SS_HEIGHT][SS_WIDTH] = {
        {0, 0, 0, 0, 1, 0, 0},
        {0, 0, 0, 0, 2, 0, 0},
        {0, 0, 2, 0, 2, 0, 0},
        {0, 0, 1, 0, 1, 0, 0},
        {0, 0, 1, 0, 2, 1, 0},
        {0, 0, 1, 2, 1, 2, 2}
    };
    actual = ss.query_steady_state(b2);
    std::cout << actual << std::endl;
    assert(actual == 7);
    std::cout << "Passed test 2!" << std::endl;


    int b3[SS_HEIGHT][SS_WIDTH] = {
        {0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 2, 0, 0},
        {0, 0, 2, 0, 2, 0, 0},
        {0, 0, 1, 0, 1, 0, 0},
        {0, 0, 1, 1, 2, 1, 0},
        {0, 0, 1, 2, 1, 2, 2}
    };
    actual = ss.query_steady_state(b3);
    std::cout << actual << std::endl;
    assert(actual == 4);
    std::cout << "Passed test 3!" << std::endl;


    int b4[SS_HEIGHT][SS_WIDTH] = {
        {0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 2, 0, 0},
        {0, 0, 2, 0, 2, 0, 0},
        {0, 0, 1, 0, 1, 0, 0},
        {0, 0, 1, 0, 2, 1, 0},
        {0, 1, 1, 2, 1, 2, 2}
    };
    actual = ss.query_steady_state(b4);
    std::cout << actual << std::endl;
    assert(actual == 2);
    std::cout << "Passed test 4!" << std::endl;
}


void steady_state_unit_tests(){
    steady_state_unit_tests_problem_1();
    steady_state_unit_tests_problem_2();
}