#pragma once

#include <list>
#include <unordered_set>
#include "SteadyState.h"
#include <random>
#include "C4Board.h"

// Method to populate char** array from std::array of strings
void populate_char_array(const std::array<std::string, SS_HEIGHT>& source, char dest[SS_HEIGHT][SS_WIDTH]);

SteadyState::SteadyState() {
    // Initialize the character array with empty cells
    for (int row = 0; row < SS_HEIGHT; ++row) {
        for (int col = 0; col < SS_WIDTH; ++col) {
            steadystate[row][col] = ' ';
        }
    }
}

SteadyState::SteadyState(const std::array<std::string, SS_HEIGHT>& chars) {
    // Initialize the character array with the provided strings
    populate_char_array(chars, steadystate);
}

int SteadyState::query_steady_state(const int (&b)[SS_HEIGHT][SS_WIDTH]) const {
    // Given a board, use the steady state to determine where to play.
    // Return the x position of the row to play in.

    // First Priority: Obey Miai
    std::unordered_map<char, int> alph;
    for (int x = 0; x < SS_WIDTH; ++x) {
        for (int y = 0; y < SS_HEIGHT; ++y) {
            if(b[y][x] != 0) continue;
            char letter = steadystate[y][x];
            std::find(miai.begin(), miai.end(), letter);
            if (std::find(miai.begin(), miai.end(), letter) != miai.end()) {
                alph[letter] = alph[letter] + 1;
            }
        }
    }

    int num_unpaired_miai = 0;
    char key = 0;
    for (const auto& entry : alph) {
        if (entry.second > 2) {
            return -1;
        }
        if(entry.second == 1){
            key = entry.first;
            num_unpaired_miai++;
        }
    }

    if(num_unpaired_miai > 1) return -7;

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

    // Second Priority: Claimeven and Claimodd
    // First, check there aren't 2 available claimparities
    int return_x = -1;
    std::vector<char> priorities(SS_WIDTH, 'x');
    for (int x = 0; x < SS_WIDTH; ++x) {
        for (int y = SS_HEIGHT - 1; y >= 0; --y) {
            char ss = steadystate[y][x];
            if (b[y][x] == 0) {
                priorities[x] = ss;
                if ((ss == ' ' && y % 2 == 0) || (ss == '|' && y % 2 == 1))
                    if(return_x == -1)
                        return_x = x;
                    else
                        return -5;
                break;
            }
        }
    }
    if(return_x != -1) return return_x+1;

    // Third Priority: Priority Markings
    int x = -1;
    for (char c : priority_list) {
        auto it = std::find(priorities.begin(), priorities.end(), c);
        if (it != priorities.end()) {
            auto next_it = std::find(std::next(it), priorities.end(), c);
            if (next_it == priorities.end()) {
                x = static_cast<int>(std::distance(priorities.begin(), it));
                break;
            }/* else {
                return -6;
            }*/
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

void SteadyState::mutate() {
    int r = rand()%6;
    if(r<2){
        //drop a miai
        char c = rand()%2==1?'@':'#';
        int x = rand()%SS_WIDTH;
        int y = SS_HEIGHT-1;
        for(y; y >= 0; y--)
            if(steadystate[y][x] != '1' && steadystate[y][x] != '2'){
                break;
            }
        if(y>=0 && !is_miai(steadystate[y][x])) steadystate[y][x] = c;
    }



    else if(r==2) {
        // Generate random coordinates until a non-disk cell is found
        int x, y;
        do {
            x = rand()%SS_WIDTH;
            y = rand()%SS_HEIGHT;
        } while (steadystate[y][x] == '1' || steadystate[y][x] == '2');

        // Choose a random replacement character from miai, priority_list, and claims
        std::vector<char> replacement_chars = {'+', '=', '-'};

        char c = replacement_chars[rand()%replacement_chars.size()];

        // Replace the character at the chosen coordinates
        steadystate[y][x] = c;
    }



    else {
        int x = rand()%SS_WIDTH;
        for (int y = 0; y < SS_HEIGHT; ++y) {
            if(steadystate[y][x] != '1' && steadystate[y][x] != '2')
                steadystate[y][x] = r==5?'|':' ';
        }
    }
}

C4Result SteadyState::play_one_game(const std::string& boardString) const {
    C4Board board(boardString);

    C4Result winner = INCOMPLETE;
    while (true) {
        int randomColumn = board.random_legal_move();
        board.play_piece(randomColumn);
        winner = board.who_won();
        if(winner != INCOMPLETE) return winner;

        // Query the steady state and play the determined disk
        int columnToPlay = query_steady_state(board.board);

        if (columnToPlay < 0) {
            return TIE; // TODO this should forfeit
        } else if (columnToPlay >= 1 && columnToPlay <= 7) {
            board.play_piece(columnToPlay);
        }
        winner = board.who_won();
        if(winner != INCOMPLETE) return winner;
    }
}

void SteadyState::print() const {
    for (int row = 0; row < SS_HEIGHT; ++row) {
        for (int col = 0; col < SS_WIDTH; ++col) {
            std::cout << steadystate[row][col] << ' ';
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

SteadyState create_random_steady_state(const int (&initial_board)[SS_HEIGHT][SS_WIDTH]) {
    SteadyState steadyState;

    for (int row = 0; row < SS_HEIGHT; ++row) {
        for (int col = 0; col < SS_WIDTH; ++col) {
            if (initial_board[row][col] == 1) {
                steadyState.steadystate[row][col] = '1';
            } else if (initial_board[row][col] == 2) {
                steadyState.steadystate[row][col] = '2';
            } else {
                steadyState.steadystate[row][col] = ' ';
            }
        }
    }

    for(int i = 0; i < 5; i++){
        steadyState.mutate();
    }

    return steadyState;
}

bool find_steady_state(std::string rep, int num_games, SteadyState& ss, bool verbose) {
    std::cout << "Searching for a steady state..." << std::endl;
    std::vector<SteadyState> steady_states;

    C4Board b(rep);

    // Convert the hash to a string
    std::ostringstream ss_hash_stream;
    ss_hash_stream << std::fixed << std::setprecision(std::numeric_limits<double>::max_digits10) << b.get_hash();
    std::string ss_hash = ss_hash_stream.str();

    // Check if a cached steady state file exists and read from it
    std::string cachedFilename = "steady_states/" + ss_hash + ".ss";
    if (std::ifstream(cachedFilename)) {
        ss.read_from_file(cachedFilename);
        std::cout << "Loaded cached steady state from file." << std::endl;
        return true;
    }

    int best = 1;

    // Generate a lot of random steady states
    int verification = 100000;
    for (int i = 0; i < 500; ++i) {
        steady_states.push_back(create_random_steady_state(b.board));
    }
    int games_played = 0;

    while(true){
        double consecutive_wins = 0;
        int idx = rand()%steady_states.size(); // Randomly select a steady state
        while (true) {
            C4Result col = steady_states[idx].play_one_game(rep);
            games_played++;

            if (col != RED) { // If it loses
                double n = 20*consecutive_wins/best;
                for (int i = 0; i < n; ++i) {
                    int random_idx = rand() % steady_states.size();
                    for(int y = 0; y < SS_HEIGHT; y++){
                        for(int x = 0; x < SS_WIDTH; x++){
                            steady_states[random_idx].steadystate[y][x] = steady_states[idx].steadystate[y][x];
                        }
                    }
                    for(int k = 0; k < n/5; k++) steady_states[random_idx].mutate();
                }
                for(int j = 0; j < n; j++){
                    int random_idx = rand() % steady_states.size();
                    int y = rand()%SS_HEIGHT;
                    int x = rand()%SS_WIDTH;
                    if(!is_miai(steady_states[random_idx].steadystate[y][x]) && !is_miai(steady_states[idx].steadystate[y][x]))
                        steady_states[random_idx].steadystate[y][x] = steady_states[idx].steadystate[y][x];
                }
                if(best < consecutive_wins){
                    best = consecutive_wins;
                    games_played = 0;
                    if(verbose){
                        std::cout << consecutive_wins << " consecutive wins" << std::endl;
                        steady_states[idx].print();
                    }
                }
                if(games_played>num_games+verification) return false;
                break;
            } else {
                if(consecutive_wins > verification){
                    std::cout << "Steady state found after " << games_played << " games." << std::endl;
                    if(verbose){
                        std::cout << consecutive_wins << " consecutive wins" << std::endl;
                        steady_states[idx].print();
                    }
                    ss = steady_states[idx];
                    std::string filename = "steady_states/" + ss_hash + ".ss";
                    steady_states[idx].write_to_file(filename);
                    return true;
                }
                consecutive_wins++;
            }
        }
    }
}


// Method to populate char** array from std::array of strings
void populate_char_array(const std::array<std::string, SS_HEIGHT>& source, char dest[SS_HEIGHT][SS_WIDTH]) {
    for (int i = 0; i < SS_HEIGHT; ++i) {
        for(int j = 0; j < SS_WIDTH; ++j){
            dest[i][j] = source[i][j];
        }
    }
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

void steady_state_unit_tests_problem_3() {
    // Define the initial board configuration
    std::array<std::string, SS_HEIGHT> ss_list = {
        "       ",
        "       ",
        "       ",
        "       ",
        "      @",
        "       "
    };
    SteadyState ss(ss_list);
    int actual = -1;







    int b1[SS_HEIGHT][SS_WIDTH] = {
        {0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0}
    };
    actual = ss.query_steady_state(b1);
    std::cout << actual << std::endl;
    assert(actual == -2);
    std::cout << "Passed test 1!" << std::endl;
}

void steady_state_unit_tests_problem_4() {
    // Define the initial board configuration
    std::array<std::string, SS_HEIGHT> ss_list = {
        " =+2++|",
        " @211||",
        " 11221|",
        " 22112|",
        "-21212|",
        "112112|"
    };
    SteadyState ss(ss_list);
    int actual = -1;







    int b1[SS_HEIGHT][SS_WIDTH] = {
        {0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0}
    };
    actual = ss.query_steady_state(b1);
    std::cout << actual << std::endl;
    assert(actual == -2);
    std::cout << "Passed test 1!" << std::endl;
}

void steady_state_unit_tests(){
    steady_state_unit_tests_problem_1();
    steady_state_unit_tests_problem_2();
    steady_state_unit_tests_problem_3();
    //steady_state_unit_tests_problem_4();
}
