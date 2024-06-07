#include "solver.hpp"
#include <cassert>
#include <ctime>
#include <ostream>
#include <string>

using namespace std;

shared_ptr<Solver> Solver::Create(Strategy strategy)
{
    switch (strategy)
    {
    case Solver::L123:
        return make_shared<detail::L123Solver>();

    case Solver::CFOP:
        return make_shared<detail::CfopSolver>();

    default:
        assert(false && "missing implementation solver strategy");
        return nullptr;
    }
}

std::ostream &operator<<(std::ostream &os, Solver::Step const &step)
{
    assert((get<0>(step) == Solver::Turn ||
            get<0>(step) == Solver::Rotate && (get<1>(step) == Rubiks::UP || get<1>(step) == Rubiks::DOWN)) &&
           "rotation only about UP/DOWN axis");

    Solver::Operation op;
    Rubiks::Face face;
    int n;
    tie(op, face, n) = step;

    switch (op)
    {
    case Solver::Turn:
        os << "- turn face '" << face << "' " << abs(n) << "x " << (n < 0 ? "CCW" : "CW") << "\n";
        break;
    case Solver::Rotate:
        os << "- rotate cube " << abs(n) << "x " << (n < 0 ? "CCW" : "CW") << "\n";
        break;
    }

    return os;
}

namespace detail {

constexpr size_t MIN_SCRAMBLE = 20;
constexpr size_t FACES = 6;
constexpr size_t CELLS = 9;
constexpr size_t TURNS = 3;

vector<Solver::Step> BaseSolver::scramble(Rubiks &cube, double min_entropy) const
{
    vector<Solver::Step> steps;
    static auto const op = Solver::Turn;

    srand(time(nullptr)); // use current time as seed for random generator
    auto rnd_step = [&]() {
        Solver::Step step;
        Rubiks::Face face;
        int count;
        do
        {
            int rnd_face = std::rand() % FACES;
            int rnd_turns = 1 + std::rand() / ((RAND_MAX + 1u) / TURNS); // Simply 1 + rand() % something is biased!
            int rnd_dir = std::rand() % 2;
            step = make_tuple(op, (Rubiks::Face)(rnd_face * CELLS), (rnd_dir ? -1 : 1) * rnd_turns);
            tie(ignore, face, count) = step;
        } while (/* Don't scramble about the same axis twice in a row */ !steps.empty() &&
                 ((face % 18 == 0 && get<1>(steps.back()) == face + 9) ||
                  (face % 18 == 9 && get<1>(steps.back()) == face - 9) || face == get<1>(steps.back())));
        cube.turn(face, count);
        steps.push_back(step);
    };

    // Ought to be enough to be an interesting solve
    for (size_t i = 0; i < MIN_SCRAMBLE; i++)
        rnd_step();

    // Continue scrambling until visually scrambled enough too
    while (cube.entropy() < min_entropy)
        rnd_step();

    return steps;
}

} // namespace detail
