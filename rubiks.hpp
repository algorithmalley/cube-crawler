#pragma once

#include <array>
#include <iosfwd>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

// A model for the Rubik's Cube
//
// The cube's state is stored as a single string of Color characters, first ordered by Face, then by Cell.
// The Face-enum codings correspond to the position of that face within the state string. Similarly, the Cell-enum
// codings correspond to the position of that cell within its corresponding face part of the string.
//
// Use one of the queries to obtain info about the cube. The 'color'-query is very low-level and returns no info
// about pieces. The '*-pieces'-query allows you to find more structural info about the cube.
//
// The 'rotate'-command reorients the cube as a whole. The 'turn'-command turns a single face. As far as looking
// at that face from the front, the effects are:
//    NW N NE       . . .              * . .             . . *
//     W C E    :   . c .   <--ccw--   . c .   --cw-->   . c .
//    SW S SE       * . .              . . .             . . .
//
class Rubiks
{
  public:
    enum Face : int { LEFT = 0, RIGHT = 9, BACK = 18, FRONT = 27, DOWN = 36, UP = 45 };
    enum Cell : int { NW = 0, N = 1, NE = 2, W = 3, CC = 4, E = 5, SW = 6, S = 7, SE = 8 };
    enum Color : char { RED = 'r', ORANGE = 'o', GREEN = 'g', BLUE = 'b', YELLOW = 'y', WHITE = 'w' };
    struct Nibble {
        Face face;
        Cell cell;
        Color color;
    };
    using CenterPiece = std::tuple<Nibble>;
    using SideCenterPiece = std::tuple<Nibble, Nibble>;
    using CornerPiece = std::tuple<Nibble, Nibble, Nibble>;

    /*
        Construction & Destruction:
    */

    // Constructs a valid solved cube instance
    explicit Rubiks();

    // Constructs a cube instance according to the string instance
    explicit Rubiks(std::string const &lrbfdu);

    /*
        Queries:
    */

    // Returns true iff instance represents a valid cube
    bool valid() const;

    // Returns true iff instance represents a cube that has all colors in the right places
    bool solved() const;

    // Calculates a measure for how scrambled the cube is. 0 is solved. 1 = max. scrambled.
    auto entropy() const -> double;

    // Gets the color at a specific face + cell
    auto color(Face face, Cell cell) const -> Color;

    // Finds the center piece at the specified face
    auto center_piece(Face face) const -> CenterPiece;

    // Finds the side center pieces that contain the specified color (1st nibble the one with matching color)
    auto side_center_pieces(Color color) const -> std::array<SideCenterPiece, 4>;

    // Finds the corner pieces that contain the specified color (1st nibble the one with matching color)
    auto corner_pieces(Color color) const -> std::array<CornerPiece, 4>;

    /*
        Commands:
    */

    // Turn a single face 1, 2 or 3 times CW (n>1) or CCW (n<1)
    void turn(Face face, int n);

    // Rotate the whole cube 1, 2 or 3 times CW (n>1) or CCW (n<1)
    void rotate(Face axis, int n);

  private:
    friend std::ostream &operator<<(std::ostream &os, Rubiks const &cube);
    friend std::ostream &operator<<(std::ostream &os, Face const &face);
    friend std::ostream &operator<<(std::ostream &os, Cell const &cell);
    friend std::ostream &operator<<(std::ostream &os, Color const &color);
    friend std::ostream &operator<<(std::ostream &os, CenterPiece const &piece);
    friend std::ostream &operator<<(std::ostream &os, SideCenterPiece const &piece);
    friend std::ostream &operator<<(std::ostream &os, CornerPiece const &piece);

    auto find_nibbles(Color color) const -> std::vector<Nibble>;
    auto match_nibble(Nibble const &nibble, std::size_t n) const -> Nibble;

    int count_distinct_colors(std::string const &part) const;
    bool check_multi_color(std::string const &part, int n) const;
    bool check_single_color(std::string const &part) const;

    void run_permutation(std::vector<int> const &permutation);
    static std::vector<int> _tlcw, _trcw, _tbcw, _tfcw, _tdcw, _tucw;
    static std::vector<int> _rlcw, _rrcw, _rbcw, _rfcw, _rdcw, _rucw;
    static std::array<std::pair<Face, Cell>, 54> _mscp2;
    static std::array<std::pair<Face, Cell>, 54> _mcp2;
    static std::array<std::pair<Face, Cell>, 54> _mcp3;

    std::string _state;
};

Rubiks::Face opposite_of(Rubiks::Face face);
Rubiks::Face left_of(Rubiks::Face face);
Rubiks::Face right_of(Rubiks::Face face);
Rubiks::Cell cell_of(Rubiks::Face face, Rubiks::CornerPiece const &piece);

std::string color_key(Rubiks::CenterPiece const &piece);
std::string color_key(Rubiks::SideCenterPiece const &piece);
std::string color_key(Rubiks::CornerPiece const &piece);
std::string color_key(Rubiks::Color c1, Rubiks::Color c2);
std::string color_key(Rubiks::Color c1, Rubiks::Color c2, Rubiks::Color c3);

std::ostream &operator<<(std::ostream &os, Rubiks const &cube);
std::ostream &operator<<(std::ostream &os, Rubiks::Face const &face);
std::ostream &operator<<(std::ostream &os, Rubiks::Cell const &cell);
std::ostream &operator<<(std::ostream &os, Rubiks::Color const &color);
std::ostream &operator<<(std::ostream &os, Rubiks::CenterPiece const &piece);
std::ostream &operator<<(std::ostream &os, Rubiks::SideCenterPiece const &piece);
std::ostream &operator<<(std::ostream &os, Rubiks::CornerPiece const &piece);

inline Rubiks::Color Rubiks::color(Face face, Cell cell) const { return (Color)_state[face + cell]; }