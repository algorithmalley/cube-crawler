#include "rubiks.hpp"
#include <cassert>
#include <iostream>

// clang-format off

std::vector<int> Rubiks::_lcw = {
    /* l */ LEFT + NE, LEFT + E, LEFT + SE, LEFT + N, LEFT + CC, LEFT + S, LEFT + NW, LEFT + W, LEFT + SW,
    /* r */ RIGHT + NW, RIGHT + N, RIGHT + NE, RIGHT + W, RIGHT + CC, RIGHT + E, RIGHT + SW, RIGHT + S, RIGHT + SE,
    /* b */ BACK + NW, BACK + N, UP + SW, BACK + W, BACK + CC, UP + W, BACK + SW, BACK + S, UP + NW,
    /* f */ DOWN + NW, FRONT + N, FRONT + NE, DOWN + W, FRONT + CC, FRONT + E, DOWN + SW, FRONT + S, FRONT + SE,
    /* d */ BACK + SE, DOWN + N, DOWN + NE, BACK + E, DOWN + CC, DOWN + E, BACK + NE, DOWN + S, DOWN + SE,
    /* u */ FRONT + NW, UP + N, UP + NE, FRONT + W, UP + CC, UP + E, FRONT + SW, UP + S, UP + SE
};

std::vector<int> Rubiks::_rcw = {
    /* l */ LEFT + NW, LEFT + N, LEFT + NE, LEFT + W, LEFT + CC, LEFT + E, LEFT + SW, LEFT + S, LEFT + SE,
    /* r */ RIGHT + NE, RIGHT + E, RIGHT + SE, RIGHT + N, RIGHT + CC, RIGHT + S, RIGHT + NW, RIGHT + W, RIGHT + SW,
    /* b */ DOWN + SE, BACK + N, BACK + NE, DOWN + E, BACK + CC, BACK + E, DOWN + NE, BACK + S, BACK + SE,
    /* f */ FRONT + NW, FRONT + N, UP + NE, FRONT + W, FRONT + CC, UP + E, FRONT + SW, FRONT + S, UP + SE,
    /* d */ DOWN + NW, DOWN + N, FRONT + NE, DOWN + W, DOWN + CC, FRONT + E, DOWN + SW, DOWN + S, FRONT + SE,
    /* u */ UP + NW, UP + N, BACK + SW, UP + W, UP + CC, BACK + W, UP + SW, UP + S, BACK + NW
};

std::vector<int> Rubiks::_bcw = {
    /* l */ DOWN + SW, LEFT + N, LEFT + NE, DOWN + S, LEFT + CC, LEFT + E, DOWN + SE, LEFT + S, LEFT + SE,
    /* r */ RIGHT + NW, RIGHT + N, UP + NW, RIGHT + W, RIGHT + CC, UP + N, RIGHT + SW, RIGHT + S, UP + NE,
    /* b */ BACK + NE, BACK + E, BACK + SE, BACK + N, BACK + CC, BACK + S, BACK + NW, BACK + W, BACK + SW,
    /* f */ FRONT + NW, FRONT + N, FRONT + NE, FRONT + W, FRONT + CC, FRONT + E, FRONT + SW, FRONT + S, FRONT + SE,
    /* d */ DOWN + NW, DOWN + N, DOWN + NE, DOWN + W, DOWN + CC, DOWN + E, RIGHT + SE, RIGHT + E, RIGHT + NE,
    /* u */ LEFT + SW, LEFT + W, LEFT + NW, UP + W, UP + CC, UP + E, UP + SW, UP + S, UP + SE
};

std::vector<int> Rubiks::_fcw = {
    /* l */ LEFT + NW, LEFT + N, UP + SE, LEFT + W, LEFT + CC, UP + S, LEFT + SW, LEFT + S, UP + SW,
    /* r */ DOWN + NE, RIGHT + N, RIGHT + NE, DOWN + N, RIGHT + CC, RIGHT + E, DOWN + NW, RIGHT + S, RIGHT + SE,
    /* b */ BACK + NW, BACK + N, BACK + NE, BACK + W, BACK + CC, BACK + E, BACK + SW, BACK + S, BACK + SE,
    /* f */ FRONT + NE, FRONT + E, FRONT + SE, FRONT + N, FRONT + CC, FRONT + S, FRONT + NW, FRONT + W, FRONT + SW,
    /* d */ LEFT + NE, LEFT + E, LEFT + SE, DOWN + W, DOWN + CC, DOWN + E, DOWN + SW, DOWN + S, DOWN + SE,
    /* u */ UP + NW, UP + N, UP + NE, UP + W, UP + CC, UP + E, RIGHT + NW, RIGHT + W, RIGHT + SW
};

std::vector<int> Rubiks::_dcw = {
    /* l */ LEFT + NW, LEFT + N, LEFT + NE, LEFT + W, LEFT + CC, LEFT + E, FRONT + SW, FRONT + S, FRONT + SE,
    /* r */ RIGHT + NW, RIGHT + N, RIGHT + NE, RIGHT + W, RIGHT + CC, RIGHT + E, BACK + SW, BACK + S, BACK + SE,
    /* b */ BACK + NW, BACK + N, BACK + NE, BACK + W, BACK + CC, BACK + E, LEFT + SW, LEFT + S, LEFT + SE,
    /* f */ FRONT + NW, FRONT + N, FRONT + NE, FRONT + W, FRONT + CC, FRONT + E, RIGHT + SW, RIGHT + S, RIGHT + SE,
    /* d */ DOWN + NE, DOWN + E, DOWN + SE, DOWN + N, DOWN + CC, DOWN + S, DOWN + NW, DOWN + W, DOWN + SW,
    /* u */ UP + NW, UP + N, UP + NE, UP + W, UP + CC, UP + E, UP + SW, UP + S, UP + SE
};

std::vector<int> Rubiks::_ucw = {
    /* l */ BACK + NW, BACK + N, BACK + NE, LEFT + W, LEFT + CC, LEFT + E, LEFT + SW, LEFT + S, LEFT + SE,
    /* r */ FRONT + NW, FRONT + N, FRONT + NE, RIGHT + W, RIGHT + CC, RIGHT + E, RIGHT + SW, RIGHT + S, RIGHT + SE,
    /* b */ RIGHT + NW, RIGHT + N, RIGHT + NE, BACK + W, BACK + CC, BACK + E, BACK + SW, BACK + S, BACK + SE,
    /* f */ LEFT + NW, LEFT + N, LEFT + NE, FRONT + W, FRONT + CC, FRONT + E, FRONT + SW, FRONT + S, FRONT + SE,
    /* d */ DOWN + NW, DOWN + N, DOWN + NE, DOWN + W, DOWN + CC, DOWN + E, DOWN + SW, DOWN + S, DOWN + SE,
    /* u */ UP + NE, UP + E, UP + SE, UP + N, UP + CC, UP + S, UP + NW, UP + W, UP + SW
};

// clang-format on

Rubiks::Rubiks()
    : _state(std::string(9, {RED}) + std::string(9, {ORANGE}) + std::string(9, {GREEN}) + std::string(9, {BLUE}) +
             std::string(9, {YELLOW}) + std::string(9, {WHITE}))
{}

Rubiks::Rubiks(std::string const &lrbfdu) : _state(lrbfdu) {}

bool Rubiks::valid() const
{
    std::string centers = {_state[LEFT + CC],  _state[RIGHT + CC], _state[BACK + CC],
                           _state[FRONT + CC], _state[DOWN + CC],  _state[UP + CC]};
    return check_multi_color(_state, 9) && check_multi_color(centers, 1);
}

bool Rubiks::solved() const
{
    return check_single_color(std::string(_state, LEFT, 9)) && check_single_color(std::string(_state, RIGHT, 9)) &&
           check_single_color(std::string(_state, BACK, 9)) && check_single_color(std::string(_state, FRONT, 9)) &&
           check_single_color(std::string(_state, DOWN, 9)) && check_single_color(std::string(_state, UP, 9));
}

void Rubiks::turn(Face face, int n)
{
    assert(n % 4 != 0 && "error: n == 0 (mod 4) has no effect");

    n = n % 4;             // 4 rotations is identity
    n = n < 0 ? n + 4 : n; // express as CW rotation

    std::vector<int> const *p = nullptr;
    switch (face) {
    case LEFT:
        p = &_lcw;
        break;
    case RIGHT:
        p = &_rcw;
        break;
    case BACK:
        p = &_bcw;
        break;
    case FRONT:
        p = &_fcw;
        break;
    case DOWN:
        p = &_dcw;
        break;
    case UP:
        p = &_ucw;
        break;
    }

    while (n-- > 0) {
        run_permutation(*p);
    }
}

void Rubiks::run_permutation(std::vector<int> const &permutation)
{
    assert(permutation.size() == _state.length() && "error: permutation and state must be of equal size");
    std::string copy = _state;
    for (std::size_t i = 0; i < _state.length(); ++i) {
        _state[permutation[i]] = copy[i];
    }
}

bool Rubiks::check_multi_color(std::string const &part, int n) const
{
    int rcount = 0;
    int ocount = 0;
    int gcount = 0;
    int bcount = 0;
    int ycount = 0;
    int wcount = 0;

    for (int i = 0; i < part.length(); i++) {
        rcount += part[i] == RED;
        ocount += part[i] == ORANGE;
        gcount += part[i] == GREEN;
        bcount += part[i] == BLUE;
        ycount += part[i] == YELLOW;
        wcount += part[i] == WHITE;
    }

    if (rcount != n || ocount != n || gcount != n || bcount != n || ycount != n || wcount != n)
        return false;

    return true;
}

bool Rubiks::check_single_color(std::string const &part) const
{
    std::string goal(part.length(), part[0]);
    return part == goal;
}

std::ostream &operator<<(std::ostream &os, Rubiks &cube)
{
    os << "--- --- --- --- --- ---\n";
    os << " l   r   b   f   d   u\n";
    os << "--- --- --- --- --- ---\n";
    for (std::size_t j = 0; j < 9; j += 3) {
        for (std::size_t i = j; i < cube._state.length(); i += 9) {
            os << std::string(cube._state, i, 3) << " ";
        }
        os << "\n";
    }
    os << "--- --- --- --- --- ---\n";
}
