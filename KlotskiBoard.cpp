#pragma once

#include <vector>
#include <cstring>
#include <cmath>
#include <set>
#include "GenericBoard.cpp"

#define EMPTY_SPACE '.'

bool in_bounds(int min, int val, int max){ return min <= val && val < max; }

class KlotskiBoard : public GenericBoard {
public:

    int BOARD_HEIGHT;
    int BOARD_WIDTH;
    char* representation;
    std::unordered_set<char> letters;
    std::string blurb;
    bool rushhour = true;
    bool symmetrical = false;

    KlotskiBoard(char* filename) : GenericBoard() {
        std::ifstream file;
        file.open(filename);
        std::string line;

        std::getline(file, line);
        std::stringstream iss(line);
        std::string rushstring;
        iss >> BOARD_HEIGHT >> BOARD_WIDTH >> rushstring;
        rushhour = rushstring == "rush";

        representation = new char[BOARD_HEIGHT*BOARD_WIDTH+1];

        int y = 0;
        for (int y = 0; y < BOARD_HEIGHT; y++){
            std::getline(file, line);
            for (int x = 0; x < BOARD_WIDTH; x++){
                representation[y*BOARD_WIDTH+x] = line[x];
            }
        }

        std::getline(file, blurb);

        representation[BOARD_HEIGHT*BOARD_WIDTH] = '\0';
        file.close();
    }

    KlotskiBoard(int h, int w, char* representation) : GenericBoard(), BOARD_HEIGHT(h), BOARD_WIDTH(w), representation(representation) {
    }

    ~KlotskiBoard(){
        delete[] representation;
    }

    void print() override {
        std::cout << std::endl;
        for(int y = 0; y < BOARD_HEIGHT; y++) {
            for(int x = 0; x < BOARD_WIDTH; x++) std::cout << representation[y*BOARD_WIDTH+x];
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }

    bool is_solution() override {
        if (rushhour)
            return representation[17] == representation[16] && representation[16] != EMPTY_SPACE;
        else {
            if(BOARD_HEIGHT==5&&BOARD_WIDTH==4)
                return representation[18]==representation[17]&&representation[14]==representation[13]&&representation[14]==representation[17];
            else
                return false;
        }
    }

    void compute_letters(){
        if(letters.empty()){
            for(int i = 0; i < BOARD_HEIGHT*BOARD_WIDTH; i++){
                letters.insert(representation[i]);
            }
            letters.erase(EMPTY_SPACE);
        }
    }

    double board_specific_hash() override {
        compute_letters();
        double hash_in_progress = 0;
        std::set<double> s; // this set is important for making sure the doubles add in the same order.
        for (const char& letter: letters) {
            double sum = 0;
            for(int y = 0; y < BOARD_HEIGHT; y++)
                for(int x = 0; x < BOARD_WIDTH; x++)
                    if(representation[y*BOARD_WIDTH+x] == letter){
                        int i=y*BOARD_WIDTH+x;
                        sum += sin((i+1)*cbrt(i+2));
                    }
            s.insert(cbrt(sum));
        }
        for(double d : s) hash_in_progress += d;
        return hash_in_progress;
    }

    double board_specific_reverse_hash() override {
        compute_letters();
        double hash_in_progress = 0;
        std::set<double> s;
        for (const char& letter: letters) {
            double sum = 0;
            for(int y = 0; y < BOARD_HEIGHT; y++)
                for(int x = 0; x < BOARD_WIDTH; x++)
                    if(representation[y*BOARD_WIDTH+(BOARD_WIDTH-1-x)] == letter){
                        int i=y*BOARD_WIDTH+x;
                        sum += sin((i+1)*cbrt(i+2));
                    }
            s.insert(cbrt(sum));
        }
        for(double d : s) hash_in_progress += d;
        return hash_in_progress;
    }

    bool can_move_piece(char letter, int dy, int dx){
        for(int y = 0; y < BOARD_HEIGHT; y++)
            for(int x = 0; x < BOARD_WIDTH; x++){
                if(representation[y*BOARD_WIDTH+x] == letter) {
                    bool inside = in_bounds(0, y+dy, BOARD_HEIGHT) && in_bounds(0, x+dx, BOARD_WIDTH);
                    char target = representation[(y+dy)*BOARD_WIDTH+(x+dx)];
                    if(!inside || (target != EMPTY_SPACE && target != letter))
                        return false;
                }
                else continue;
            }
        return true;
    }

    KlotskiBoard* move_piece(char letter, int dy, int dx){
        char* rep = new char[BOARD_HEIGHT*BOARD_WIDTH+1];
        for(int i = 0; i < BOARD_HEIGHT*BOARD_WIDTH; i++)
            rep[i] = EMPTY_SPACE;
        representation[BOARD_HEIGHT*BOARD_WIDTH] = '\0';

        for(int y = 0; y < BOARD_HEIGHT; y++)
            for(int x = 0; x < BOARD_WIDTH; x++){
                int position = y*BOARD_WIDTH+x;
                char letter_here = representation[position];
                if(letter_here == letter) {
                    int target = (y+dy)*BOARD_WIDTH+(x+dx);
                    rep[target] = letter;
                }
                else if(letter_here != EMPTY_SPACE)
                    rep[position] = letter_here;
            }
        KlotskiBoard* new_KlotskiBoard = new KlotskiBoard(BOARD_HEIGHT, BOARD_WIDTH, rep);
        return new_KlotskiBoard;
    }

    std::unordered_set<KlotskiBoard*> get_neighbors(){
        std::unordered_set<KlotskiBoard*> neighbors;

        compute_letters();

        //for each letter
        for (const char& letter: letters) {
            //for each direction of motion
            for(int dy = -1; dy <= 1; dy++)
                for(int dx = -1; dx <= 1; dx++){
                    if((dx+dy)%2==0) continue;
                    if(rushhour && (letter - 'a' + dy)%2==0) continue;

                    bool movable = can_move_piece(letter, dy, dx);
                    //attempt to move the piece
                    if(movable)
                        neighbors.insert(move_piece(letter, dy, dx));
                }
        }

        return neighbors;
    }
};
