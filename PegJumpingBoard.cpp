#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cmath>
#include "GenericBoard.cpp"

class PegJumpingBoard : public GenericBoard {
public:
    int BOARD_SIZE;
    std::vector<char> representation;
    std::string game_name = "peg_jumping";

    PegJumpingBoard(int board_size) : GenericBoard(), BOARD_SIZE(board_size) {
        // Initialize the board with pegs (1) and empty spaces (0)
        for (int i = 0; i < (BOARD_SIZE * (BOARD_SIZE + 1)) / 2; ++i) {
            representation.push_back(1);
        }
        representation[0] = 0; // The starting position is empty
    }

    json get_data() const {
        json data;  // Create a JSON object

        // Store board size and representation
        data["board_size"] = BOARD_SIZE;
        data["representation"] = representation;

        return data;
    }

    void print() const override {
        int row = 0;
        int count = 0;

        for (int i = 0; i < representation.size(); ++i) {
            // Print spaces for formatting
            for (int j = 0; j < BOARD_SIZE - row; ++j) {
                std::cout << " ";
            }

            // Print pegs (1) or empty spaces (0)
            if (representation[i] == 1) {
                std::cout << "1 ";
            } else {
                std::cout << "0 ";
            }

            ++count;
            if (count > row) {
                std::cout << std::endl;
                count = 0;
                ++row;
            }
        }
        std::cout << std::endl;
    }

    int count_pegs() const {
        int peg_count = 0;
        for (char peg : representation) {
            if (peg == 1) {
                ++peg_count;
            }
        }
        return peg_count;
    }

    bool is_solution() override {
        return count_pegs() == 1;
    }

    std::unordered_set<PegJumpingBoard*> get_neighbors() {
        std::unordered_set<PegJumpingBoard*> neighbors;

        for (int i = 0; i < representation.size(); ++i) {
            if (representation[i] == 1) {
                // Try jumping over a peg in different directions
                jumpAndAddNeighbor(neighbors, i, i + 1, i + 2);
                jumpAndAddNeighbor(neighbors, i, i - 1, i - 2);
                jumpAndAddNeighbor(neighbors, i, i + BOARD_SIZE + 1, i + 2 * (BOARD_SIZE + 1));
                jumpAndAddNeighbor(neighbors, i, i - BOARD_SIZE - 1, i - 2 * (BOARD_SIZE + 1));
            }
        }

        return neighbors;
    }

private:
    int calculateBoardSize() const {
        // Calculate the board size based on the representation size
        return static_cast<int>((std::sqrt(1 + 8 * representation.size()) - 1) / 2);
    }
    
    void jumpAndAddNeighbor(std::unordered_set<PegJumpingBoard*>& neighbors, int from, int over, int to) {
        if (isValidJump(from, over, to)) {
            std::vector<char> new_representation = representation;
            new_representation[from] = 0;   // Remove peg from the starting position
            new_representation[over] = 0;  // Remove the peg being jumped over
            new_representation[to] = 1;     // Place the peg in the destination

            PegJumpingBoard* neighbor = new PegJumpingBoard(new_representation);
            neighbors.insert(neighbor);
        }
    }

    bool isValidJump(int from, int over, int to) const {
        // Check if the jump is valid:
        // 1. The 'from', 'over', and 'to' positions are within the board bounds.
        // 2. 'from' has a peg, 'over' has a peg to jump over, and 'to' is an empty space.
        return inBounds(0, from, representation.size()) &&
               inBounds(0, over, representation.size()) &&
               inBounds(0, to, representation.size()) &&
               representation[from] == 1 && representation[over] == 1 && representation[to] == 0;
    }

    bool inBounds(int min, int val, int max) const {
        return min <= val && val < max;
    }
};
