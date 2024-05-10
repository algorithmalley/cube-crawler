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

vector<Solver::step> Solver::scramble(Rubiks &cube, double min_entropy) const
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

namespace detail {

vector<Solver::step> BaseSolver::solve(Rubiks &cube) const { return {}; }

vector<Solver::step> CfopSolver::solve(Rubiks &cube) const { throw runtime_error("CFOP Solver not implemented"); }

} // namespace detail
