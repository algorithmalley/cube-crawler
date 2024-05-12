#include "solver.hpp"
#include <cassert>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <stdexcept>

using namespace std;

namespace {

constexpr size_t MIN_SCRAMBLE = 30;
constexpr size_t FACES = 6;
constexpr size_t CELLS = 9;
constexpr size_t TURNS = 3;

bool apply_steps(Rubiks &cube, std::vector<Solver::step> &registry, size_t &curr_step_idx)
{
    if (curr_step_idx < registry.size())
    {
        while (curr_step_idx < registry.size())
        {
            cube.turn(registry.at(curr_step_idx).first, registry.at(curr_step_idx).second);
            curr_step_idx++;
        }
        return true;
    }
    return false;
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

    log() << "solving cube:\n" << cube;

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
    Rubiks::Color down_cc, left_cc, right_cc, back_cc, front_cc;
    down_cc = cube.color(Rubiks::DOWN, Rubiks::CC);
    left_cc = cube.color(Rubiks::LEFT, Rubiks::CC);
    right_cc = cube.color(Rubiks::RIGHT, Rubiks::CC);
    back_cc = cube.color(Rubiks::BACK, Rubiks::CC);
    front_cc = cube.color(Rubiks::FRONT, Rubiks::CC);

    log() << "* solve cross (color DOWN = " << down_cc << ")\n";

    size_t curr_step_idx = registry.size();
    bool changed;

    log() << "+ move '" << down_cc << "' side center pieces UP to 3rd layer\n";
    do
    {
        changed = false;

        auto scpcs = cube.side_center_pieces(down_cc);
        for (auto &piece : scpcs) // conj1 := guaranteed the face with same color as the target
        {
            Rubiks::Nibble conj1, conj2;
            std::tie(conj1, conj2) = piece;

            if (conj1.face == Rubiks::UP)
                continue;

            log() << "- " << piece << ": ";
            if (conj2.face == Rubiks::UP)
            {
                log() << "found in 3rd layer --> reflect\n";
                registry.push_back(make_pair(conj1.face, -1));
                registry.push_back(make_pair(Rubiks::UP, 1));
                registry.push_back(make_pair(left_of(conj1.face), -1));
            }
            else if (conj1.face == Rubiks::DOWN)
            {
                log() << "found in 1st layer --> rotate\n";

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
                log() << "found in 1st layer --> reflect & rotate\n";

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
                log() << "found in 2nd layer --> move to 1st layer\n";
                registry.push_back(make_pair(conj1.face, conj1.cell == Rubiks::W ? -1 : 1));
                registry.push_back(make_pair(Rubiks::DOWN, -1));
                registry.push_back(make_pair(conj1.face, conj1.cell == Rubiks::W ? 1 : -1));
            }

            if (apply_steps(cube, registry, curr_step_idx))
            {
                changed = true;
                break;
            }
        }

    } while (changed);

    log() << "+ align with centers and shift into cross\n";
    log() << "- center colors: " //
          << "(L, R, B, F) = (" << left_cc << ", " << right_cc << ", " << back_cc << ", " << front_cc << ")\n";
    do
    {
        changed = false;

        auto scpcs = cube.side_center_pieces(down_cc);
        for (auto &piece : scpcs) // conj1 := guaranteed the face with same color as the target
        {
            Rubiks::Nibble conj1, conj2;
            std::tie(conj1, conj2) = piece;

            if (conj1.face == Rubiks::DOWN)
                continue;

            log() << "- " << piece << ": ";
            if (conj2.face == Rubiks::LEFT)
            {
                log() << "found on the left --> ";
                Rubiks::Face shift_to = Rubiks::LEFT;
                if (left_cc != conj2.color)
                {
                    log() << "align & ";
                    if (right_cc == conj2.color)
                        shift_to = Rubiks::RIGHT, registry.push_back(make_pair(Rubiks::UP, 2));
                    else if (back_cc == conj2.color)
                        shift_to = Rubiks::BACK, registry.push_back(make_pair(Rubiks::UP, 1));
                    else if (front_cc == conj2.color)
                        shift_to = Rubiks::FRONT, registry.push_back(make_pair(Rubiks::UP, -1));
                }
                log() << "shift\n";
                registry.push_back(make_pair(shift_to, 2));
            }
            else if (conj2.face == Rubiks::RIGHT)
            {
                log() << "found on the right --> ";
                Rubiks::Face shift_to = Rubiks::RIGHT;
                if (right_cc != conj2.color)
                {
                    log() << "align & ";
                    if (left_cc == conj2.color)
                        shift_to = Rubiks::LEFT, registry.push_back(make_pair(Rubiks::UP, 2));
                    else if (back_cc == conj2.color)
                        shift_to = Rubiks::BACK, registry.push_back(make_pair(Rubiks::UP, -1));
                    else if (front_cc == conj2.color)
                        shift_to = Rubiks::FRONT, registry.push_back(make_pair(Rubiks::UP, +1));
                }
                log() << "shift\n";
                registry.push_back(make_pair(shift_to, 2));
            }
            else if (conj2.face == Rubiks::BACK)
            {
                log() << "found at the back --> ";
                Rubiks::Face shift_to = Rubiks::BACK;
                if (back_cc != conj2.color)
                {
                    log() << "align & ";
                    if (left_cc == conj2.color)
                        shift_to = Rubiks::LEFT, registry.push_back(make_pair(Rubiks::UP, -1));
                    else if (right_cc == conj2.color)
                        shift_to = Rubiks::RIGHT, registry.push_back(make_pair(Rubiks::UP, 1));
                    else if (front_cc == conj2.color)
                        shift_to = Rubiks::FRONT, registry.push_back(make_pair(Rubiks::UP, 2));
                }
                log() << "shift\n";
                registry.push_back(make_pair(shift_to, 2));
            }
            else if (conj2.face == Rubiks::FRONT)
            {
                log() << "found at the front --> ";
                Rubiks::Face shift_to = Rubiks::FRONT;
                if (front_cc != conj2.color)
                {
                    log() << "align & ";
                    if (left_cc == conj2.color)
                        shift_to = Rubiks::LEFT, registry.push_back(make_pair(Rubiks::UP, 1));
                    else if (right_cc == conj2.color)
                        shift_to = Rubiks::RIGHT, registry.push_back(make_pair(Rubiks::UP, -1));
                    else if (back_cc == conj2.color)
                        shift_to = Rubiks::BACK, registry.push_back(make_pair(Rubiks::UP, 2));
                }
                log() << "shift\n";
                registry.push_back(make_pair(shift_to, 2));
            }

            if (apply_steps(cube, registry, curr_step_idx))
            {
                changed = true;
                break;
            }
        }

    } while (changed);

    log() << "- result:\n";
    log() << cube;
}

void EasySolver::solve_l1_corners(Rubiks &cube, std::vector<step> &registry) const {}

void EasySolver::solve_l2_edges(Rubiks &cube, std::vector<step> &registry) const {}

void EasySolver::solve_l3_cross(Rubiks &cube, std::vector<step> &registry) const {}

void EasySolver::solve_l3_edges(Rubiks &cube, std::vector<step> &registry) const {}

void EasySolver::solve_l3_corners_permutation(Rubiks &cube, std::vector<step> &registry) const {}

void EasySolver::solve_l3_corners_orientation(Rubiks &cube, std::vector<step> &registry) const {}

vector<Solver::step> CfopSolver::solve(Rubiks &cube) const { throw runtime_error("CFOP Solver not implemented"); }

} // namespace detail
