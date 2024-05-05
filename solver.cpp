#include "solver.hpp"
#include <cassert>
#include <cstdlib>
#include <ctime>
#include <stdexcept>

using namespace std;

namespace {

constexpr double MIN_ENTROPY = 0.6;
constexpr size_t MIN_STEPS = 20;
constexpr size_t MAX_STEPS = 25;
constexpr size_t FACES = 6;
constexpr size_t CELLS = 9;
constexpr size_t TURNS = 3;

} // namespace

shared_ptr<Solver> Solver::Create(Strategy strategy)
{
    switch (strategy)
    {
    case Solver::EASY:
        return make_shared<detail::BaseSolver>();

    case Solver::CFOP:
        return make_shared<detail::CfopSolver>();

    default:
        assert(false && "missing implementation solver strategy");
        return nullptr;
    }
}

vector<Solver::step> Solver::scramble(Rubiks &cube) const
{
    vector<Solver::step> steps;
    do
    {
        srand(time(nullptr)); // use current time as seed for random generator
        steps.clear();
        Rubiks tmp_cube = cube;
        while (tmp_cube.entropy() < MIN_ENTROPY)
        {
            int random_face = std::rand() % FACES;
            int random_turns = 1 + std::rand() / ((RAND_MAX + 1u) / TURNS); // Simply 1 + rand() % something is biased!
            int random_dir = std::rand() % 2;

            auto face = (Rubiks::Face)(random_face * CELLS);
            auto n = (random_dir ? -1 : 1) * random_turns;

            if (!steps.empty() && face == steps.back().first) // don't do same face twice consecutively
                continue;

            tmp_cube.turn(face, n);
            steps.push_back(make_pair(face, n));
        }
    } while (steps.size() < MIN_STEPS || steps.size() > MAX_STEPS);

    for (auto step : steps)
        cube.turn(step.first, step.second);

    return steps;
}

namespace detail {

vector<Solver::step> BaseSolver::solve(Rubiks &cube) const { return {}; }

vector<Solver::step> CfopSolver::solve(Rubiks &cube) const { throw runtime_error("CFOP Solver not implemented"); }

} // namespace detail
