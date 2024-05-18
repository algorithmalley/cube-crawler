#include "solver.hpp"
#include <cassert>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <stdexcept>
#include <string>
#include <unordered_set>

using namespace std;

namespace {

constexpr size_t MIN_SCRAMBLE = 30;
constexpr size_t FACES = 6;
constexpr size_t CELLS = 9;
constexpr size_t TURNS = 3;

bool apply_steps(Rubiks &cube, std::vector<Solver::step> &registry, size_t &curr_step_idx,
                 ::detail::BaseSolver::Logger const &log)
{
    if (curr_step_idx < registry.size())
    {
        while (curr_step_idx < registry.size())
        {
            auto face = registry.at(curr_step_idx).first;
            auto n = registry.at(curr_step_idx).second;
            log << "- " << face << ": " << abs(n) << "x " << (n < 0 ? "CCW" : "CW") << "\n";
            cube.turn(face, n);
            curr_step_idx++;
        }
        return true;
    }
    return false;
}

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

shared_ptr<Solver> Solver::Create(Strategy strategy)
{
    switch (strategy)
    {
    case Solver::L123:
        return make_shared<detail::EasySolver>();

    case Solver::CFOP:
        return make_shared<detail::CfopSolver>();

    default:
        assert(false && "missing implementation solver strategy");
        return nullptr;
    }
}

namespace detail {

vector<Solver::step> BaseSolver::scramble(Rubiks &cube, double min_entropy) const
{
    vector<Solver::step> steps;

    srand(time(nullptr)); // use current time as seed for random generator
    auto rnd_step = [&]() {
        Solver::step rnd_step;
        do
        {
            int random_face = std::rand() % FACES;
            int random_turns = 1 + std::rand() / ((RAND_MAX + 1u) / TURNS); // Simply 1 + rand() % something is biased!
            int random_dir = std::rand() % 2;
            auto face = (Rubiks::Face)(random_face * CELLS);
            auto n = (random_dir ? -1 : 1) * random_turns;
            rnd_step = make_pair(face, n);
        } while (!steps.empty() && ((rnd_step.first % 18 == 0 && steps.back().first == rnd_step.first + 9) ||
                                    (rnd_step.first % 18 == 9 && steps.back().first == rnd_step.first - 9) ||
                                    rnd_step.first == steps.back().first));
        cube.turn(rnd_step.first, rnd_step.second);
        steps.push_back(rnd_step);
    };

    for (size_t i = 0; i < MIN_SCRAMBLE; i++)
    {
        rnd_step();
    }

    while (cube.entropy() < min_entropy)
    {
        rnd_step();
    }

    return steps;
}

vector<Solver::step> EasySolver::solve(Rubiks &cube) const
{
    std::vector<step> registry;

    solve_1st_layer(cube, registry);
    solve_2nd_layer(cube, registry);
    solve_3rd_layer(cube, registry);

    return registry;
}

void EasySolver::solve_1st_layer(Rubiks &cube, std::vector<step> &registry) const
{
    log() << "1st layer\n";
    solve_l1_cross(cube, registry);
    solve_l1_corners(cube, registry);
}

void EasySolver::solve_2nd_layer(Rubiks &cube, std::vector<step> &registry) const
{
    log() << "2nd layer\n";
    solve_l2_edges(cube, registry);
}

void EasySolver::solve_3rd_layer(Rubiks &cube, std::vector<step> &registry) const
{
    log() << "3rd layer\n";
    solve_l3_cross(cube, registry);
    solve_l3_edges(cube, registry);
    solve_l3_corners_permutation(cube, registry);
    solve_l3_corners_orientation(cube, registry);
}

void EasySolver::solve_l1_cross(Rubiks &cube, std::vector<step> &registry) const
{
    log() << "1st layer :: cross\n";
    solve_l1_cross_a(cube, registry);
    solve_l1_cross_b(cube, registry);
}

void EasySolver::solve_l1_cross_a(Rubiks &cube, std::vector<step> &registry) const
{
    log() << "1st layer :: cross :: prepare\n";

    auto down_cc = cube.color(Rubiks::DOWN, Rubiks::CC);
    auto done = unordered_set<string>();

    // With this nested for/while/for loop we can run until one piece is done (not knowing the nbr of steps
    // required), still refreshing the actual position of piece1 by means of piece2.

    auto cpcs1 = cube.side_center_pieces(down_cc);
    for (auto &piece_i : cpcs1) // conj1 := guaranteed the face with same color as the target
    {
        auto curr_step_idx = registry.size();
        auto color_key_i = color_key(piece_i);
        log() << "1st layer :: cross :: prepare :: " << piece_i << " (" << color_key_i << ")\n";

        do
        {
            auto cpcs2 = cube.side_center_pieces(down_cc);
            for (auto &piece_j : cpcs2)
            {
                auto color_key_j = color_key(piece_j);
                if (color_key_j != color_key_i) // only process piece_j "==" piece_i (up to position)
                    continue;

                Rubiks::Nibble conj1, conj2;
                std::tie(conj1, conj2) = piece_j;
                int d;

                log() << "1st layer :: cross :: prepare :: " << piece_i << " (" << color_key_i << ") :: ";
                if (conj1.face == Rubiks::UP)
                {
                    log() << "correctly positioned\n";
                    done.insert(color_key_j);
                    break;
                }
                else if (conj2.face == Rubiks::UP)
                {
                    log() << "reflect to get " << down_cc << " cell UP \n";
                    registry.push_back(make_pair(conj1.face, -1));
                    registry.push_back(make_pair(Rubiks::UP, 1));
                    registry.push_back(make_pair(left_of(conj1.face), -1));
                }
                else if (conj1.face == Rubiks::DOWN)
                {
                    log() << "rotate to get " << down_cc << " cell UP \n";

                    if (cube.color(Rubiks::UP, Rubiks::N) != conj1.color)
                        registry.push_back(make_pair(Rubiks::UP, 2));
                    else if (cube.color(Rubiks::UP, Rubiks::W) != conj1.color)
                        registry.push_back(make_pair(Rubiks::UP, -1));
                    else if (cube.color(Rubiks::UP, Rubiks::E) != conj1.color)
                        registry.push_back(make_pair(Rubiks::UP, 1));

                    if (conj2.face == Rubiks::BACK)
                        registry.push_back(make_pair(Rubiks::DOWN, 2));
                    else if (conj2.face == Rubiks::LEFT)
                        registry.push_back(make_pair(Rubiks::DOWN, 1));
                    else if (conj2.face == Rubiks::RIGHT)
                        registry.push_back(make_pair(Rubiks::DOWN, -1));

                    registry.push_back(make_pair(Rubiks::FRONT, 2));
                }
                else if (conj2.face == Rubiks::DOWN)
                {
                    log() << "reflect then rotate to get " << down_cc << " cell UP \n";

                    if (cube.color(Rubiks::UP, Rubiks::N) != down_cc)
                        registry.push_back(make_pair(Rubiks::UP, 1));
                    else if (cube.color(Rubiks::UP, Rubiks::W) != down_cc)
                        registry.push_back(make_pair(Rubiks::UP, 2));
                    else if (cube.color(Rubiks::UP, Rubiks::S) != down_cc)
                        registry.push_back(make_pair(Rubiks::UP, -1));

                    if (conj1.face == Rubiks::BACK)
                        registry.push_back(make_pair(Rubiks::DOWN, 2));
                    else if (conj1.face == Rubiks::LEFT)
                        registry.push_back(make_pair(Rubiks::DOWN, 1));
                    else if (conj1.face == Rubiks::RIGHT)
                        registry.push_back(make_pair(Rubiks::DOWN, -1));

                    registry.push_back(make_pair(Rubiks::FRONT, -1));
                    registry.push_back(make_pair(Rubiks::RIGHT, 1));
                    registry.push_back(make_pair(Rubiks::FRONT, 1));
                }
                else
                {
                    log() << "first move from 2nd to 1st layer\n";
                    registry.push_back(make_pair(conj1.face, conj1.cell == Rubiks::W ? -1 : 1));
                    registry.push_back(make_pair(Rubiks::DOWN, -1));
                    registry.push_back(make_pair(conj1.face, conj1.cell == Rubiks::W ? 1 : -1));
                }

                apply_steps(cube, registry, curr_step_idx, log());
                break;
            }

        } while (done.count(color_key_i) == 0);
    }
}

void EasySolver::solve_l1_cross_b(Rubiks &cube, std::vector<step> &registry) const
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
    // required), still refreshing the actual position of piece1 by means of piece2.

    auto cpcs1 = cube.side_center_pieces(down_cc);
    for (auto &piece_i : cpcs1) // conj1 := guaranteed the face with same color as the target
    {
        auto curr_step_idx = registry.size();
        auto color_key_i = color_key(piece_i);
        log() << "1st layer :: cross :: finalize :: " << piece_i << " (" << color_key_i << ")\n";

        do
        {
            auto cpcs2 = cube.side_center_pieces(down_cc);
            for (auto &piece_j : cpcs2)
            {
                auto color_key_j = color_key(piece_j);
                if (color_key_j != color_key_i) // only process piece_j "==" piece_i (up to position)
                    continue;

                Rubiks::Nibble conj1, conj2;
                std::tie(conj1, conj2) = piece_j;
                Rubiks::Face shift_to = conj2.face;

                log() << "1st layer :: cross :: finalize :: " << piece_i << " (" << color_key_i << ") :: ";
                if (cube.color(conj2.face, Rubiks::CC) == conj2.color && conj1.face == Rubiks::DOWN)
                {
                    log() << "correctly positioned\n";
                    done.insert(color_key_j);
                    break;
                }
                else if (cube.color(conj2.face, Rubiks::CC) == conj2.color)
                {
                    log() << "rotate into 1st layer\n";
                    registry.push_back(make_pair(shift_to, 2));
                }
                else if (conj2.face == Rubiks::LEFT && left_cc != conj2.color)
                {
                    if (right_cc == conj2.color)
                        shift_to = Rubiks::RIGHT, registry.push_back(make_pair(Rubiks::UP, 2));
                    else if (back_cc == conj2.color)
                        shift_to = Rubiks::BACK, registry.push_back(make_pair(Rubiks::UP, 1));
                    else if (front_cc == conj2.color)
                        shift_to = Rubiks::FRONT, registry.push_back(make_pair(Rubiks::UP, -1));
                    log() << "align with " << conj2.color << " center at " << shift_to << "\n";
                }
                else if (conj2.face == Rubiks::RIGHT && right_cc != conj2.color)
                {
                    if (left_cc == conj2.color)
                        shift_to = Rubiks::LEFT, registry.push_back(make_pair(Rubiks::UP, 2));
                    else if (back_cc == conj2.color)
                        shift_to = Rubiks::BACK, registry.push_back(make_pair(Rubiks::UP, -1));
                    else if (front_cc == conj2.color)
                        shift_to = Rubiks::FRONT, registry.push_back(make_pair(Rubiks::UP, +1));
                    log() << "align with " << conj2.color << " center at " << shift_to << "\n";
                }
                else if (conj2.face == Rubiks::BACK && back_cc != conj2.color)
                {
                    if (left_cc == conj2.color)
                        shift_to = Rubiks::LEFT, registry.push_back(make_pair(Rubiks::UP, -1));
                    else if (right_cc == conj2.color)
                        shift_to = Rubiks::RIGHT, registry.push_back(make_pair(Rubiks::UP, 1));
                    else if (front_cc == conj2.color)
                        shift_to = Rubiks::FRONT, registry.push_back(make_pair(Rubiks::UP, 2));
                    log() << "align with " << conj2.color << " center at " << shift_to << "\n";
                }
                else if (conj2.face == Rubiks::FRONT && front_cc != conj2.color)
                {
                    if (left_cc == conj2.color)
                        shift_to = Rubiks::LEFT, registry.push_back(make_pair(Rubiks::UP, 1));
                    else if (right_cc == conj2.color)
                        shift_to = Rubiks::RIGHT, registry.push_back(make_pair(Rubiks::UP, -1));
                    else if (back_cc == conj2.color)
                        shift_to = Rubiks::BACK, registry.push_back(make_pair(Rubiks::UP, 2));
                    log() << "align with " << conj2.color << " center at " << shift_to << "\n";
                }

                apply_steps(cube, registry, curr_step_idx, log());
                break;
            }

        } while (done.count(color_key_i) == 0);
    }
}

void EasySolver::solve_l1_corners(Rubiks &cube, std::vector<step> &registry) const
{
    log() << "1st layer :: corners\n";

    auto down_cc = cube.color(Rubiks::DOWN, Rubiks::CC);
    auto done = unordered_set<string>();

    // With this nested for/while/for loop we can run until one piece is done (not knowing the nbr of steps
    // required), still refreshing the actual position of piece1 by means of piece2.

    auto cpcs1 = cube.corner_pieces(down_cc);
    for (auto &piece_i : cpcs1) // conj1 := guaranteed the face with same color as the target
    {
        auto curr_step_idx = registry.size();
        auto color_key_i = color_key(piece_i);
        log() << "1st layer :: corners :: " << piece_i << " (" << color_key_i << ")\n";

        do
        {
            auto cpcs2 = cube.corner_pieces(down_cc);
            for (auto &piece_j : cpcs2)
            {
                auto color_key_j = color_key(piece_j);
                if (color_key_j != color_key_i) // only process piece_j "==" piece_i (up to position)
                    continue;

                Rubiks::Nibble conj1, conj2, conj3;
                std::tie(conj1, conj2, conj3) = piece_j;
                int d;

                log() << "1st layer :: corners :: " << piece_i << " (" << color_key_i << ") :: ";
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
                    registry.push_back(make_pair(conj2.face, conj2.cell == Rubiks::SW ? 1 : -1));
                    registry.push_back(make_pair(Rubiks::UP, conj2.cell == Rubiks::SW ? 1 : -1));
                    registry.push_back(make_pair(conj2.face, conj2.cell == Rubiks::SW ? -1 : 1));
                    registry.push_back(make_pair(Rubiks::UP, conj2.cell == Rubiks::SW ? -1 : 1));
                }
                else if (conj2.face == Rubiks::DOWN || conj3.face == Rubiks::DOWN)
                {
                    log() << "move from 1st to 3rd layer\n";
                    registry.push_back(make_pair(conj1.face, conj1.cell == Rubiks::SW ? 1 : -1));
                    registry.push_back(make_pair(Rubiks::UP, conj1.cell == Rubiks::SW ? 1 : -1));
                    registry.push_back(make_pair(conj1.face, conj1.cell == Rubiks::SW ? -1 : 1));
                    registry.push_back(make_pair(Rubiks::UP, conj1.cell == Rubiks::SW ? -1 : 1));
                }
                else if ((d = projected_distance_up(cube, piece_j)) != 0)
                {
                    log() << "align with " << conj2.color << " and " << conj3.color << " corner\n";
                    registry.push_back(make_pair(Rubiks::UP, d));
                }
                else if (conj1.face == Rubiks::UP)
                {
                    log() << "reorient to get " << down_cc << " up\n";
                    registry.push_back(make_pair(conj2.face, conj2.cell == Rubiks::NW ? 1 : -1));
                    registry.push_back(make_pair(Rubiks::UP, conj2.cell == Rubiks::NW ? 2 : -2));
                    registry.push_back(make_pair(conj2.face, conj2.cell == Rubiks::NW ? -1 : 1));
                    registry.push_back(make_pair(Rubiks::UP, conj2.cell == Rubiks::NW ? -1 : 1));
                }
                else
                {
                    log() << "rotate into 1st layer\n";
                    registry.push_back(make_pair(conj2.face, conj2.cell == Rubiks::NW ? -1 : 1));
                    registry.push_back(make_pair(conj1.face, conj2.cell == Rubiks::NW ? 1 : -1));
                    registry.push_back(make_pair(conj2.face, conj2.cell == Rubiks::NW ? 1 : -1));
                    registry.push_back(make_pair(conj1.face, conj2.cell == Rubiks::NW ? -1 : 1));
                }

                apply_steps(cube, registry, curr_step_idx, log());
                break;
            }

        } while (done.count(color_key_i) == 0);
    }
}

void EasySolver::solve_l2_edges(Rubiks &cube, std::vector<step> &registry) const {}

void EasySolver::solve_l3_cross(Rubiks &cube, std::vector<step> &registry) const {}

void EasySolver::solve_l3_edges(Rubiks &cube, std::vector<step> &registry) const {}

void EasySolver::solve_l3_corners_permutation(Rubiks &cube, std::vector<step> &registry) const {}

void EasySolver::solve_l3_corners_orientation(Rubiks &cube, std::vector<step> &registry) const {}

vector<Solver::step> CfopSolver::solve(Rubiks &cube) const { throw runtime_error("CFOP Solver not implemented"); }

} // namespace detail
