#include "rubiks.hpp"
#include <cassert>
#include <cmath>
#include <iostream>

using namespace std;

namespace {

ostream &operator<<(ostream &os, Rubiks::Nibble const &nibble)
{
    return os << "<" << nibble.face << ", " << nibble.cell << ", " << nibble.color << ">";
}

} // namespace

// clang-format off

vector<int> Rubiks::_tlcw = {
    /* l */ LEFT + NE, LEFT + E, LEFT + SE, LEFT + N, LEFT + CC, LEFT + S, LEFT + NW, LEFT + W, LEFT + SW,
    /* r */ RIGHT + NW, RIGHT + N, RIGHT + NE, RIGHT + W, RIGHT + CC, RIGHT + E, RIGHT + SW, RIGHT + S, RIGHT + SE,
    /* b */ BACK + NW, BACK + N, UP + SW, BACK + W, BACK + CC, UP + W, BACK + SW, BACK + S, UP + NW,
    /* f */ DOWN + NW, FRONT + N, FRONT + NE, DOWN + W, FRONT + CC, FRONT + E, DOWN + SW, FRONT + S, FRONT + SE,
    /* d */ BACK + SE, DOWN + N, DOWN + NE, BACK + E, DOWN + CC, DOWN + E, BACK + NE, DOWN + S, DOWN + SE,
    /* u */ FRONT + NW, UP + N, UP + NE, FRONT + W, UP + CC, UP + E, FRONT + SW, UP + S, UP + SE
};

vector<int> Rubiks::_trcw = {
    /* l */ LEFT + NW, LEFT + N, LEFT + NE, LEFT + W, LEFT + CC, LEFT + E, LEFT + SW, LEFT + S, LEFT + SE,
    /* r */ RIGHT + NE, RIGHT + E, RIGHT + SE, RIGHT + N, RIGHT + CC, RIGHT + S, RIGHT + NW, RIGHT + W, RIGHT + SW,
    /* b */ DOWN + SE, BACK + N, BACK + NE, DOWN + E, BACK + CC, BACK + E, DOWN + NE, BACK + S, BACK + SE,
    /* f */ FRONT + NW, FRONT + N, UP + NE, FRONT + W, FRONT + CC, UP + E, FRONT + SW, FRONT + S, UP + SE,
    /* d */ DOWN + NW, DOWN + N, FRONT + NE, DOWN + W, DOWN + CC, FRONT + E, DOWN + SW, DOWN + S, FRONT + SE,
    /* u */ UP + NW, UP + N, BACK + SW, UP + W, UP + CC, BACK + W, UP + SW, UP + S, BACK + NW
};

vector<int> Rubiks::_tbcw = {
    /* l */ DOWN + SW, LEFT + N, LEFT + NE, DOWN + S, LEFT + CC, LEFT + E, DOWN + SE, LEFT + S, LEFT + SE,
    /* r */ RIGHT + NW, RIGHT + N, UP + NW, RIGHT + W, RIGHT + CC, UP + N, RIGHT + SW, RIGHT + S, UP + NE,
    /* b */ BACK + NE, BACK + E, BACK + SE, BACK + N, BACK + CC, BACK + S, BACK + NW, BACK + W, BACK + SW,
    /* f */ FRONT + NW, FRONT + N, FRONT + NE, FRONT + W, FRONT + CC, FRONT + E, FRONT + SW, FRONT + S, FRONT + SE,
    /* d */ DOWN + NW, DOWN + N, DOWN + NE, DOWN + W, DOWN + CC, DOWN + E, RIGHT + SE, RIGHT + E, RIGHT + NE,
    /* u */ LEFT + SW, LEFT + W, LEFT + NW, UP + W, UP + CC, UP + E, UP + SW, UP + S, UP + SE
};

 vector<int> Rubiks::_tfcw = {
    /* l */ LEFT + NW, LEFT + N, UP + SE, LEFT + W, LEFT + CC, UP + S, LEFT + SW, LEFT + S, UP + SW,
    /* r */ DOWN + NE, RIGHT + N, RIGHT + NE, DOWN + N, RIGHT + CC, RIGHT + E, DOWN + NW, RIGHT + S, RIGHT + SE,
    /* b */ BACK + NW, BACK + N, BACK + NE, BACK + W, BACK + CC, BACK + E, BACK + SW, BACK + S, BACK + SE,
    /* f */ FRONT + NE, FRONT + E, FRONT + SE, FRONT + N, FRONT + CC, FRONT + S, FRONT + NW, FRONT + W, FRONT + SW,
    /* d */ LEFT + NE, LEFT + E, LEFT + SE, DOWN + W, DOWN + CC, DOWN + E, DOWN + SW, DOWN + S, DOWN + SE,
    /* u */ UP + NW, UP + N, UP + NE, UP + W, UP + CC, UP + E, RIGHT + NW, RIGHT + W, RIGHT + SW
};

 vector<int> Rubiks::_tdcw = {
    /* l */ LEFT + NW, LEFT + N, LEFT + NE, LEFT + W, LEFT + CC, LEFT + E, FRONT + SW, FRONT + S, FRONT + SE,
    /* r */ RIGHT + NW, RIGHT + N, RIGHT + NE, RIGHT + W, RIGHT + CC, RIGHT + E, BACK + SW, BACK + S, BACK + SE,
    /* b */ BACK + NW, BACK + N, BACK + NE, BACK + W, BACK + CC, BACK + E, LEFT + SW, LEFT + S, LEFT + SE,
    /* f */ FRONT + NW, FRONT + N, FRONT + NE, FRONT + W, FRONT + CC, FRONT + E, RIGHT + SW, RIGHT + S, RIGHT + SE,
    /* d */ DOWN + NE, DOWN + E, DOWN + SE, DOWN + N, DOWN + CC, DOWN + S, DOWN + NW, DOWN + W, DOWN + SW,
    /* u */ UP + NW, UP + N, UP + NE, UP + W, UP + CC, UP + E, UP + SW, UP + S, UP + SE
};

 vector<int> Rubiks::_tucw = {
    /* l */ BACK + NW, BACK + N, BACK + NE, LEFT + W, LEFT + CC, LEFT + E, LEFT + SW, LEFT + S, LEFT + SE,
    /* r */ FRONT + NW, FRONT + N, FRONT + NE, RIGHT + W, RIGHT + CC, RIGHT + E, RIGHT + SW, RIGHT + S, RIGHT + SE,
    /* b */ RIGHT + NW, RIGHT + N, RIGHT + NE, BACK + W, BACK + CC, BACK + E, BACK + SW, BACK + S, BACK + SE,
    /* f */ LEFT + NW, LEFT + N, LEFT + NE, FRONT + W, FRONT + CC, FRONT + E, FRONT + SW, FRONT + S, FRONT + SE,
    /* d */ DOWN + NW, DOWN + N, DOWN + NE, DOWN + W, DOWN + CC, DOWN + E, DOWN + SW, DOWN + S, DOWN + SE,
    /* u */ UP + NE, UP + E, UP + SE, UP + N, UP + CC, UP + S, UP + NW, UP + W, UP + SW
};

vector<int> Rubiks::_rlcw = {
    /* l */ LEFT + NE, LEFT + E, LEFT + SE, LEFT + N, LEFT + CC, LEFT + S, LEFT + NW, LEFT + W, LEFT + SW,
    /* r */ RIGHT + SW, RIGHT + W, RIGHT + NW, RIGHT + S, RIGHT + CC, RIGHT + N, RIGHT + SE, RIGHT + E, RIGHT + NE,
    /* b */ UP + SE, UP + S, UP + SW, UP + E, UP + CC, UP + W, UP + NE, UP + N, UP + NW,
    /* f */ DOWN + NW, DOWN + N, DOWN + NE, DOWN + W, DOWN + CC, DOWN + E, DOWN + SW, DOWN + S, DOWN + SE,
    /* d */ BACK + SE, BACK + S, BACK + SW, BACK + E, BACK + CC, BACK + W, BACK + NE, BACK + N, BACK + NW,
    /* u */ FRONT + NW, FRONT + N, FRONT + NE, FRONT + W, FRONT + CC, FRONT + E, FRONT + SW, FRONT + S, FRONT + SE
};

vector<int> Rubiks::_rrcw = {
    /* l */ LEFT + SW, LEFT + W, LEFT + NW, LEFT + S, LEFT + CC, LEFT + N, LEFT + SE, LEFT + E, LEFT + NE,
    /* r */ RIGHT + NE, RIGHT + E, RIGHT + SE, RIGHT + N, RIGHT + CC, RIGHT + S, RIGHT + NW, RIGHT + W, RIGHT + SW,
    /* b */ DOWN + SE, DOWN + S, DOWN + SW, DOWN + E, DOWN + CC, DOWN + W, DOWN + NE, DOWN + N, DOWN + NW,
    /* f */ UP + NW, UP + N, UP + NE, UP + W, UP + CC, UP + E, UP + SW, UP + S, UP + SE,
    /* d */ FRONT + NW, FRONT + N, FRONT + NE, FRONT + W, FRONT + CC, FRONT + E, FRONT + SW, FRONT + S, FRONT + SE,
    /* u */ BACK + SE, BACK + S, BACK + SW, BACK + E, BACK + CC, BACK + W, BACK + NE, BACK + N, BACK + NW
};

vector<int> Rubiks::_rbcw = {
    /* l */ DOWN + SW, DOWN + W, DOWN + NW, DOWN + S, DOWN + CC, DOWN + N, DOWN + SE, DOWN + E, DOWN + NE,
    /* r */ UP + SW, UP + W, UP + NW, UP + S, UP + CC, UP + N, UP + SE, UP + E, UP + NE,
    /* b */ BACK + NE, BACK + E, BACK + SE, BACK + N, BACK + CC, BACK + S, BACK + NW, BACK + W, BACK + SW,
    /* f */ FRONT + SW, FRONT + W, FRONT + NW, FRONT + S, FRONT + CC, FRONT + N, FRONT + SE, FRONT + E, FRONT + NE,
    /* d */ RIGHT + SW, RIGHT + W, RIGHT + NW, RIGHT + S, RIGHT + CC, RIGHT + N, RIGHT + SE, RIGHT + E, RIGHT + NE,
    /* u */ LEFT + SW, LEFT + W, LEFT + NW, LEFT + S, LEFT + CC, LEFT + N, LEFT + SE, LEFT + E, LEFT + NE
};

vector<int> Rubiks::_rfcw = {
    /* l */ UP + NE, UP + E, UP + SE, UP + N, UP + CC, UP + S, UP + NW, UP + W, UP + SW,
    /* r */ DOWN + NE, DOWN + E, DOWN + SE, DOWN + N, DOWN + CC, DOWN + S, DOWN + NW, DOWN + W, DOWN + SW,
    /* b */ BACK + SW, BACK + W, BACK + NW, BACK + S, BACK + CC, BACK + N, BACK + SE, BACK + E, BACK + NE,
    /* f */ FRONT + NE, FRONT + E, FRONT + SE, FRONT + N, FRONT + CC, FRONT + S, FRONT + NW, FRONT + W, FRONT + SW,
    /* d */ LEFT + NE, LEFT + E, LEFT + SE, LEFT + N, LEFT + CC, LEFT + S, LEFT + NW, LEFT + W, LEFT + SW,
    /* u */ RIGHT + NE, RIGHT + E, RIGHT + SE, RIGHT + N, RIGHT + CC, RIGHT + S, RIGHT + NW, RIGHT + W, RIGHT + SW,
};

vector<int> Rubiks::_rdcw = {
    /* l */ FRONT + NW, FRONT + N, FRONT + NE, FRONT + W, FRONT + CC, FRONT + E, FRONT + SW, FRONT + S, FRONT + SE,
    /* r */ BACK + NW, BACK + N, BACK + NE, BACK + W, BACK + CC, BACK + E, BACK + SW, BACK + S, BACK + SE,
    /* b */ LEFT + NW, LEFT + N, LEFT + NE, LEFT + W, LEFT + CC, LEFT + E, LEFT + SW, LEFT + S, LEFT + SE,
    /* f */ RIGHT + NW, RIGHT + N, RIGHT + NE, RIGHT + W, RIGHT + CC, RIGHT + E, RIGHT + SW, RIGHT + S, RIGHT + SE,
    /* d */ DOWN + NE, DOWN + E, DOWN + SE, DOWN + N, DOWN + CC, DOWN + S, DOWN + NW, DOWN + W, DOWN + SW,
    /* u */ UP + SW, UP + W, UP + NW, UP + S, UP + CC, UP + N, UP + SE, UP + E, UP + NE
};

vector<int> Rubiks::_rucw = {
    /* l */ BACK + NW, BACK + N, BACK + NE, BACK + W, BACK + CC, BACK + E, BACK + SW, BACK + S, BACK + SE,
    /* r */ FRONT + NW, FRONT + N, FRONT + NE, FRONT + W, FRONT + CC, FRONT + E, FRONT + SW, FRONT + S, FRONT + SE,
    /* b */ RIGHT + NW, RIGHT + N, RIGHT + NE, RIGHT + W, RIGHT + CC, RIGHT + E, RIGHT + SW, RIGHT + S, RIGHT + SE,
    /* f */ LEFT + NW, LEFT + N, LEFT + NE, LEFT + W, LEFT + CC, LEFT + E, LEFT + SW, LEFT + S, LEFT + SE,
    /* d */ DOWN + SW, DOWN + W, DOWN + NW, DOWN + S, DOWN + CC, DOWN + N, DOWN + SE, DOWN + E, DOWN + NE,
    /* u */ UP + NE, UP + E, UP + SE, UP + N, UP + CC, UP + S, UP + NW, UP + W, UP + SW
};

array<pair<Rubiks::Face, Rubiks::Cell>, 54> Rubiks::_mscp2 = {{
    /* l */ {}, {UP, W}, {}, {BACK, E}, {}, {FRONT, W}, {}, {DOWN, W}, {},
    /* r */ {}, {UP, E}, {}, {FRONT, E}, {}, {BACK, W}, {}, {DOWN, E}, {},
    /* b */ {}, {UP, N}, {}, {RIGHT, E}, {}, {LEFT, W}, {}, {DOWN, S}, {},
    /* f */ {}, {UP, S}, {}, {LEFT, E}, {}, {RIGHT, W}, {}, {DOWN, N}, {},
    /* d */ {}, {FRONT, S}, {}, {LEFT, S}, {}, {RIGHT, S}, {}, {BACK, S}, {},
    /* u */ {}, {BACK, N}, {}, {LEFT, N}, {}, {RIGHT, N}, {}, {FRONT, N}, {}
}};

array<pair<Rubiks::Face, Rubiks::Cell>, 54> Rubiks::_mcp2 = {{
    /* l */ {BACK, NE}, {}, {FRONT, NW}, {}, {}, {}, {BACK, SE}, {}, {FRONT, SW},
    /* r */ {FRONT, NE}, {}, {BACK, NW}, {}, {}, {}, {FRONT, SE}, {}, {BACK, SW},
    /* b */ {RIGHT, NE}, {}, {LEFT, NW}, {}, {}, {}, {RIGHT, SE}, {}, {LEFT, SW},
    /* f */ {LEFT, NE}, {}, {RIGHT, NW}, {}, {}, {}, {LEFT, SE}, {}, {RIGHT, SW},
    /* d */ {LEFT, SE}, {}, {RIGHT, SW}, {}, {}, {}, {LEFT, SW}, {}, {RIGHT, SE},
    /* u */ {LEFT, NW}, {}, {RIGHT, NE}, {}, {}, {}, {LEFT, NE}, {}, {RIGHT, NW}
}};

array<pair<Rubiks::Face, Rubiks::Cell>, 54> Rubiks::_mcp3 = {{
    /* l */ {UP, NW}, {}, {UP, SW}, {}, {}, {}, {DOWN, SW}, {}, {DOWN, NW},
    /* r */ {UP, SE}, {}, {UP, NE}, {}, {}, {}, {DOWN, NE}, {}, {DOWN, SE},
    /* b */ {UP, NE}, {}, {UP, NW}, {}, {}, {}, {DOWN, SE}, {}, {DOWN, SW},
    /* f */ {UP, SW}, {}, {UP, SE}, {}, {}, {}, {DOWN, NW}, {}, {DOWN, NE},
    /* d */ {FRONT, SW}, {}, {FRONT, SE}, {}, {}, {}, {BACK, SE}, {}, {BACK, SW},
    /* u */ {BACK, NE}, {}, {BACK, NW}, {}, {}, {}, {FRONT, NW}, {}, {FRONT, NE}
}};

// clang-format on

Rubiks::Rubiks()
    : _state(string(9, {RED}) + string(9, {ORANGE}) + string(9, {GREEN}) + string(9, {BLUE}) + string(9, {YELLOW}) +
             string(9, {WHITE}))
{}

Rubiks::Rubiks(string const &lrbfdu) : _state(lrbfdu)
{
    if (!valid())
    {
        throw invalid_argument("lrbfdu: invalid Rubik's Cube representation");
    }
}

bool Rubiks::valid() const
{
    string centers = {_state[LEFT + CC],  _state[RIGHT + CC], _state[BACK + CC],
                      _state[FRONT + CC], _state[DOWN + CC],  _state[UP + CC]};
    return check_multi_color(_state, 9) && check_multi_color(centers, 1);
}

bool Rubiks::solved() const
{
    return check_single_color(string(_state, LEFT, 9)) && check_single_color(string(_state, RIGHT, 9)) &&
           check_single_color(string(_state, BACK, 9)) && check_single_color(string(_state, FRONT, 9)) &&
           check_single_color(string(_state, DOWN, 9)) && check_single_color(string(_state, UP, 9));
}

double Rubiks::entropy() const
{
    double entropy = 1.0;
    entropy *= count_distinct_colors(string(_state, LEFT, 9));
    entropy *= count_distinct_colors(string(_state, RIGHT, 9));
    entropy *= count_distinct_colors(string(_state, BACK, 9));
    entropy *= count_distinct_colors(string(_state, FRONT, 9));
    entropy *= count_distinct_colors(string(_state, DOWN, 9));
    entropy *= count_distinct_colors(string(_state, UP, 9));
    entropy -= 1.0;
    entropy /= pow(6, 6);
    return entropy;
}

Rubiks::CenterPiece Rubiks::center_piece(Face face) const
{
    Nibble nibble{face, Cell::CC, color(face, Cell::CC)};
    return make_tuple(nibble);
}

array<Rubiks::SideCenterPiece, 4> Rubiks::side_center_pieces(Color color) const
{
    array<Rubiks::SideCenterPiece, 4> result;
    size_t curr_piece = 0;
    auto nibbles = find_nibbles(color);
    for (auto const &nibble1 : nibbles)
    {
        if (nibble1.cell % 2 == 1) // the side centers are odd
        {
            auto nibble2 = match_nibble(nibble1, 2);
            result[curr_piece++] = make_pair(nibble1, nibble2);
        }
    }
    return result;
}

array<Rubiks::CornerPiece, 4> Rubiks::corner_pieces(Color color) const
{
    array<Rubiks::CornerPiece, 4> result;
    size_t curr_piece = 0;
    auto nibbles = find_nibbles(color);
    for (auto const &nibble1 : nibbles)
    {
        if (nibble1.cell == Cell::CC) // CC also even, so need to skip
            continue;

        if (nibble1.cell % 2 == 0) // the corners are even
        {
            auto nibble2 = match_nibble(nibble1, 2);
            auto nibble3 = match_nibble(nibble1, 3);
            result[curr_piece++] = make_tuple(nibble1, nibble2, nibble3);
        }
    }
    return result;
}

vector<Rubiks::Nibble> Rubiks::find_nibbles(Color color) const
{
    vector<Nibble> result;
    size_t pos = _state.find(color, 0);
    while (pos != string::npos)
    {
        Face face = (Face)((pos / 9) * 9);
        Cell cell = (Cell)(pos % 9);
        result.push_back({face, cell, color});
        pos = _state.find(color, pos + 1);
    }
    return result;
}

Rubiks::Nibble Rubiks::match_nibble(Nibble const &nibble, size_t n) const
{
    assert((n == 1 || n == 2 || n == 3) && "error: can only match nibble with 1st, 2nd or 3rd conjugate");
    assert((nibble.cell % 2 == 0 || (nibble.cell % 2 == 1 && n != 3)) && "error: side center has no 3rd conjugate");

    // 1st conjugate is just the nibble itself
    if (n == 1)
        return nibble;

    // for centers the 1st, 2nd and 3rd conjugate are all the same
    if (nibble.cell == Cell::CC)
        return nibble;

    // for side center piece, get its 2nd conjugate
    if (nibble.cell % 2 == 1)
    {
        auto match = _mscp2[nibble.face + nibble.cell];
        return Nibble{match.first, match.second, color(match.first, match.second)};
    }

    // for corner piece, get its 2nd conjugate
    if (nibble.cell % 2 == 0 && n == 2)
    {
        auto match = _mcp2[nibble.face + nibble.cell];
        return Nibble{match.first, match.second, color(match.first, match.second)};
    }

    // for corner piece, get its 3rd conjugate
    if (nibble.cell % 2 == 0 && n == 3)
    {
        auto match = _mcp3[nibble.face + nibble.cell];
        return Nibble{match.first, match.second, color(match.first, match.second)};
    }

    throw invalid_argument("request for invalid nibble conjugate");
}

void Rubiks::turn(Face face, int n)
{
    n = n % 4; // 4 turns is identity

    if (n == 0) // no effect
        return;

    n = n < 0 ? n + 4 : n; // express as CW rotation

    vector<int> const *p = nullptr;
    switch (face)
    {
    case LEFT:
        p = &_tlcw;
        break;
    case RIGHT:
        p = &_trcw;
        break;
    case BACK:
        p = &_tbcw;
        break;
    case FRONT:
        p = &_tfcw;
        break;
    case DOWN:
        p = &_tdcw;
        break;
    case UP:
        p = &_tucw;
        break;
    }

    while (n-- > 0)
    {
        run_permutation(*p);
    }
}

void Rubiks::rotate(Face axis, int n)
{
    assert(n % 4 != 0 && "error: n == 0 (mod 4) has no effect");

    n = n % 4;             // 4 rotations is identity
    n = n < 0 ? n + 4 : n; // express as CW rotation

    vector<int> const *p = nullptr;
    switch (axis)
    {
    case LEFT:
        p = &_rlcw;
        break;
    case RIGHT:
        p = &_rrcw;
        break;
    case BACK:
        p = &_rbcw;
        break;
    case FRONT:
        p = &_rfcw;
        break;
    case DOWN:
        p = &_rdcw;
        break;
    case UP:
        p = &_rucw;
        break;
    }

    while (n-- > 0)
    {
        run_permutation(*p);
    }
}

void Rubiks::run_permutation(vector<int> const &permutation)
{
    assert(permutation.size() == _state.length() && "error: permutation and state must be of equal size");
    string copy = _state;
    for (size_t i = 0; i < _state.length(); ++i)
    {
        _state[permutation[i]] = copy[i];
    }
}

int Rubiks::count_distinct_colors(string const &part) const
{
    int rcount = 0;
    int ocount = 0;
    int gcount = 0;
    int bcount = 0;
    int ycount = 0;
    int wcount = 0;

    for (int i = 0; i < part.length(); i++)
    {
        rcount += part[i] == RED;
        ocount += part[i] == ORANGE;
        gcount += part[i] == GREEN;
        bcount += part[i] == BLUE;
        ycount += part[i] == YELLOW;
        wcount += part[i] == WHITE;
    }

    return (rcount > 0) + (ocount > 0) + (gcount > 0) + (bcount > 0) + (ycount > 0) + (wcount > 0);
}

bool Rubiks::check_multi_color(string const &part, int n) const
{
    int rcount = 0;
    int ocount = 0;
    int gcount = 0;
    int bcount = 0;
    int ycount = 0;
    int wcount = 0;

    for (int i = 0; i < part.length(); i++)
    {
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

bool Rubiks::check_single_color(string const &part) const
{
    string goal(part.length(), part[0]);
    return part == goal;
}

Rubiks::Face opposite_of(Rubiks::Face face)
{
    return face = face % 18 == 0 ? (Rubiks::Face)(face + 9) : (Rubiks::Face)(face - 9);
}

Rubiks::Face left_of(Rubiks::Face face)
{
    switch (face)
    {
    case Rubiks::LEFT:
        return Rubiks::BACK;
    case Rubiks::RIGHT:
        return Rubiks::FRONT;
    case Rubiks::BACK:
        return Rubiks::RIGHT;
    case Rubiks::FRONT:
        return Rubiks::LEFT;
    case Rubiks::DOWN:
    case Rubiks::UP:
        throw invalid_argument("face: cannot take left-of operator from down/up face");
    }
}

Rubiks::Face right_of(Rubiks::Face face)
{
    switch (face)
    {
    case Rubiks::LEFT:
        return Rubiks::FRONT;
    case Rubiks::RIGHT:
        return Rubiks::BACK;
    case Rubiks::BACK:
        return Rubiks::LEFT;
    case Rubiks::FRONT:
        return Rubiks::RIGHT;
    case Rubiks::DOWN:
    case Rubiks::UP:
        throw invalid_argument("face: cannot take right-of operator from down/up face");
    }
}

ostream &operator<<(ostream &os, Rubiks const &cube)
{
    os << "--- --- --- --- --- ---\n";
    os << " l   r   b   f   d   u \n";
    os << "--- --- --- --- --- ---\n";
    for (size_t j = 0; j < 9; j += 3)
    {
        for (size_t i = j; i < cube._state.length(); i += 9)
        {
            os << string(cube._state, i, 3) << " ";
        }
        os << "\n";
    }
    os << "--- --- --- --- --- ---\n";
    return os;
}

ostream &operator<<(ostream &os, Rubiks::Face const &face)
{
    switch (face)
    {
    case Rubiks::LEFT:
        os << "L";
        break;
    case Rubiks::RIGHT:
        os << "R";
        break;
    case Rubiks::BACK:
        os << "B";
        break;
    case Rubiks::FRONT:
        os << "F";
        break;
    case Rubiks::DOWN:
        os << "D";
        break;
    case Rubiks::UP:
        os << "U";
        break;
    }
    return os;
}

ostream &operator<<(ostream &os, Rubiks::Cell const &cell)
{
    switch (cell)
    {
    case Rubiks::NW:
        os << "NW";
        break;
    case Rubiks::N:
        os << "N";
        break;
    case Rubiks::NE:
        os << "NE";
        break;
    case Rubiks::W:
        os << "W";
        break;
    case Rubiks::CC:
        os << "CC";
        break;
    case Rubiks::E:
        os << "E";
        break;
    case Rubiks::SW:
        os << "SW";
        break;
    case Rubiks::S:
        os << "S";
        break;
    case Rubiks::SE:
        os << "SE";
        break;
    }
    return os;
}

ostream &operator<<(ostream &os, Rubiks::Color const &color) { return os << (char)color; }

ostream &operator<<(ostream &os, Rubiks::CenterPiece const &piece)
{
    os << "{ " << get<0>(piece) << " }";
    return os;
}

ostream &operator<<(ostream &os, Rubiks::SideCenterPiece const &piece)
{
    os << "{ " << get<0>(piece) << ",";
    os << " " << get<1>(piece) << " }";
    return os;
}

ostream &operator<<(ostream &os, Rubiks::CornerPiece const &piece)
{
    os << "{ " << get<0>(piece) << ",";
    os << " " << get<1>(piece) << ",";
    os << " " << get<2>(piece) << " }";
    return os;
}
