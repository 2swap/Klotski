#pragma once

#include <vector>
#include <cstring>
#include <cmath>
#include <set>
#include <unordered_set>
#include "GenericBoard.cpp"

enum C4Result {
    RED,
    YELLOW,
    TIE
};

class C4Board : public GenericBoard {
public:

    const int BOARD_HEIGHT = 6;
    const int BOARD_WIDTH = 7;

    std::string representation;
    int** board;
    std::string blurb = "A connect 4 board.";

    bool symmetrical = true;

    C4Board(std::string representation) : representation(representation) {
        // Allocate memory for the board array
        board = new int*[BOARD_HEIGHT];
        for (int i = 0; i < BOARD_HEIGHT; ++i) {
            board[i] = new int[BOARD_WIDTH];
        }

        fill_board_from_string();
    }

    // Destructor to clean up dynamically allocated memory
    ~C4Board() {
        for (int i = 0; i < BOARD_HEIGHT; ++i) {
            delete[] board[i];
        }
        delete[] board;
    }

    void print() override {
        std::cout << representation << std::endl;
    }

    bool is_solution() override {
        // check horizontally
        for (int row = 0; row < BOARD_HEIGHT; row++) {
            for (int col = 0; col < BOARD_WIDTH - 3; col++) {
                int player = board[row][col];
                if (player != 0 && player == board[row][col+1] && player == board[row][col+2] && player == board[row][col+3]) {
                    return true;
                }
            }
        }
        
        // check vertically
        for (int row = 0; row < BOARD_HEIGHT - 3; row++) {
            for (int col = 0; col < BOARD_WIDTH; col++) {
                int player = board[row][col];
                if (player != 0 && player == board[row+1][col] && player == board[row+2][col] && player == board[row+3][col]) {
                    return true;
                }
            }
        }
        
        // check diagonals
        for (int row = 0; row < BOARD_HEIGHT - 3; row++) {
            for (int col = 0; col < BOARD_WIDTH - 3; col++) {
                int player = board[row][col];
                if (player != 0 && player == board[row+1][col+1] && player == board[row+2][col+2] && player == board[row+3][col+3]) {
                    return true;
                }
            }
        }
        for (int row = 3; row < BOARD_HEIGHT; row++) {
            for (int col = 0; col < BOARD_WIDTH - 3; col++) {
                int player = board[row][col];
                if (player != 0 && player == board[row-1][col+1] && player == board[row-2][col+2] && player == board[row-3][col+3]) {
                    return true;
                }
            }
        }
        
        // no winner
        return false;
    }

    double board_specific_hash() override {
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

    double board_specific_reverse_hash() override {
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

    void fill_board_from_string()
    {
        // Initialize the board to all empty slots
        for (int i = 0; i < BOARD_HEIGHT; i++) {
            for (int j = 0; j < BOARD_WIDTH; j++) {
                board[i][j] = 0;
            }
        }
    
        // Iterate through the moves and fill the board
        int player = 1;
        for (int i = 0; i < representation.size(); i++) {
            int col = representation[i] - '0' - 1; // convert from char to int
            for (int row = BOARD_HEIGHT - 1; row >= 0; row--) {
                if (board[row][col] == 0) {
                    board[row][col] = player;
                    break;
                }
            }
            player = 3 - player; // switch player between 1 and 2
        }
    }

    int countChar(std::string str, char ch) {
        int count = 0;
        
        for(int i = 0; i < str.size(); i++) {
            if(str[i] == ch) {
                count++;
            }
        }
        
        return count;
    }
    
    C4Board* remove_piece(){
        std::string rep = representation.substr(0, representation.size()-1);
        C4Board* new_board = new C4Board(rep);
        return new_board;
    }

    C4Board* move_piece(int piece){
        C4Board* new_board = new C4Board(representation+std::to_string(piece));
        return new_board;
    }

    C4Result who_is_winning(){
        char command[150];
        std::sprintf(command, "echo %s | ~/Unduhan/Fhourstones/SearchGame", representation.c_str());
        std::cout << "Calling fhourstones on " << command << "... ";
        FILE* pipe = popen(command, "r");
        if (!pipe) {
            exit(1);
        }
        char buffer[4096];
        std::string result = "";
        while(!feof(pipe)) {
            if(fgets(buffer, 4096, pipe) != NULL) {
                result += buffer;
            }
        }
        pclose(pipe);
        if (result.find("(0)") != std::string::npos){
            std::cout << "Tie!" << std::endl;
            return TIE;
        }
        else if ((result.find("(+)") != std::string::npos) == (representation.size() % 2 == 0)){
            std::cout << "Red!" << std::endl;
            return RED;
        }
        else{
            std::cout << "Yellow!" << std::endl;
            return YELLOW;
        }
    }

    std::unordered_set<C4Board*> get_neighbors(){
        std::unordered_set<C4Board*> neighbors;

        if (is_solution()) {
            return neighbors;
        }

        for (int i = 1; i <= BOARD_WIDTH; i++) {
            if (countChar(representation, '0'+i) < BOARD_HEIGHT) {
                C4Board* moved = move_piece(i);
                if(moved->who_is_winning() == RED){
                    neighbors.insert(moved);
                } else {
                    delete moved;
                }
            }
        }

        std::cout << get_hash() << ": This board has " << neighbors.size() << " neighbors" << std::endl;

        return neighbors;
    }
};