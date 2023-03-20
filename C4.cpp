#ifndef BOARD_H
#define BOARD_H

#include <vector>
#include <cstring>
#include <cmath>
#include <set>

const int BOARD_HEIGHT = 6;
const int BOARD_WIDTH = 7;

class Board{
public:

    std::string representation;
    int board[BOARD_HEIGHT][BOARD_WIDTH];

	Board(std::string representation): representation(representation) {
        fill_board_from_string();
    }

	void print(){
        std::cout << representation << std::endl;
	}

	bool is_solution(){
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

    double get_hash() {
		if(hash != 0)
			return hash;
        double a = 1;
        for (int i = 0; i < BOARD_HEIGHT; i++) {
            for (int j = 0; j < BOARD_WIDTH; j++) {
                hash += board[i][j] * a;
                a *= 1.21813947;
            }
        }
        return hash;
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
    
    Board* remove_piece(){
        std::string rep = representation.substr(0, representation.size()-1);
        Board* new_board = new Board(rep);
        return new_board;
    }

    Board* move_piece(int piece){
        Board* new_board = new Board(representation+std::to_string(piece));
        return new_board;
    }

    bool fhourstones_losing(char* str, int piece){
        char command[150];
        std::sprintf(command, "echo %s%d | ~/Unduhan/Fhourstones/SearchGame", str, piece);
        FILE* pipe = popen(command, "r");
        if (!pipe) {
            return false;
        }
        char buffer[4096];
        std::string result = "";
        while(!feof(pipe)) {
            if(fgets(buffer, 4096, pipe) != NULL) {
                result += buffer;
            }
        }
        pclose(pipe);
        return (result.find("(-)") != std::string::npos);
    }

	std::unordered_set<Board*> get_neighbors(){
		std::unordered_set<Board*> neighbors;

        if (is_solution()) {
            return neighbors;
        }

        for (int i = 1; i <= BOARD_WIDTH; i++) {
            if (countChar(representation, '0'+i) < BOARD_HEIGHT) {
                Board* moved = move_piece(i);
		        neighbors.insert(moved);
            }
        }

		return neighbors;
	}
private:
	double hash = 0;
};

#endif
