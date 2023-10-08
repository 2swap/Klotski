int evaluate(C4Result result) {
    if (result == C4Result::RED) {
        return 1;
    } else if (result == C4Result::YELLOW) {
        return -1;
    }
    if(result == INCOMPLETE) {
        std::cout << "evaluating non-terminal node!" << std::endl;
        exit(1);
    }
    return 0; // Game is a tie
}

// a cache for the results of the alphabeta function
std::unordered_map<Bitboard, std::unordered_map<Bitboard, int>> ab_cache;

int alphabeta(C4Board& board, int alpha, int beta, bool maximizingPlayer) {
    // Check if the result is already in the cache
    if (ab_cache.find(board.red_bitboard) != ab_cache.end())
        if(ab_cache[board.red_bitboard].find(board.yellow_bitboard) != ab_cache[board.yellow_bitboard].end())
            return ab_cache[board.red_bitboard][board.yellow_bitboard];

    C4Result who_won = board.who_won();
    if (who_won != INCOMPLETE) return evaluate(who_won);
    int i = 1;
    int iw = board.get_instant_win();
    if(iw != -1) i = iw;
    if (maximizingPlayer){
        int value = -1000;
        for(i; i <= 7; i++){
            if(!board.is_legal(i)) continue;
            C4Board child = board.child(i);
            value = std::max(value, alphabeta(child, alpha, beta, false));
            if (value > beta) break; /* beta cutoff */
            alpha = std::max(alpha, value);
        }
        ab_cache[board.red_bitboard][board.yellow_bitboard] = value; // Cache the result
        return value;
    } else {
        int value = 1000;
        for(i; i <= 7; i++){
            if(!board.is_legal(i)) continue;
            C4Board child = board.child(i);
            value = std::min(value, alphabeta(child, alpha, beta, true));
            if (value < alpha) break; /* alpha cutoff */
            beta = std::min(beta, value);
        }
        ab_cache[board.red_bitboard][board.yellow_bitboard] = value; // Cache the result
        return value;
    }
}

void print_ab_blurb(C4Board initial_board){
    initial_board.print();
    std::cout << std::endl;

    for(int i = 1; i <= 7; i++){
        std::string best_move = "illegal";
        if(initial_board.is_legal(i)){
            C4Board board = initial_board.child(i);
            // Find the best move using alpha-beta search
            int result = alphabeta(board, -1000, 1000, board.is_reds_turn());
            if(result == 0) best_move = "tie";
            if(result == 1) best_move = "red win";
            if(result == -1) best_move = "yellow win";
        }
        std::cout << "Result of " << i << " is: " << best_move << std::endl;
    }
}

void alpha_beta_unit_tests(){
    C4Board b("4444422227575526643666362111777711");
    C4Board b_child1 = b.child(1);
    C4Board b_child3 = b.child(3);
    C4Board b_child5 = b.child(5);

    assert(alphabeta(b_child1, -1000, 1000, b_child1.is_reds_turn()) == 0);
    assert(alphabeta(b_child3, -1000, 1000, b_child3.is_reds_turn()) == 1);
    assert(alphabeta(b_child5, -1000, 1000, b_child5.is_reds_turn()) == -1);

    C4Board b1 = b.child(1);
    C4Board b1_child3 = b1.child(3);
    C4Board b1_child5 = b1.child(5);

    assert(alphabeta(b1_child3, -1000, 1000, b1_child3.is_reds_turn()) == 1);
    assert(alphabeta(b1_child5, -1000, 1000, b1_child5.is_reds_turn()) == 0);
}
