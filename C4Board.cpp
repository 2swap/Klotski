#pragma once

#include "C4Board.h"
#include "SteadyState.h"

C4Board::C4Board(std::string representation) {
    // Allocate memory for the board array
    for (int i = 0; i < BOARD_HEIGHT; ++i) {
        for (int j = 0; j < BOARD_WIDTH; ++j) {
            board[i][j] = 0;
        }
    }

    fill_board_from_string(representation);
}

void C4Board::print() {
    std::cout << representation << std::endl;
    for(int y=0;y<BOARD_HEIGHT;y++){
        for(int x=0;x<BOARD_WIDTH;x++){
            std::cout << board[y][x];
        }
        std::cout << std::endl;
    }
}

int C4Board::random_legal_move() const {
    std::vector<int> legal_columns;

    for (int col = 0; col < BOARD_WIDTH; ++col) {
        if (board[0][col] == 0) {
            legal_columns.push_back(col);
        }
    }

    if (legal_columns.empty()) {
        return -1; // No legal columns available
    }

    int random_index = rand() % legal_columns.size();
    return legal_columns[random_index]+1;
}

C4Result C4Board::who_won() {
    int lastRow = -1;
    int lastCol = -1;

    // Find the column and row of the last piece played
    if (representation.empty()) {
        return INCOMPLETE;
    }else{
        int lastPiece = representation.back() - '0';
        lastCol = lastPiece - 1;
        lastRow = 0;
        while (lastRow < BOARD_HEIGHT && board[lastRow][lastCol] == 0) {
            lastRow++;
        }
    }

    int player = board[lastRow][lastCol];
    C4Result win = static_cast<C4Result>(player);

    // Check horizontally
    int count = 1;
    for (int x = lastCol+1; x<BOARD_WIDTH; x++) {
        if (board[lastRow][x] != player) break;
        count++;
        if(count>=4) return win;
    }
    for (int x = lastCol-1; x>=0; x--) {
        if (board[lastRow][x] != player) break;
        count++;
        if(count>=4) return win;
    }

    // Check vertically
    count = 0;
    if(lastRow<3){
        bool all_match = true;
        for (int dy = 1; dy<4; dy++) {
            all_match &= board[lastRow+dy][lastCol] == player;
        }
        if(all_match)
            return win;
    }

    // Check diagonals
    count = 1;
    for (int dx = 1, dy = 1; dx < 4 && dy < 4; dx++, dy++) {
        if (lastRow + dy >= BOARD_HEIGHT || lastCol + dx >= BOARD_WIDTH || board[lastRow + dy][lastCol + dx] != player) {
            break;
        }
        count++;
        if (count >= 4) return win;
    }
    for (int dx = -1, dy = -1; dx > -4 && dy > -4; dx--, dy--) {
        if (lastRow + dy < 0 || lastCol + dx < 0 || board[lastRow + dy][lastCol + dx] != player) {
            break;
        }
        count++;
        if (count >= 4) return win;
    }

    // Check anti-diagonals
    count = 1;
    for (int dx = 1, dy = -1; dx < 4 && dy > -4; dx++, dy--) {
        if (lastRow + dy < 0 || lastCol + dx >= BOARD_WIDTH || board[lastRow + dy][lastCol + dx] != player) {
            break;
        }
        count++;
        if (count >= 4) return win;
    }
    for (int dx = -1, dy = 1; dx > -4 && dy < 4; dx--, dy++) {
        if (lastRow + dy >= BOARD_HEIGHT || lastCol + dx < 0 || board[lastRow + dy][lastCol + dx] != player) {
            break;
        }
        count++;
        if (count >= 4) return win;
    }

    // Check for a tie
    if (representation.size() == BOARD_HEIGHT*BOARD_WIDTH) {
        return TIE;
    }

    // No winner yet
    return INCOMPLETE;
}

bool C4Board::is_solution() {
    C4Result winner = who_won();
    return winner == RED || winner == YELLOW;
}

double C4Board::board_specific_hash() {
    double a = 1;
    double hash_in_progress = 0;
    for (int i = 0; i < BOARD_HEIGHT; i++) {
        for (int j = 0; j < BOARD_WIDTH; j++) {
            hash_in_progress += board[i][j] * a;
            a *= 1.21813947;
        }
    }
    return hash_in_progress;
}

double C4Board::board_specific_reverse_hash() {
    double a = 1;
    double hash_in_progress = 0;
    for (int i = 0; i < BOARD_HEIGHT; i++) {
        for (int j = 0; j < BOARD_WIDTH; j++) {
            hash_in_progress += board[i][BOARD_WIDTH-1-j] * a;
            a *= 1.21813947;
        }
    }
    return hash_in_progress;
}

void C4Board::fill_board_from_string(std::string rep)
{
    // Initialize the board to all empty slots
    for (int i = 0; i < BOARD_HEIGHT; i++) {
        for (int j = 0; j < BOARD_WIDTH; j++) {
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
    for (int y = BOARD_HEIGHT - 1; y >= 0; y--) {
        if (board[y][x] == 0) {
            board[y][x] = representation.size()%2+1;
            break;
        }
    }
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
    for (int i = 1; i <= BOARD_WIDTH; i++) {
        if (countChar(representation, '0'+i) < BOARD_HEIGHT) {
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

int C4Board::get_best_winning_fhourstones() {
    int lowest_work = INT_MAX;
    int lowest_work_move = -1;

    for (int i = 1; i <= BOARD_WIDTH; i++) {
        if (countChar(representation, '0' + i) < BOARD_HEIGHT) {
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

void C4Board::add_best_winning_fhourstones(std::unordered_set<C4Board*>& neighbors) {
    C4Board* moved = child(get_best_winning_fhourstones());
    std::cout << moved->representation << std::endl;
    neighbors.insert(moved);
}

void C4Board::add_all_legal_children(std::unordered_set<C4Board*>& neighbors){
    for (int i = 1; i <= BOARD_WIDTH; i++) {
        if (countChar(representation, '0'+i) < BOARD_HEIGHT) {
            C4Board* moved = child(i);
            std::cout << moved->representation << std::endl;
            neighbors.insert(moved);
        }
    }
}

void C4Board::add_only_child_steady_state(const SteadyState& ss, std::unordered_set<C4Board*>& neighbors){
    int x = ss.query_steady_state(board);
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
                bool found = find_steady_state(representation, 50000, ss, true);
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
