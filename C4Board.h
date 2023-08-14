#pragma once

#include <vector>
#include <cstring>
#include <cmath>
#include <set>
#include <unordered_set>
#include <climits>
#include "GenericBoard.cpp"

enum C4BranchMode {
    UNION_WEAK,
    TRIM_STEADY_STATES,
    SIMPLE_WEAK
};
C4BranchMode mode = TRIM_STEADY_STATES;

/*std::array<std::string, SS_HEIGHT> ss_list = {
    "   |@  ",
    "   |2  ",
    "  2|2  ",
    "  1|1  ",
    "  1|21@",
    "  12122"
};
std::array<std::string, SS_HEIGHT> ss_list = {
    "       ",
    "       ",
    " #1  ++",
    " 12  ==",
    "#21  --",
    "212  @@"
};*/
std::array<std::string, SS_HEIGHT> ss_list = {
    "@ 21+@|",
    "2 12+2|",
    "1 11+2|",
    "2 12+11",
    "1221222",
    "2112121"
};
SteadyState ss_simple_weak(ss_list);
std::unordered_map<std::string, C4Result> cache;

class C4Board : public GenericBoard {
public:

    const int BOARD_HEIGHT = SS_HEIGHT;
    const int BOARD_WIDTH = SS_WIDTH;

    std::string representation;
    int board[SS_HEIGHT][SS_WIDTH];
    std::string blurb = "A connect 4 board.";

    bool symmetrical = false;

    C4Board(std::string representation);
    void print() override;
    int random_legal_move() const;
    C4Result who_won();
    bool is_solution() override;
    double board_specific_hash() override;
    double board_specific_reverse_hash() override;
    void fill_board_from_string(std::string rep);
    int countChar(std::string str, char ch);
    C4Board* remove_piece();
    void play_piece(int piece);
    C4Board* child(int piece);
    C4Result who_is_winning(int& work);
    int get_best_winning_fhourstones();
    void add_best_winning_fhourstones(std::unordered_set<C4Board*>& neighbors);
    void add_all_winning_fhourstones(std::unordered_set<C4Board*>& neighbors);
    void add_all_legal_children(std::unordered_set<C4Board*>& neighbors);
    void add_only_child_steady_state(const SteadyState& ss, std::unordered_set<C4Board*>& neighbors);
    std::unordered_set<C4Board*> get_neighbors();
};

void fhourstones_tests(){
    C4Board b("4444445623333356555216622");
    int work = -1;
    C4Result winner = b.who_is_winning(work);
    assert(work == 8);
    assert(winner == RED);
    b.play_piece(5);
    assert(b.get_best_winning_fhourstones() == 2);
}

void winner_tests(){
    std::list<std::pair<std::string, C4Result>> pairs;
    //verticals
    pairs.emplace_back("4141414", RED);
    pairs.emplace_back("1212121", RED);
    pairs.emplace_back("7171717", RED);
    pairs.emplace_back("777171717", RED);
    pairs.emplace_back("34141414", YELLOW);
    pairs.emplace_back("31212121", YELLOW);
    pairs.emplace_back("37171717", YELLOW);
    //horizontals
    pairs.emplace_back("44444156666222262114155112767377373337355", RED);
    pairs.emplace_back("23534224552467456663", YELLOW);
    //diagonals
    pairs.emplace_back("12344324324", RED);
    pairs.emplace_back("76544564564", RED);
    pairs.emplace_back("126552422441467575776336", YELLOW);
    //ties
    pairs.emplace_back("444444562333365666321755523756177122711172", TIE);
    pairs.emplace_back("343344444217622613116332266627117555775755", TIE);

    int n = 0;
    for (const auto& pair : pairs) {
        const std::string& rep = pair.first;
        C4Result winner = pair.second;

        C4Board b("");
        for (int i = 0; i < rep.size(); i++) {
            b.play_piece(rep[i] - '0'); // Convert char to int
            C4Result observed_winner = b.who_won();
            //std::cout << b.representation << ":" << observed_winner << std::endl;
            assert(observed_winner == (i == rep.size()-1 ? winner : INCOMPLETE));
        }
        n++;
        std::cout << "Passed c4 unit test " << n << "!" << std::endl;
    }
}

void c4_unit_tests() {
    winner_tests();
    fhourstones_tests();
}
