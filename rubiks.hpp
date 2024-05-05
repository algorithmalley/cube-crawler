#pragma once

#include <iosfwd>
#include <string>
#include <utility>
#include <vector>

// A model for the Rubik's Cube
//
// The cube's state is stored as a single string of Color characters, first ordered by face, then by cell.
// The order of the faces is L, R, B, F, D, U. Note that the Face-enum codings correspond to the position of
// that face within the state string.
//
// The order of the cells within each face-part of the state string is:
//    NW N NE
//     W C E
//    SW S SE
// Note that the Cell-enum codings correspond to the position of that cell within its corresponding face
// part of the state string.
//
// The 'turn'-command ensures the state string stays valid if it was valid. Its effects are:
//   . . .              * . .             . . *
//   . c .   <--ccw--   . c .   --cw-->   . c .
//   * . .              . . .             . . .
//
class Rubiks
{
  public:
    enum Face : int { LEFT = 0, RIGHT = 9, BACK = 18, FRONT = 27, DOWN = 36, UP = 45 };
    enum Cell : int { NW = 0, N = 1, NE = 2, W = 3, CC = 4, E = 5, SW = 6, S = 7, SE = 8 };
    enum Color : char { RED = 'r', ORANGE = 'o', GREEN = 'g', BLUE = 'b', YELLOW = 'y', WHITE = 'w' };

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

    // Calculates a measure for how scrambled the cube is. 0 is solved.
    auto entropy() const -> double;

    // Gets the color at a specific face + cell
    auto color(Face face, Cell cell) const -> Color;

    // Finds all positions of the specified color (always 9 of them)
    auto find_color(Color color) const -> std::vector<std::pair<Face, Cell>>;

    /*
        Commands:
    */

    // Turn the cube 1, 2 or 3 times CW (n>1) or CCW (n<1)
    void turn(Face face, int n);

  private:
    friend std::ostream &operator<<(std::ostream &os, Rubiks &cube);

    int count_distinct_colors(std::string const &part) const;
    bool check_multi_color(std::string const &part, int n) const;
    bool check_single_color(std::string const &part) const;

    void run_permutation(std::vector<int> const &permutation);
    static std::vector<int> _lcw, _rcw, _bcw, _fcw, _dcw, _ucw;

    std::string _state;
};

std::ostream &operator<<(std::ostream &os, Rubiks &cube);

inline Rubiks::Color Rubiks::color(Face face, Cell cell) const { return (Color)_state[face + cell]; }