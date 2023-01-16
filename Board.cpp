#ifndef BOARD_H
#define BOARD_H

#include <vector>
#include <cstring>
#include <cmath>
#include <set>

#define EMPTY_SPACE '.'
bool rushhour = false;
bool symmetrical = false;

bool in_bounds(int min, int val, int max){ return min <= val && val < max; }

class Board{
public:

	int h;
	int w;
	char* representation;
	std::unordered_set<char> letters;
	std::string blurb;

	Board(char* filename) {
		std::ifstream file;
		file.open(filename);
		std::string line;

		std::getline(file, line);
		std::stringstream iss(line);
		std::string rushstring;
		iss >> h >> w >> rushstring;
		rushhour = rushstring == "rush";

		representation = new char[h*w+1];

		int y = 0;
		for (int y = 0; y < h; y++){
			std::getline(file, line);
			for (int x = 0; x < w; x++){
				representation[y*w+x] = line[x];
			}
		}

		std::getline(file, blurb);

		representation[h*w] = '\0';
		file.close();
	}

	Board(int h, int w, char* representation): h(h), w(w), representation(representation) {
	}

	~Board(){
		delete[] representation;
	}

	void print(){
		std::cout << std::endl;
		for(int y = 0; y < h; y++) {
			for(int x = 0; x < w; x++) std::cout << representation[y*w+x];
			std::cout << std::endl;
		}
		std::cout << std::endl;
	}

	bool is_solution(){
		if (rushhour)
			return representation[17] == representation[16] && representation[16] != EMPTY_SPACE;
		else {
			if(h==5&&w==4)
				return representation[18]==representation[17]&&representation[14]==representation[13]&&representation[14]==representation[17];
			else
				return false;
		}
	}

	void compute_letters(){
		if(letters.empty()){
			for(int i = 0; i < h*w; i++){
				letters.insert(representation[i]);
			}
			letters.erase(EMPTY_SPACE);
		}
	}

	double get_hash() {
		compute_letters();
		if(hash != 0)
			return hash;
		std::set<double> s; // this set is important for making sure the doubles add in the same order.
		for (const char& letter: letters) {
		    double sum = 0;
			for(int y = 0; y < h; y++)
				for(int x = 0; x < w; x++)
					if(representation[y*w+x] == letter){
						int i=y*w+x;
						sum += sin((i+1)*cbrt(i+2));
					}
			s.insert(cbrt(sum));
		}
		for(double d : s) hash += d;
		return hash;
	}

	double get_reverse_hash() {
		compute_letters();
		if(reverse_hash != 0)
			return reverse_hash;
		std::set<double> s;
		for (const char& letter: letters) {
		    double sum = 0;
			for(int y = 0; y < h; y++)
				for(int x = 0; x < w; x++)
					if(representation[y*w+(w-1-x)] == letter){
						int i=y*w+x;
						sum += sin((i+1)*cbrt(i+2));
					}
			s.insert(cbrt(sum));
		}
		for(double d : s) reverse_hash += d;
		return reverse_hash;
	}

	bool is_left(){
		return get_reverse_hash() > get_hash();
	}

	bool is_right(){
		return get_reverse_hash() < get_hash();
	}

	bool is_center(){
		return get_reverse_hash() == get_hash();
	}

	bool can_move_piece(char letter, int dy, int dx){
		for(int y = 0; y < h; y++)
			for(int x = 0; x < w; x++){
				if(representation[y*w+x] == letter) {
					bool inside = in_bounds(0, y+dy, h) && in_bounds(0, x+dx, w);
					char target = representation[(y+dy)*w+(x+dx)];
					if(!inside || (target != EMPTY_SPACE && target != letter))
						return false;
				}
				else continue;
			}
		return true;
	}

	Board* move_piece(char letter, int dy, int dx){
		char* rep = new char[h*w+1];
		for(int i = 0; i < h*w; i++)
			rep[i] = EMPTY_SPACE;
		representation[h*w] = '\0';

		for(int y = 0; y < h; y++)
			for(int x = 0; x < w; x++){
				int position = y*w+x;
				char letter_here = representation[position];
				if(letter_here == letter) {
					int target = (y+dy)*w+(x+dx);
					rep[target] = letter;
				}
				else if(letter_here != EMPTY_SPACE)
					rep[position] = letter_here;
			}
		Board* new_board = new Board(h, w, rep);
		return new_board;
	}

	std::unordered_set<Board*> get_neighbors(){
		std::unordered_set<Board*> neighbors;

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
private:
	double hash = 0;
	double reverse_hash = 0;
};

#endif
