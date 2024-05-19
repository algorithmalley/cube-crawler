#include "solver.hpp"
#include <stdexcept>

using namespace std;

namespace detail {

vector<Solver::Step> CfopSolver::solve(Rubiks &cube) const { throw runtime_error("CFOP Solver not implemented"); }

} // namespace detail
