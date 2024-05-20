#include "solver.hpp"
#include <cassert>
#include <cmath>
#include <cstdlib>
#include <ostream>
#include <string>
#include <unordered_set>

using namespace std;

namespace {

bool apply_steps(Rubiks &cube, std::vector<Solver::Step> &registry, size_t &curr_step_idx,
                 ::detail::BaseSolver::Logger const &log)
{
    if (curr_step_idx < registry.size())
    {
        while (curr_step_idx < registry.size())
        {
            auto &step = registry.at(curr_step_idx);

            Solver::Operation op;
            Rubiks::Face face;
            int n;
            tie(op, face, n) = step;

            switch (op)
            {
            case Solver::Turn:
                cube.turn(face, n);
                break;

            case Solver::Rotate:
                cube.rotate(face, n);
                break;
            }
            log << step;
            curr_step_idx++;
        }
        return true;
    }
    return false;
}

// Nbr of times to turn UP to align two centers matching the given corner key with the cell in UP
int projected_distance_up(Rubiks const &cube, string const &cc_cols_key, Rubiks::Cell cell)
{
    assert(cc_cols_key.length() == 2 && "two center's colors expected in projected_distance_up");
    assert((cell == Rubiks::NW || cell == Rubiks::NE || cell == Rubiks::SW || cell == Rubiks::SE) &&
           "corner cell of UP face expected in projected_distance_up");

    Rubiks::Color left_cc, right_cc, back_cc, front_cc;
    left_cc = cube.color(Rubiks::LEFT, Rubiks::CC);
    right_cc = cube.color(Rubiks::RIGHT, Rubiks::CC);
    back_cc = cube.color(Rubiks::BACK, Rubiks::CC);
    front_cc = cube.color(Rubiks::FRONT, Rubiks::CC);

    string nw_color = color_key(left_cc, back_cc);
    string ne_color = color_key(right_cc, back_cc);
    string sw_color = color_key(left_cc, front_cc);
    string se_color = color_key(right_cc, front_cc);

    if (cell == Rubiks::NW)
    {
        if (cc_cols_key == nw_color)
            return 0;
        if (cc_cols_key == ne_color)
            return -1;
        if (cc_cols_key == sw_color)
            return 1;
        if (cc_cols_key == se_color)
            return 2;
    }
    else if (cell == Rubiks::NE)
    {
        if (cc_cols_key == nw_color)
            return 1;
        if (cc_cols_key == ne_color)
            return 0;
        if (cc_cols_key == sw_color)
            return 2;
        if (cc_cols_key == se_color)
            return -1;
    }
    else if (cell == Rubiks::SW)
    {
        if (cc_cols_key == nw_color)
            return -1;
        if (cc_cols_key == ne_color)
            return 2;
        if (cc_cols_key == sw_color)
            return 0;
        if (cc_cols_key == se_color)
            return 1;
    }
    else if (cell == Rubiks::SE)
    {
        if (cc_cols_key == nw_color)
            return 2;
        if (cc_cols_key == ne_color)
            return 1;
        if (cc_cols_key == sw_color)
            return -1;
        if (cc_cols_key == se_color)
            return 0;
    }

    assert(false && "logic error in projected_distance_up");
    return 0;
}

// Nbr of times to turn UP to align given side piece above side of corner with matching colors
int projected_distance_up(Rubiks const &cube, Rubiks::SideCenterPiece piece)
{
    Rubiks::Color left_cc, right_cc, back_cc, front_cc;
    left_cc = cube.color(Rubiks::LEFT, Rubiks::CC);
    right_cc = cube.color(Rubiks::RIGHT, Rubiks::CC);
    back_cc = cube.color(Rubiks::BACK, Rubiks::CC);
    front_cc = cube.color(Rubiks::FRONT, Rubiks::CC);

    string nw_color = color_key(left_cc, back_cc);
    string ne_color = color_key(right_cc, back_cc);
    string sw_color = color_key(left_cc, front_cc);
    string se_color = color_key(right_cc, front_cc);

    auto piece_cell = cell_of(Rubiks::UP, piece); // where are we, projected in UP
    auto piece_color = color_key(piece);          // where do we need to go?

    if (piece_cell == Rubiks::N)
    {
        if (piece_color == nw_color)
            return cube.color(Rubiks::UP, piece_cell) == back_cc ? -1 : 0;
        if (piece_color == ne_color)
            return cube.color(Rubiks::UP, piece_cell) == back_cc ? 1 : 0;
        if (piece_color == sw_color)
            return cube.color(Rubiks::UP, piece_cell) == front_cc ? -1 : 2;
        if (piece_color == se_color)
            return cube.color(Rubiks::UP, piece_cell) == front_cc ? 1 : 2;
    }
    else if (piece_cell == Rubiks::W)
    {
        if (piece_color == nw_color)
            return cube.color(Rubiks::UP, piece_cell) == left_cc ? 1 : 0;
        if (piece_color == sw_color)
            return cube.color(Rubiks::UP, piece_cell) == left_cc ? -1 : 0;
        if (piece_color == ne_color)
            return cube.color(Rubiks::UP, piece_cell) == right_cc ? 1 : 2;
        if (piece_color == se_color)
            return cube.color(Rubiks::UP, piece_cell) == right_cc ? -1 : 2;
    }
    else if (piece_cell == Rubiks::E)
    {
        if (piece_color == ne_color)
            return cube.color(Rubiks::UP, piece_cell) == right_cc ? -1 : 0;
        if (piece_color == se_color)
            return cube.color(Rubiks::UP, piece_cell) == right_cc ? 1 : 0;
        if (piece_color == nw_color)
            return cube.color(Rubiks::UP, piece_cell) == left_cc ? -1 : 2;
        if (piece_color == sw_color)
            return cube.color(Rubiks::UP, piece_cell) == left_cc ? 1 : 2;
    }
    else if (piece_cell == Rubiks::S)
    {
        if (piece_color == sw_color)
            return cube.color(Rubiks::UP, piece_cell) == front_cc ? 1 : 0;
        if (piece_color == se_color)
            return cube.color(Rubiks::UP, piece_cell) == front_cc ? -1 : 0;
        if (piece_color == nw_color)
            return cube.color(Rubiks::UP, piece_cell) == back_cc ? 1 : 2;
        if (piece_color == ne_color)
            return cube.color(Rubiks::UP, piece_cell) == back_cc ? -1 : 2;
    }

    assert(false && "logic error in projected_distance_up");
    return 0;
}

// Nbr of times to turn UP to align given corner piece above corner with matching colors
int projected_distance_up(Rubiks const &cube, Rubiks::CornerPiece piece)
{
    Rubiks::Color down_cc, left_cc, right_cc, back_cc, front_cc;
    down_cc = cube.color(Rubiks::DOWN, Rubiks::CC);
    left_cc = cube.color(Rubiks::LEFT, Rubiks::CC);
    right_cc = cube.color(Rubiks::RIGHT, Rubiks::CC);
    back_cc = cube.color(Rubiks::BACK, Rubiks::CC);
    front_cc = cube.color(Rubiks::FRONT, Rubiks::CC);

    string nw_color = color_key(down_cc, left_cc, back_cc);
    string ne_color = color_key(down_cc, right_cc, back_cc);
    string sw_color = color_key(down_cc, left_cc, front_cc);
    string se_color = color_key(down_cc, right_cc, front_cc);

    auto piece_cell = cell_of(Rubiks::UP, piece); // where are we, projected in UP
    auto piece_color = color_key(piece);          // where do we need to go?

    if (piece_cell == Rubiks::NW)
    {
        if (piece_color == nw_color)
            return 0;
        if (piece_color == ne_color)
            return -1;
        if (piece_color == sw_color)
            return 1;
        if (piece_color == se_color)
            return 2;
    }
    else if (piece_cell == Rubiks::NE)
    {
        if (piece_color == nw_color)
            return 1;
        if (piece_color == ne_color)
            return 0;
        if (piece_color == sw_color)
            return 2;
        if (piece_color == se_color)
            return -1;
    }
    else if (piece_cell == Rubiks::SW)
    {
        if (piece_color == nw_color)
            return -1;
        if (piece_color == ne_color)
            return 2;
        if (piece_color == sw_color)
            return 0;
        if (piece_color == se_color)
            return 1;
    }
    else if (piece_cell == Rubiks::SE)
    {
        if (piece_color == nw_color)
            return -2;
        if (piece_color == ne_color)
            return 1;
        if (piece_color == sw_color)
            return -1;
        if (piece_color == se_color)
            return 0;
    }

    assert(false && "logic error in projected_distance_up");
    return 0;
}

} // namespace

namespace detail {

vector<Solver::Step> L123Solver::solve(Rubiks &cube) const
{
    std::vector<Step> registry;

    solve_1st_layer(cube, registry);
    solve_2nd_layer(cube, registry);
    solve_3rd_layer(cube, registry);

    return registry;
}

void L123Solver::solve_1st_layer(Rubiks &cube, std::vector<Step> &registry) const
{
    log() << "1st layer\n";
    solve_l1_cross(cube, registry);
    solve_l1_corners(cube, registry);
}

void L123Solver::solve_2nd_layer(Rubiks &cube, std::vector<Step> &registry) const
{
    log() << "2nd layer\n";
    solve_l2_edges(cube, registry);
}

void L123Solver::solve_3rd_layer(Rubiks &cube, std::vector<Step> &registry) const
{
    log() << "3rd layer\n";
    solve_l3_cross(cube, registry);
    solve_l3_edges(cube, registry);
    solve_l3_corners_permutation(cube, registry);
    solve_l3_corners_orientation(cube, registry);
}

void L123Solver::solve_l1_cross(Rubiks &cube, std::vector<Step> &registry) const
{
    log() << "1st layer :: cross\n";
    solve_l1_cross_a(cube, registry);
    solve_l1_cross_b(cube, registry);
}

void L123Solver::solve_l1_cross_a(Rubiks &cube, std::vector<Step> &registry) const
{
    log() << "1st layer :: cross :: prepare\n";

    auto down_cc = cube.color(Rubiks::DOWN, Rubiks::CC);
    auto done = unordered_set<string>();

    // With this nested for/while/for loop we can run until one piece is done (not knowing the nbr of steps
    // required), still refreshing the actual position of piece_i by means of piece_j.

    auto goal = cube.side_center_pieces(down_cc);
    for (auto &piece_i : goal)
    {
        auto curr_step_idx = registry.size();
        auto color_key_i = color_key(piece_i);
        log() << "1st layer :: cross :: prepare :: " << color_key_i << "\n";

        do
        {
            for (auto &piece_j : cube.side_center_pieces(down_cc))
            {
                auto color_key_j = color_key(piece_j);
                if (color_key_j != color_key_i) // only process piece_j "==" piece_i (up to position)
                    continue;

                Rubiks::Nibble conj1, conj2; // conj1 := guaranteed the face with same color as down_cc
                std::tie(conj1, conj2) = piece_j;
                int d;

                log() << piece_j << ": ";
                if (conj1.face == Rubiks::UP)
                {
                    log() << "correctly positioned\n";
                    done.insert(color_key_j);
                    break;
                }
                else if (conj2.face == Rubiks::UP)
                {
                    log() << "reflect to get " << down_cc << " cell UP \n";
                    registry.push_back(make_tuple(Solver::Turn, conj1.face, -1));
                    registry.push_back(make_tuple(Solver::Turn, Rubiks::UP, 1));
                    registry.push_back(make_tuple(Solver::Turn, left_of(conj1.face), -1));
                }
                else if (conj1.face == Rubiks::DOWN)
                {
                    log() << "rotate to get " << down_cc << " cell UP \n";

                    if (cube.color(Rubiks::UP, Rubiks::N) != conj1.color)
                        registry.push_back(make_tuple(Solver::Turn, Rubiks::UP, 2));
                    else if (cube.color(Rubiks::UP, Rubiks::W) != conj1.color)
                        registry.push_back(make_tuple(Solver::Turn, Rubiks::UP, -1));
                    else if (cube.color(Rubiks::UP, Rubiks::E) != conj1.color)
                        registry.push_back(make_tuple(Solver::Turn, Rubiks::UP, 1));

                    if (conj2.face == Rubiks::BACK)
                        registry.push_back(make_tuple(Solver::Turn, Rubiks::DOWN, 2));
                    else if (conj2.face == Rubiks::LEFT)
                        registry.push_back(make_tuple(Solver::Turn, Rubiks::DOWN, 1));
                    else if (conj2.face == Rubiks::RIGHT)
                        registry.push_back(make_tuple(Solver::Turn, Rubiks::DOWN, -1));

                    registry.push_back(make_tuple(Solver::Turn, Rubiks::FRONT, 2));
                }
                else if (conj2.face == Rubiks::DOWN)
                {
                    log() << "reflect then rotate to get " << down_cc << " cell UP \n";

                    if (cube.color(Rubiks::UP, Rubiks::N) != down_cc)
                        registry.push_back(make_tuple(Solver::Turn, Rubiks::UP, 1));
                    else if (cube.color(Rubiks::UP, Rubiks::W) != down_cc)
                        registry.push_back(make_tuple(Solver::Turn, Rubiks::UP, 2));
                    else if (cube.color(Rubiks::UP, Rubiks::S) != down_cc)
                        registry.push_back(make_tuple(Solver::Turn, Rubiks::UP, -1));

                    if (conj1.face == Rubiks::BACK)
                        registry.push_back(make_tuple(Solver::Turn, Rubiks::DOWN, 2));
                    else if (conj1.face == Rubiks::LEFT)
                        registry.push_back(make_tuple(Solver::Turn, Rubiks::DOWN, 1));
                    else if (conj1.face == Rubiks::RIGHT)
                        registry.push_back(make_tuple(Solver::Turn, Rubiks::DOWN, -1));

                    registry.push_back(make_tuple(Solver::Turn, Rubiks::FRONT, -1));
                    registry.push_back(make_tuple(Solver::Turn, Rubiks::RIGHT, 1));
                    registry.push_back(make_tuple(Solver::Turn, Rubiks::FRONT, 1));
                }
                else
                {
                    log() << "first move from 2nd to 1st layer\n";
                    registry.push_back(make_tuple(Solver::Turn, conj1.face, conj1.cell == Rubiks::W ? -1 : 1));
                    registry.push_back(make_tuple(Solver::Turn, Rubiks::DOWN, -1));
                    registry.push_back(make_tuple(Solver::Turn, conj1.face, conj1.cell == Rubiks::W ? 1 : -1));
                }

                apply_steps(cube, registry, curr_step_idx, log());
                break;
            }

        } while (done.count(color_key_i) == 0);
    }
}

void L123Solver::solve_l1_cross_b(Rubiks &cube, std::vector<Step> &registry) const
{
    log() << "1st layer :: cross :: finalize\n";

    Rubiks::Color down_cc, left_cc, right_cc, back_cc, front_cc;
    down_cc = cube.color(Rubiks::DOWN, Rubiks::CC);
    left_cc = cube.color(Rubiks::LEFT, Rubiks::CC);
    right_cc = cube.color(Rubiks::RIGHT, Rubiks::CC);
    back_cc = cube.color(Rubiks::BACK, Rubiks::CC);
    front_cc = cube.color(Rubiks::FRONT, Rubiks::CC);
    auto done = unordered_set<string>();

    // With this nested for/while/for loop we can run until one piece is done (not knowing the nbr of steps
    // required), still refreshing the actual position of piece_i by means of piece_j.

    auto goal = cube.side_center_pieces(down_cc);
    for (auto &piece_i : goal)
    {
        auto curr_step_idx = registry.size();
        auto color_key_i = color_key(piece_i);
        log() << "1st layer :: cross :: finalize :: " << color_key_i << "\n";

        do
        {
            for (auto &piece_j : cube.side_center_pieces(down_cc))
            {
                auto color_key_j = color_key(piece_j);
                if (color_key_j != color_key_i) // only process piece_j "==" piece_i (up to position)
                    continue;

                Rubiks::Nibble conj1, conj2; // conj1 := guaranteed the face with same color as down_cc
                std::tie(conj1, conj2) = piece_j;
                Rubiks::Face shift_to = conj2.face;

                log() << piece_j << ": ";
                if (cube.color(conj2.face, Rubiks::CC) == conj2.color && conj1.face == Rubiks::DOWN)
                {
                    log() << "correctly positioned\n";
                    done.insert(color_key_j);
                    break;
                }
                else if (cube.color(conj2.face, Rubiks::CC) == conj2.color)
                {
                    log() << "rotate into 1st layer\n";
                    registry.push_back(make_tuple(Solver::Turn, shift_to, 2));
                }
                else if (conj2.face == Rubiks::LEFT && left_cc != conj2.color)
                {
                    if (right_cc == conj2.color)
                        shift_to = Rubiks::RIGHT, registry.push_back(make_tuple(Solver::Turn, Rubiks::UP, 2));
                    else if (back_cc == conj2.color)
                        shift_to = Rubiks::BACK, registry.push_back(make_tuple(Solver::Turn, Rubiks::UP, 1));
                    else if (front_cc == conj2.color)
                        shift_to = Rubiks::FRONT, registry.push_back(make_tuple(Solver::Turn, Rubiks::UP, -1));
                    log() << "align with " << conj2.color << " center at " << shift_to << "\n";
                }
                else if (conj2.face == Rubiks::RIGHT && right_cc != conj2.color)
                {
                    if (left_cc == conj2.color)
                        shift_to = Rubiks::LEFT, registry.push_back(make_tuple(Solver::Turn, Rubiks::UP, 2));
                    else if (back_cc == conj2.color)
                        shift_to = Rubiks::BACK, registry.push_back(make_tuple(Solver::Turn, Rubiks::UP, -1));
                    else if (front_cc == conj2.color)
                        shift_to = Rubiks::FRONT, registry.push_back(make_tuple(Solver::Turn, Rubiks::UP, +1));
                    log() << "align with " << conj2.color << " center at " << shift_to << "\n";
                }
                else if (conj2.face == Rubiks::BACK && back_cc != conj2.color)
                {
                    if (left_cc == conj2.color)
                        shift_to = Rubiks::LEFT, registry.push_back(make_tuple(Solver::Turn, Rubiks::UP, -1));
                    else if (right_cc == conj2.color)
                        shift_to = Rubiks::RIGHT, registry.push_back(make_tuple(Solver::Turn, Rubiks::UP, 1));
                    else if (front_cc == conj2.color)
                        shift_to = Rubiks::FRONT, registry.push_back(make_tuple(Solver::Turn, Rubiks::UP, 2));
                    log() << "align with " << conj2.color << " center at " << shift_to << "\n";
                }
                else if (conj2.face == Rubiks::FRONT && front_cc != conj2.color)
                {
                    if (left_cc == conj2.color)
                        shift_to = Rubiks::LEFT, registry.push_back(make_tuple(Solver::Turn, Rubiks::UP, 1));
                    else if (right_cc == conj2.color)
                        shift_to = Rubiks::RIGHT, registry.push_back(make_tuple(Solver::Turn, Rubiks::UP, -1));
                    else if (back_cc == conj2.color)
                        shift_to = Rubiks::BACK, registry.push_back(make_tuple(Solver::Turn, Rubiks::UP, 2));
                    log() << "align with " << conj2.color << " center at " << shift_to << "\n";
                }

                apply_steps(cube, registry, curr_step_idx, log());
                break;
            }

        } while (done.count(color_key_i) == 0);
    }
}

void L123Solver::solve_l1_corners(Rubiks &cube, std::vector<Step> &registry) const
{
    log() << "1st layer :: corners\n";

    auto down_cc = cube.color(Rubiks::DOWN, Rubiks::CC);
    auto done = unordered_set<string>();

    // With this nested for/while/for loop we can run until one piece is done (not knowing the nbr of steps
    // required), still refreshing the actual position of piece_i by means of piece_j.

    auto goal = cube.corner_pieces(down_cc);
    for (auto &piece_i : goal)
    {
        auto curr_step_idx = registry.size();
        auto color_key_i = color_key(piece_i);
        log() << "1st layer :: corners :: " << color_key_i << "\n";

        do
        {
            for (auto &piece_j : cube.corner_pieces(down_cc))
            {
                auto color_key_j = color_key(piece_j);
                if (color_key_j != color_key_i) // only process piece_j "==" piece_i (up to position)
                    continue;

                Rubiks::Nibble conj1, conj2, conj3; // conj1 := guaranteed the face with same color as down_cc
                std::tie(conj1, conj2, conj3) = piece_j;
                int d;

                log() << piece_j << ": ";
                if (conj1.face == Rubiks::DOWN                           //
                    && cube.color(conj2.face, Rubiks::CC) == conj2.color //
                    && cube.color(conj3.face, Rubiks::CC) == conj3.color)
                {
                    log() << "correctly positioned\n";
                    done.insert(color_key_j);
                    break;
                }
                else if (conj1.face == Rubiks::DOWN)
                {
                    log() << "move from 1st to 3rd layer\n";
                    registry.push_back(make_tuple(Solver::Turn, conj2.face, conj2.cell == Rubiks::SW ? 1 : -1));
                    registry.push_back(make_tuple(Solver::Turn, Rubiks::UP, conj2.cell == Rubiks::SW ? 1 : -1));
                    registry.push_back(make_tuple(Solver::Turn, conj2.face, conj2.cell == Rubiks::SW ? -1 : 1));
                    registry.push_back(make_tuple(Solver::Turn, Rubiks::UP, conj2.cell == Rubiks::SW ? -1 : 1));
                }
                else if (conj2.face == Rubiks::DOWN || conj3.face == Rubiks::DOWN)
                {
                    log() << "move from 1st to 3rd layer\n";
                    registry.push_back(make_tuple(Solver::Turn, conj1.face, conj1.cell == Rubiks::SW ? 1 : -1));
                    registry.push_back(make_tuple(Solver::Turn, Rubiks::UP, conj1.cell == Rubiks::SW ? 1 : -1));
                    registry.push_back(make_tuple(Solver::Turn, conj1.face, conj1.cell == Rubiks::SW ? -1 : 1));
                    registry.push_back(make_tuple(Solver::Turn, Rubiks::UP, conj1.cell == Rubiks::SW ? -1 : 1));
                }
                else if ((d = projected_distance_up(cube, piece_j)) != 0)
                {
                    log() << "align with " << conj2.color << " and " << conj3.color << " corner\n";
                    registry.push_back(make_tuple(Solver::Turn, Rubiks::UP, d));
                }
                else if (conj1.face == Rubiks::UP)
                {
                    log() << "reorient to get " << down_cc << " up\n";
                    registry.push_back(make_tuple(Solver::Turn, conj2.face, conj2.cell == Rubiks::NW ? 1 : -1));
                    registry.push_back(make_tuple(Solver::Turn, Rubiks::UP, conj2.cell == Rubiks::NW ? 2 : -2));
                    registry.push_back(make_tuple(Solver::Turn, conj2.face, conj2.cell == Rubiks::NW ? -1 : 1));
                    registry.push_back(make_tuple(Solver::Turn, Rubiks::UP, conj2.cell == Rubiks::NW ? -1 : 1));
                }
                else
                {
                    log() << "rotate into 1st layer\n";
                    registry.push_back(make_tuple(Solver::Turn, conj2.face, conj2.cell == Rubiks::NW ? -1 : 1));
                    registry.push_back(make_tuple(Solver::Turn, conj1.face, conj2.cell == Rubiks::NW ? 1 : -1));
                    registry.push_back(make_tuple(Solver::Turn, conj2.face, conj2.cell == Rubiks::NW ? 1 : -1));
                    registry.push_back(make_tuple(Solver::Turn, conj1.face, conj2.cell == Rubiks::NW ? -1 : 1));
                }

                apply_steps(cube, registry, curr_step_idx, log());
                break;
            }

        } while (done.count(color_key_i) == 0);
    }
}

void L123Solver::solve_l2_edges(Rubiks &cube, std::vector<Step> &registry) const
{
    log() << "2nd layer :: edges\n";

    Rubiks::Color left_cc, right_cc, back_cc, front_cc;
    left_cc = cube.color(Rubiks::LEFT, Rubiks::CC);
    right_cc = cube.color(Rubiks::RIGHT, Rubiks::CC);
    back_cc = cube.color(Rubiks::BACK, Rubiks::CC);
    front_cc = cube.color(Rubiks::FRONT, Rubiks::CC);
    auto done = unordered_set<string>();

    // With this nested for/while/for loop we can run until one piece is done (not knowing the nbr of steps
    // required), still refreshing the actual position of piece_i by means of piece_j.

    auto goal = array<pair<Rubiks::Color, Rubiks::Color>, 4>{
        {{left_cc, front_cc}, {front_cc, right_cc}, {right_cc, back_cc}, {back_cc, left_cc}}};
    for (auto &piece_i : goal)
    {
        auto curr_step_idx = registry.size();
        auto color_key_i = color_key(piece_i.first, piece_i.second);
        log() << "2nd layer :: edges :: " << color_key_i << "\n";

        do
        {
            for (auto &piece_j : cube.side_center_pieces(piece_i.first))
            {
                auto color_key_j = color_key(piece_j);
                if (color_key_j != color_key_i) // only process piece_j "==" piece_i (up to position)
                    continue;

                Rubiks::Nibble conj1, conj2; // conj1 := guaranteed the face with same color as piece_i.first
                std::tie(conj1, conj2) = piece_j;
                int d;

                log() << piece_j << ": ";
                if (cube.color(conj1.face, Rubiks::CC) == conj1.color &&
                    cube.color(conj2.face, Rubiks::CC) == conj2.color)
                {
                    log() << "correctly positioned\n";
                    done.insert(color_key_j);
                    break;
                }
                else if ((d = projected_distance_up(cube, color_key_j, Rubiks::SE)) != 0)
                {
                    log() << "rotate cube to have " << color_key_j << " in right-front\n";
                    registry.push_back(make_tuple(Solver::Rotate, Rubiks::UP, d));
                }
                else if (conj1.face != Rubiks::UP && conj2.face != Rubiks::UP)
                {
                    log() << "first move from 2nd to 1st layer\n";
                    if (conj1.cell == Rubiks::E) // doesn't matter locally, and so we only need one case
                        swap(conj1.face, conj2.face);
                    registry.push_back(make_tuple(Solver::Turn, conj1.face, -1));
                    registry.push_back(make_tuple(Solver::Turn, conj2.face, -1));
                    registry.push_back(make_tuple(Solver::Turn, conj1.face, 1));
                    registry.push_back(make_tuple(Solver::Turn, Rubiks::UP, 1));
                    registry.push_back(make_tuple(Solver::Turn, conj1.face, 1));
                    registry.push_back(make_tuple(Solver::Turn, Rubiks::UP, -1));
                    registry.push_back(make_tuple(Solver::Turn, conj1.face, -1));
                    registry.push_back(make_tuple(Solver::Turn, conj2.face, 1));
                }
                else if ((d = projected_distance_up(cube, piece_j)) != 0)
                {
                    log() << "align with " << conj1.color << " and " << conj2.color << " side\n";
                    registry.push_back(make_tuple(Solver::Turn, Rubiks::UP, d));
                }
                else
                {
                    log() << "rotate into 2nd layer\n";
                    if (cell_of(Rubiks::UP, piece_j) == Rubiks::S)
                    {
                        registry.push_back(make_tuple(Solver::Turn, Rubiks::UP, 1));
                        registry.push_back(make_tuple(Solver::Turn, Rubiks::RIGHT, 1));
                        registry.push_back(make_tuple(Solver::Turn, Rubiks::UP, -1));
                        registry.push_back(make_tuple(Solver::Turn, Rubiks::RIGHT, -1));
                        registry.push_back(make_tuple(Solver::Turn, Rubiks::UP, -1));
                        registry.push_back(make_tuple(Solver::Turn, Rubiks::FRONT, -1));
                        registry.push_back(make_tuple(Solver::Turn, Rubiks::UP, 1));
                        registry.push_back(make_tuple(Solver::Turn, Rubiks::FRONT, 1));
                    }
                    else // cell_of(Rubiks::UP, piece_j) == Rubiks::E
                    {
                        registry.push_back(make_tuple(Solver::Turn, Rubiks::RIGHT, -1));
                        registry.push_back(make_tuple(Solver::Turn, Rubiks::FRONT, -1));
                        registry.push_back(make_tuple(Solver::Turn, Rubiks::RIGHT, 1));
                        registry.push_back(make_tuple(Solver::Turn, Rubiks::UP, 1));
                        registry.push_back(make_tuple(Solver::Turn, Rubiks::RIGHT, 1));
                        registry.push_back(make_tuple(Solver::Turn, Rubiks::UP, -1));
                        registry.push_back(make_tuple(Solver::Turn, Rubiks::RIGHT, -1));
                        registry.push_back(make_tuple(Solver::Turn, Rubiks::FRONT, 1));
                    }
                }

                apply_steps(cube, registry, curr_step_idx, log());
                break;
            }

        } while (done.count(color_key_i) == 0);
    }
}

void L123Solver::solve_l3_cross(Rubiks &cube, std::vector<Step> &registry) const {}

void L123Solver::solve_l3_edges(Rubiks &cube, std::vector<Step> &registry) const {}

void L123Solver::solve_l3_corners_permutation(Rubiks &cube, std::vector<Step> &registry) const {}

void L123Solver::solve_l3_corners_orientation(Rubiks &cube, std::vector<Step> &registry) const {}

} // namespace detail
