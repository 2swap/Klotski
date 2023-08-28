#pragma once

#include "C4Board.h"
#include "SteadyState.h"

C4Board::C4Board(std::string representation) {
    // Allocate memory for the board array
    for (int i = 0; i < C4_HEIGHT; ++i) {
        for (int j = 0; j < C4_WIDTH; ++j) {
            board[i][j] = 0;
        }
    }

    fill_board_from_string(representation);
}

std::string disk_col(int i){
    if(i == 1) return "\033[31mx\033[0m";  // Red "x"
    if(i == 2) return "\033[33mo\033[0m";  // Yellow "o"
    return " ";
}

void C4Board::print() {
    std::cout << representation << std::endl;
    for(int y = 0; y < C4_HEIGHT; y++) {
        for(int x = 0; x < C4_WIDTH; x++) {
            std::cout << disk_col(board[y][x]) << " ";
        }
        std::cout << std::endl;
    }
}

bool C4Board::is_legal(int x) const {
    return board[0][x] == 0;
}

int C4Board::random_legal_move() const {
    std::vector<int> legal_columns;

    for (int x = 0; x < C4_WIDTH; ++x) {
        if(is_legal(x))
            legal_columns.push_back(x);
    }

    if (legal_columns.empty()) {
        return -1; // No legal columns available
    }

    int random_index = rand() % legal_columns.size();
    return legal_columns[random_index]+1;
}

C4Result C4Board::who_won() {
    const int v = C4_WIDTH;
    const int w = C4_WIDTH + 1; // there is a space of padding on the right of the bitboard
    const int x = C4_WIDTH + 2; // since otherwise horizontal wins would wrap walls
    if(both_bitboard == 140185576636287ul) // this wont be resilient to other board sizes...
        return TIE;

    for (int i = 0; i < 2; i++){
        const Bitboard b = i==0?red_bitboard:yellow_bitboard;
        if( (b & (b>>1) & (b>>(2*1)) & (b>>(3*1)))
         || (b & (b>>w) & (b>>(2*w)) & (b>>(3*w)))
         || (b & (b>>v) & (b>>(2*v)) & (b>>(3*v)))
         || (b & (b>>x) & (b>>(2*x)) & (b>>(3*x))) )
            return i==0?RED:YELLOW;
    }

    return INCOMPLETE;
}

bool C4Board::is_solution() {
    C4Result winner = who_won();
    return winner == RED || winner == YELLOW;
}

double C4Board::board_specific_hash() {
    double a = 1;
    double hash_in_progress = 0;
    for (int i = 0; i < C4_HEIGHT; i++) {
        for (int j = 0; j < C4_WIDTH; j++) {
            hash_in_progress += board[i][j] * a;
            a *= 1.21813947;
        }
    }
    return hash_in_progress;
}

double C4Board::board_specific_reverse_hash() {
    double a = 1;
    double hash_in_progress = 0;
    for (int i = 0; i < C4_HEIGHT; i++) {
        for (int j = 0; j < C4_WIDTH; j++) {
            hash_in_progress += board[i][C4_WIDTH-1-j] * a;
            a *= 1.21813947;
        }
    }
    return hash_in_progress;
}

void C4Board::fill_board_from_string(std::string rep)
{
    // Initialize the board to all empty slots
    for (int i = 0; i < C4_HEIGHT; i++) {
        for (int j = 0; j < C4_WIDTH; j++) {
            board[i][j] = 0;
        }
    }

    // Iterate through the moves and fill the board
    for (int i = 0; i < rep.size(); i++) {
        play_piece(rep[i]-'0');
    }
}

int C4Board::countChar(std::string str, char ch) {
    int count = 0;
    
    for(int i = 0; i < str.size(); i++) {
        if(str[i] == ch) {
            count++;
        }
    }
    
    return count;
}

C4Board* C4Board::remove_piece(){
    std::string rep = representation.substr(0, representation.size()-1);
    C4Board* new_board = new C4Board(rep);
    return new_board;
}

void C4Board::play_piece(int piece){
    int x = piece - 1; // convert from char to int
    int y = 0;
    for (y = C4_HEIGHT - 1; y >= 0; y--) {
        if (board[y][x] == 0) {
            board[y][x] = representation.size()%2+1;
            break;
        }
    }
    Bitboard add = 1UL<<((1+C4_WIDTH)*y+x);
    both_bitboard += add;
    if(representation.size()%2==0)
        red_bitboard += add;
    else
        yellow_bitboard += add;

    representation+=std::to_string(piece);
}

C4Board* C4Board::child(int piece){
    C4Board* new_board = new C4Board(representation+std::to_string(piece));
    return new_board;
}

C4Result C4Board::who_is_winning(int& work) {
    C4Result cachedResult;
    auto it = cache.find(representation);
    if (it != cache.end()) {
        std::cout << "Using cached result..." << std::endl;
        return it->second;
    }

    char command[150];
    std::sprintf(command, "echo %s | ~/Unduhan/Fhourstones/SearchGame", representation.c_str());
    std::cout << "Calling fhourstones on " << command << "... ";
    FILE* pipe = popen(command, "r");
    if (!pipe) {
        exit(1);
    }
    char buffer[4096];
    std::string result = "";
    while (!feof(pipe)) {
        if (fgets(buffer, 4096, pipe) != NULL) {
            result += buffer;
        }
    }
    pclose(pipe);

    C4Result gameResult;
    size_t workPos = result.find("work = ");
    if (workPos != std::string::npos) {
        work = std::stoi(result.substr(workPos + 7, result.find('\n', workPos) - workPos - 7));
    } else {
        work = -1; // Set work to a default value if not found
    }

    if (result.find("(=)") != std::string::npos) {
        std::cout << "Tie!" << std::endl;
        gameResult = TIE;
    } else if ((result.find("(+)") != std::string::npos) == (representation.size() % 2 == 0)) {
        std::cout << "Red!" << std::endl;
        gameResult = RED;
    } else {
        std::cout << "Yellow!" << std::endl;
        gameResult = YELLOW;
    }

    cache[representation] = gameResult;
    return gameResult;
}

void C4Board::add_all_winning_fhourstones(std::unordered_set<C4Board*>& neighbors){
    for (int i = 1; i <= C4_WIDTH; i++) {
        if (countChar(representation, '0'+i) < C4_HEIGHT) {
            C4Board* moved = child(i);
            int work = -1;
            if(moved->who_is_winning(work) == RED){
                std::cout << moved->representation << std::endl;
                neighbors.insert(moved);
            } else {
                delete moved;
            }
        }
    }
}

int C4Board::get_human_winning_fhourstones() {
    std::vector<int> winning_columns;

    for (int col = 1; col <= C4_WIDTH; col++) {
        if (countChar(representation, '0' + col) < C4_HEIGHT) {
            C4Board* moved = child(col);
            int work = -1;
            C4Result winner = moved->who_is_winning(work);
            if (winner == RED) {
                winning_columns.push_back(col);
            }
            delete moved;
        }
    }

    print();
    if (winning_columns.size() == 1) {
        // Single winning column
        return winning_columns[0];
    } else {
        // Multiple winning columns
        std::cout << "There are multiple winning columns. Please select one:" << std::endl;
        for (size_t i = 0; i < winning_columns.size(); i++) {
            std::cout << "Column " << winning_columns[i] << std::endl;
        }
        
        int choice;
        do {
            std::cout << "Enter your choice: ";
            std::cin >> choice;
        } while (std::find(winning_columns.begin(), winning_columns.end(), choice) == winning_columns.end());

        return choice;
    }
}


int C4Board::get_best_winning_fhourstones() {
    int lowest_work = INT_MAX;
    int lowest_work_move = -1;

    for (int i = 1; i <= C4_WIDTH; i++) {
        if (countChar(representation, '0' + i) < C4_HEIGHT) {
            C4Board* moved = child(i);
            int work = -1;
            C4Result winner = moved->who_is_winning(work);
            if (winner == RED && work < lowest_work) {
                lowest_work = work;
                lowest_work_move = i;
            }
            delete moved;
        }
    }
    return lowest_work_move;
}

int C4Board::get_centermost_winning_fhourstones() {
    int lowest_work = INT_MAX;
    int lowest_work_move = -1;

    for (int i = 1; i <= C4_WIDTH; i++) {
        if (countChar(representation, '0' + i) < C4_HEIGHT) {
            C4Board* moved = child(i);
            int work = -1;
            C4Result winner = moved->who_is_winning(work);
            work = abs(4-i);
            if (winner == RED && work < lowest_work) {
                lowest_work = work;
                lowest_work_move = i;
            }
            delete moved;
        }
    }
    return lowest_work_move;
}

/*bool C4Board::add_children_already_in_graph(std::unordered_set<C4Board*>& neighbors) {
    for (int i = 1; i <= C4_WIDTH; i++) {
        if (countChar(representation, '0' + i) < C4_HEIGHT) {
            C4Board* moved = child(i);
            int work = -1;
            C4Result winner = moved->who_is_winning(work);
            if (winner == RED && work < lowest_work) {
                lowest_work = work;
                lowest_work_move = i;
            }
            if(in graph){
                neighbors.insert(moved);
                return true;
            }
            delete moved;
        }
    }
}*/

void C4Board::add_best_winning_fhourstones(std::unordered_set<C4Board*>& neighbors) {
    C4Board* moved = child(get_human_winning_fhourstones());
    std::cout << moved->representation << std::endl;
    neighbors.insert(moved);
}

void C4Board::add_all_legal_children(std::unordered_set<C4Board*>& neighbors){
    for (int i = 1; i <= C4_WIDTH; i++) {
        if (countChar(representation, '0'+i) < C4_HEIGHT) {
            C4Board* moved = child(i);
            std::cout << moved->representation << std::endl;
            neighbors.insert(moved);
        }
    }
}

void C4Board::add_only_child_steady_state(const SteadyState& ss, std::unordered_set<C4Board*>& neighbors){
    int x = ss.query_steady_state(*this);
    C4Board* moved = child(x);
    std::cout << moved->representation << std::endl;
    neighbors.insert(moved);
}

std::unordered_set<C4Board*> C4Board::get_neighbors(){
    std::unordered_set<C4Board*> neighbors;

    if (is_solution()) {
        return neighbors;
    }

    switch (mode){
        case UNION_WEAK:
            add_all_winning_fhourstones(neighbors);
            break;
        case SIMPLE_WEAK:
            if(representation.size() % 2 == 0){
                add_only_child_steady_state(ss_simple_weak, neighbors);
            }else{
                add_all_legal_children(neighbors);
            }
            break;
        case TRIM_STEADY_STATES:
            if(representation.size() % 2 == 1){ // if it's yellow's move
                SteadyState ss;
                bool found = false;
                for(int i = 0; i < 3 && !found; i++){
                    found = find_steady_state(representation, 200000, ss, true);
                }
                if(found){
                    std::cout << "found a steady state!" << std::endl;
                    break;
                }
                else{
                    std::cout << "Adding children as yellow!" << std::endl;
                    add_all_legal_children(neighbors);
                }
            }else{ // red's move
                std::cout << "Making a good move as red." << std::endl;
                add_best_winning_fhourstones(neighbors);
            }
            break;
    }

    std::cout << "I have " << neighbors.size() << " neighbors!" << std::endl;
    return neighbors;
}
