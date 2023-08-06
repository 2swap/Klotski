#pragma once

#include <vector>
#include <cstring>
#include <cmath>
#include <set>

class GenericBoard{
public:
    virtual void print() = 0;
    virtual bool is_solution() = 0;
    virtual double board_specific_hash() = 0;
    virtual double board_specific_reverse_hash() = 0;

    double get_hash() {
        if(hash != 0)
            return hash;
        hash = board_specific_hash();
        return hash;
    }

    double get_reverse_hash() {
        if(reverse_hash != 0)
            return reverse_hash;
        reverse_hash = board_specific_reverse_hash();
        return reverse_hash;
    }

    bool is_left(){return get_reverse_hash() > get_hash();}
    bool is_right(){return get_reverse_hash() < get_hash();}
    bool is_center(){return get_reverse_hash() == get_hash();}

protected:
    double hash = 0;
    double reverse_hash = 0;
};
