#pragma once

#include <list>
#include <unordered_set>
#include "SteadyState.h"
#include <random>
#include "C4Board.h"

// Method to populate char** array from std::array of strings
void populate_char_array(const std::array<std::string, C4_HEIGHT>& source, char dest[C4_HEIGHT][C4_WIDTH]);

std::vector<char> replacement_chars = {'+', '=', '-'};

SteadyState::SteadyState() {
    // Initialize the character array with empty cells
    for (int row = 0; row < C4_HEIGHT; ++row) {
        for (int col = 0; col < C4_WIDTH; ++col) {
            steadystate[row][col] = ' ';
        }
    }
}

SteadyState::SteadyState(const std::array<std::string, C4_HEIGHT>& chars) {
    // Initialize the character array with the provided strings
    populate_char_array(chars, steadystate);
}

int SteadyState::query_steady_state(const C4Board board) const {
    int b[C4_HEIGHT][C4_WIDTH];
    for (int i = 0; i < C4_HEIGHT; ++i) {
        for (int j = 0; j < C4_WIDTH; ++j) {
            b[i][j] = board.board[i][j];
        }
    }
    // Given a board, use the steady state to determine where to play.
    // Return the x position of the row to play in.

    // First Priority: Make 4 in a row!
    //drop a red piece in each column and see if it wins
    for (int x = 0; x < C4_WIDTH; ++x) {
        C4Board board_copy = board;
        board_copy.play_piece(x+1);
        if(board_copy.who_won() == RED) return x+1;
    }

    // Second Priority: Block opponent lines of 4!
    //drop a yellow piece in each column and see if it wins
    for (int x = 0; x < C4_WIDTH; ++x) {
        C4Board board_copy = board;
        board_copy.play_piece((x+1)%C4_WIDTH+1);
        board_copy.play_piece(x+1);
        if(board_copy.who_won() == YELLOW) return x+1;
    }

    // Third Priority: Make Traps!
    //drop a red piece in each column and see if it makes a trap
    for (int x = 0; x < C4_WIDTH; ++x) {
        C4Board board_copy = board;
        board_copy.play_piece(x+1);
        board_copy.play_piece(0);
        int save_x2 = -10;
        for (int x2 = 0; x2 < C4_WIDTH; ++x2) {
            C4Board board_copy3 = board_copy;
            board_copy3.play_piece(x2+1);
            if(board_copy3.who_won() == RED) {if(save_x2>-1) {return x+1;} save_x2 = x2;}
        }
        if(save_x2 < 0) continue;
        C4Board board_copy2 = board;
        board_copy2.play_piece(x+1);
        board_copy2.play_piece(save_x2+1);
        board_copy2.play_piece(save_x2+1);
        if(board_copy2.who_won() == RED) {return x+1;}
    }

    // Third Priority: Obey Miai
    std::unordered_map<char, int> alph;
    for (int x = 0; x < C4_WIDTH; ++x) {
        for (int y = 0; y < C4_HEIGHT; ++y) {
            if(b[y][x] != 0) continue;
            char letter = steadystate[y][x];
            if (letter == '@' || letter == '#') {
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

    for (int x = 0; x < C4_WIDTH; ++x) {
        for (int y = 0; y < C4_HEIGHT; ++y) {
            if (b[y][x] == 0 && steadystate[y][x] == key) {
                // Forfeit if there is an unpaired unplayable miai
                if (y != C4_HEIGHT - 1 && b[y + 1][x] == 0) {
                    return -2;
                }
                return x+1;
            }
        }
    }

    for (int x = 0; x < C4_WIDTH; ++x) {
        for (int y = 0; y < C4_HEIGHT; ++y) {
            if (b[y][x] == 0 && steadystate[y][x] == '!') {
                if (y == C4_HEIGHT - 1 || b[y + 1][x] != 0) {
                    return x+1;
                }
            }
        }
    }

    // Fourth Priority: Claimeven and Claimodd
    // First, check there aren't 2 available claimparities
    int return_x = -1;
    std::vector<char> priorities(C4_WIDTH, 'x');
    for (int x = 0; x < C4_WIDTH; ++x) {
        for (int y = C4_HEIGHT - 1; y >= 0; --y) {
            char ss = steadystate[y][x];
            if (b[y][x] == 0) {
                priorities[x] = ss;
                bool even = y%2==0;
                if ((ss == ' ' && even) || (ss == '|' && !even))
                    if(return_x == -1)
                        return_x = x;
                    else
                        return -5;
                break;
            }
        }
    }
    if(return_x != -1) return return_x+1;

    // Fifth Priority: Priority Markings
    int x = -1;
    for (char c : priority_list) {
        auto it = std::find(priorities.begin(), priorities.end(), c);
        if (it != priorities.end()) {
            auto next_it = std::find(std::next(it), priorities.end(), c);
            if (next_it == priorities.end()) {
                x = static_cast<int>(std::distance(priorities.begin(), it));
                break;
            } else {
                return -6;
            }
        }
    }

    int y = -1;
    for (int i = 0; i < C4_HEIGHT; ++i) {
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
    int r = rand()%10;

    if(r<2){
        //drop a miai pair
        char c = '@';
        for(int y = 0; y < C4_WIDTH; y++){
            for(int x = 0; x < C4_WIDTH; x++){
                if(steadystate[y][x] == c){
                    steadystate[y][x] = replacement_chars[rand()%replacement_chars.size()];
                }
            }
        }
        for(int i = 0; i < 2; i++){
            int x = rand()%C4_WIDTH;
            int y = C4_HEIGHT-1;
            for(y; y >= 0; y--)
                if(steadystate[y][x] != '1' && steadystate[y][x] != '2'){
                    break;
                }
            if(y>=0 && !is_miai(steadystate[y][x])) steadystate[y][x] = c;
        }
    }


    else if (r < 7){
        int x = rand()%C4_WIDTH;
        int y = C4_HEIGHT-1;
        for(y; y >= 0; y--)
            if(steadystate[y][x] == ' ' || steadystate[y][x] == '|'){
                break;
            }
        if(y>=0) steadystate[y][x] = replacement_chars[rand()%replacement_chars.size()];
    }


    else {
        int x = rand()%C4_WIDTH;
        for (int y = 0; y < C4_HEIGHT; ++y) {
            if(steadystate[y][x] != '1' && steadystate[y][x] != '2')
                steadystate[y][x] = r==9?'|':' ';
        }
    }
}

C4Result SteadyState::play_one_game(const std::string& boardString, std::string& defeat, const std::string& prior_defeat) const {
    std::string defeat_ = "";
    C4Board board(boardString);
    int moveno = 0;

    bool off_the_rails = false;

    C4Result winner = INCOMPLETE;
    while (true) {
        int randomColumn = board.random_legal_move();
        if (moveno < prior_defeat.size() && !off_the_rails){
            int p = prior_defeat[moveno]-'0';
            if(board.is_legal(p))
                randomColumn = p;
            else off_the_rails = true;
            if(rand()%20 == 0) off_the_rails = true;
        }
        moveno++;
        if(randomColumn != -1){
            defeat_ += '0'+randomColumn;
            board.play_piece(randomColumn);
        }
        winner = board.who_won();
        if(winner != INCOMPLETE) {defeat = defeat_; return winner;}

        // Query the steady state and play the determined disk
        int columnToPlay = query_steady_state(board);

        if (columnToPlay < 0) {
            defeat = defeat_;
            return TIE; // TODO this should forfeit
        } else if (columnToPlay >= 1 && columnToPlay <= 7) {
            board.play_piece(columnToPlay);
        }
        winner = board.who_won();
        if(winner != INCOMPLETE) return winner;
    }
}

void SteadyState::print() const {
    for (int row = 0; row < C4_HEIGHT; ++row) {
        for (int col = 0; col < C4_WIDTH; ++col) {
            std::cout << steadystate[row][col] << ' ';
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

SteadyState create_random_steady_state(const int (&initial_board)[C4_HEIGHT][C4_WIDTH]) {
    SteadyState steadyState;

    for (int row = 0; row < C4_HEIGHT; ++row) {
        for (int col = 0; col < C4_WIDTH; ++col) {
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

bool find_steady_state(std::string rep, int num_games, SteadyState& ss, bool verbose, bool allow_cache = true) {
    std::cout << "Searching for a steady state..." << std::endl;

    C4Board b(rep);

    // Convert the hash to a string
    std::ostringstream ss_hash_stream;
    ss_hash_stream << std::fixed << std::setprecision(std::numeric_limits<double>::max_digits10) << b.get_hash();
    std::string ss_hash = ss_hash_stream.str();

    // Check if a cached steady state file exists and read from it
    std::string cachedFilename = "steady_states/" + ss_hash + ".ss";
    if (std::ifstream(cachedFilename) && allow_cache) {
        ss.read_from_file(cachedFilename);
        std::cout << "Loaded cached steady state from file." << std::endl;
        return true;
    }

    std::vector<SteadyState> steady_states;
    int num_coevolution = 100;
    std::list<std::string> last_defeats;
    for(int i = 0; i < num_coevolution; i++){
        last_defeats.push_front("");
    }
    int best = 1;

    // Generate a lot of random steady states
    int verification = 200000;
    for (int i = 0; i < 1000; ++i) {
        steady_states.push_back(create_random_steady_state(b.board));
    }
    int games_played = 0;

    while(true){
        int consecutive_wins = 0;
        int idx = rand()%steady_states.size(); // Randomly select a steady state
        auto coe = last_defeats.begin();
        while (true) {
            C4Result col;
            std::string prior_defeat = "";
            bool use_prior = rand()%5==0;
            if(use_prior){
                prior_defeat = *coe;
                coe++;
                if(coe == last_defeats.end())
                    coe = last_defeats.begin();
            }
            std::string new_defeat = "not_defeated";
            col = steady_states[idx].play_one_game(rep, new_defeat, prior_defeat);
            if(new_defeat != "not_defeated"){
                //std::cout << prior_defeat << " " << new_defeat << " " << "defeated by " << (use_prior?"prior":"random") << std::endl;
                last_defeats.push_front(new_defeat);
                last_defeats.pop_back();
            }
            games_played++;

            if (col != RED) { // If it loses
                std::cout << consecutive_wins << " consecutive wins" << std::endl;
                double n = consecutive_wins/10;
                for (int i = 0; i < n; ++i) {
                    int random_idx = rand() % steady_states.size();
                    for(int y = 0; y < C4_HEIGHT; y++){
                        for(int x = 0; x < C4_WIDTH; x++){
                            steady_states[random_idx].steadystate[y][x] = steady_states[idx].steadystate[y][x];
                        }
                    }
                    steady_states[random_idx].mutate();
                }
                /*for(int j = 0; j < n; j++){
                    int random_idx = rand() % steady_states.size();
                    int y = rand()%C4_HEIGHT;
                    int x = rand()%C4_WIDTH;
                    if(!is_miai(steady_states[random_idx].steadystate[y][x]) && !is_miai(steady_states[idx].steadystate[y][x]))
                        steady_states[random_idx].steadystate[y][x] = steady_states[idx].steadystate[y][x];
                }*/
                if(best < consecutive_wins){
                    best = consecutive_wins;
                    games_played = 0;
                }
                if(games_played>num_games) return false;
                break;
            } else {
                if(consecutive_wins % 100==99 && verbose){
                    std::cout << consecutive_wins << " consecutive wins" << std::endl;
                    steady_states[idx].print();
                }
                if(consecutive_wins > verification){
                    std::cout << "Steady state found after " << games_played << " games." << std::endl;
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
void populate_char_array(const std::array<std::string, C4_HEIGHT>& source, char dest[C4_HEIGHT][C4_WIDTH]) {
    for (int i = 0; i < C4_HEIGHT; ++i) {
        for(int j = 0; j < C4_WIDTH; ++j){
            dest[i][j] = source[i][j];
        }
    }
}

void steady_state_unit_tests_problem_1() {
    // Define the initial board configuration
    std::array<std::string, C4_HEIGHT> ss_list = {
        "       ",
        "       ",
        " #1  ++",
        " 12  ==",
        "#21  --",
        "212  @@"
    };
    SteadyState ss(ss_list);
    int actual = -1;







    C4Board b1("233332212");
    actual = ss.query_steady_state(b1);
    std::cout << actual << std::endl;
    assert(actual == 1);
    std::cout << "Passed test 1!" << std::endl;


    C4Board b2("233332211");
    actual = ss.query_steady_state(b2);
    std::cout << actual << std::endl;
    assert(actual == 2);
    std::cout << "Passed test 2!" << std::endl;


    C4Board b3("233332216");
    actual = ss.query_steady_state(b3);
    std::cout << actual << std::endl;
    assert(actual == 7);
    std::cout << "Passed test 3!" << std::endl;


    C4Board b4("233332217");
    actual = ss.query_steady_state(b4);
    std::cout << actual << std::endl;
    assert(actual == 6);
    std::cout << "Passed test 4!" << std::endl;


    C4Board b5("233332213");
    actual = ss.query_steady_state(b5);
    std::cout << actual << std::endl;
    assert(actual == 3);
    std::cout << "Passed test 5!" << std::endl;


    C4Board b6("233332214");
    actual = ss.query_steady_state(b6);
    std::cout << actual << std::endl;
    assert(actual == 4);
    std::cout << "Passed test 6!" << std::endl;


    C4Board b7("23333221676");
    actual = ss.query_steady_state(b7);
    std::cout << actual << std::endl;
    assert(actual == 6);
    std::cout << "Passed test 7!" << std::endl;


    C4Board b8("233332217677");
    actual = ss.query_steady_state(b8);
    std::cout << actual << std::endl;
    assert(actual == 7);
    std::cout << "Passed test 8!" << std::endl;
}

void steady_state_unit_tests_problem_2() {
    // Define the initial board configuration
    std::array<std::string, C4_HEIGHT> ss_list = {
        "   |@  ",
        "   |1  ",
        "  1|1  ",
        "  2|2  ",
        "  2|12@",
        "  21211"
    };
    SteadyState ss(ss_list);
    int actual = -1;







    C4Board b1("43667555535337");
    actual = ss.query_steady_state(b1);
    std::cout << actual << std::endl;
    assert(actual == 5);
    std::cout << "Passed test 1!" << std::endl;


    C4Board b2("43667555535335");
    actual = ss.query_steady_state(b2);
    std::cout << actual << std::endl;
    assert(actual == 7);
    std::cout << "Passed test 2!" << std::endl;


    C4Board b3("43667555535334");
    actual = ss.query_steady_state(b3);
    std::cout << actual << std::endl;
    assert(actual == 4);
    std::cout << "Passed test 3!" << std::endl;


    C4Board b4("43667555535332");
    actual = ss.query_steady_state(b4);
    std::cout << actual << std::endl;
    assert(actual == 2);
    std::cout << "Passed test 4!" << std::endl;
}

void steady_state_unit_tests_problem_5() {
    // Define the initial board configuration
    std::array<std::string, C4_HEIGHT> ss_list = {
        "------+",
        "------+",
        "------+",
        "------+",
        "------+",
        "------+"
    };
    SteadyState ss(ss_list);
    int actual = -1;







    C4Board b1("121212");
    actual = ss.query_steady_state(b1);
    std::cout << actual << std::endl;
    assert(actual == 1);
    std::cout << "Passed test 1!" << std::endl;


    C4Board b2("123212");
    actual = ss.query_steady_state(b2);
    std::cout << actual << std::endl;
    assert(actual == 2);
    std::cout << "Passed test 2!" << std::endl;


    C4Board b3("4332322441");
    actual = ss.query_steady_state(b3);
    std::cout << actual << std::endl;
    assert(actual == 1);
    std::cout << "Passed test 3!" << std::endl;


    C4Board b4("55567667364242");
    actual = ss.query_steady_state(b4);
    std::cout << actual << std::endl;
    assert(actual == 4);
    std::cout << "Passed test 4!" << std::endl;
}

void steady_state_unit_tests_problem_3() {
    // Define the initial board configuration
    std::array<std::string, C4_HEIGHT> ss_list = {
        "       ",
        "       ",
        "       ",
        "       ",
        "      @",
        "       "
    };
    SteadyState ss(ss_list);
    int actual = -1;



    
    C4Board b1("");
    actual = ss.query_steady_state(b1);
    std::cout << actual << std::endl;
    assert(actual == -2);
    std::cout << "Passed test 1!" << std::endl;
}

void steady_state_unit_tests_problem_4() {
    // Define the initial board configuration
    std::array<std::string, C4_HEIGHT> ss_list = {
        " =+2++|",
        " @211||",
        " 11221|",
        " 22112|",
        "-21212|",
        "112112|"
    };
    SteadyState ss(ss_list);
    int actual = -1;



    C4Board b1("");
    actual = ss.query_steady_state(b1);
    std::cout << actual << std::endl;
    assert(actual == -2);
    std::cout << "Passed test 1!" << std::endl;
}

void steady_state_unit_tests(){
    steady_state_unit_tests_problem_1();
    steady_state_unit_tests_problem_2();
    steady_state_unit_tests_problem_3();
    steady_state_unit_tests_problem_4();
    steady_state_unit_tests_problem_5();
    //steady_state_unit_tests_problem_4();
}
