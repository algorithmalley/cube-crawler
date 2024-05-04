#pragma once

#include <iosfwd>
#include <string>
#include <utility>
#include <vector>

class Rubiks
{
  public:
    enum Face : int { LEFT = 0, RIGHT = 9, BACK = 18, FRONT = 27, DOWN = 36, UP = 45 };
    enum Cell : int { NW = 0, N = 1, NE = 2, W = 3, CC = 4, E = 5, SW = 6, S = 7, SE = 8 };
    enum Color : char { RED = 'r', ORANGE = 'o', GREEN = 'g', BLUE = 'b', YELLOW = 'y', WHITE = 'w' };

    explicit Rubiks();
    explicit Rubiks(std::string const &lrbfdu);

    bool valid() const;
    bool solved() const;

    auto color(Face face, Cell cell) const -> Color;
    auto find_color(Color color) const -> std::vector<std::pair<Face, Cell>>;

    void turn(Face face, int n);

  private:
    friend std::ostream &operator<<(std::ostream &os, Rubiks &cube);

    bool check_multi_color(std::string const &part, int n) const;
    bool check_single_color(std::string const &part) const;

    void run_permutation(std::vector<int> const &permutation);
    static std::vector<int> _lcw, _rcw, _bcw, _fcw, _dcw, _ucw;

    // Stored as one string for all 6 faces, such that each substring
    // is oriented as if one looks at a face while keeping the cube
    // fixed in standard position.
    // - face order: L, R, B, F, D, U
    // - face value: "*...c...."
    // - orientated: . . .              * . .             . . *    / NW N NE \
    //               . c .   <--ccw--   . c .   --cw-->   . c .       W C E
    //               * . .              . . .             . . .    \ SW S SE /
    std::string _state;
};

std::ostream &operator<<(std::ostream &os, Rubiks &cube);

inline Rubiks::Color Rubiks::color(Face face, Cell cell) const { return (Color)_state[face + cell]; }