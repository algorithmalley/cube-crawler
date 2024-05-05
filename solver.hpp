#pragma once

#include "rubiks.hpp"
#include <memory>
#include <utility>
#include <vector>

struct Solver {

    enum Strategy {
        EASY, // See https://ruwix.com/the-rubiks-cube/how-to-solve-the-rubiks-cube-beginners-method/
        CFOP  // See https://ruwix.com/the-rubiks-cube/advanced-cfop-fridrich/
    };

    using step = std::pair<Rubiks::Face, int>;

    static auto Create(Strategy strategy) -> std::shared_ptr<Solver>;

    virtual auto strategy() const -> Strategy = 0;

    virtual auto solve(Rubiks &cube) const -> std::vector<step> = 0;

    virtual auto scramble(Rubiks &cube) const -> std::vector<step>;

    virtual ~Solver() = default;
};

namespace detail {

class BaseSolver final : public Solver
{
  public:
    virtual ~BaseSolver() = default;

    auto strategy() const -> Strategy override { return EASY; };

    auto solve(Rubiks &cube) const -> std::vector<step> override;
};

class CfopSolver final : public Solver
{
  public:
    virtual ~CfopSolver() = default;

    auto strategy() const -> Strategy override { return CFOP; };

    auto solve(Rubiks &cube) const -> std::vector<step> override;
};

} // namespace detail
