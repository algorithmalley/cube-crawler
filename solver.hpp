#pragma once

#include "rubiks.hpp"
#include <memory>
#include <ostream>
#include <utility>
#include <vector>

struct Solver {

    enum Strategy {
        L123, // See https://ruwix.com/the-rubiks-cube/how-to-solve-the-rubiks-cube-beginners-method/
        CFOP  // See https://ruwix.com/the-rubiks-cube/advanced-cfop-fridrich/
    };

    using step = std::pair<Rubiks::Face, int>;

    static auto Create(Strategy strategy) -> std::shared_ptr<Solver>;
    virtual ~Solver() = default;

    virtual auto strategy() const -> Strategy = 0;
    virtual void set_log(std::ostream &os) = 0;
    virtual auto solve(Rubiks &cube) const -> std::vector<step> = 0;
    virtual auto scramble(Rubiks &cube, double min_entropy = 0.0) const -> std::vector<step> = 0;
};

namespace detail {

class BaseSolver : public Solver
{
  public:
    struct Logger {
        std::ostream *_os = nullptr;
    };
    virtual ~BaseSolver() = default;

    auto log() const -> Logger const & { return _logger; }
    void set_log(std::ostream &os) override { _logger._os = &os; };

    auto scramble(Rubiks &cube, double min_entropy = 0.0) const -> std::vector<step> override;

  private:
    Logger _logger;
};

template <typename T> BaseSolver::Logger const &operator<<(BaseSolver::Logger const &logger, T value)
{
    if (logger._os != nullptr) // else simply ignore the log action
    {
        (*logger._os) << value;
    }
    return logger;
};

class EasySolver final : public BaseSolver
{
  public:
    virtual ~EasySolver() = default;

    auto strategy() const -> Strategy override { return L123; };

    auto solve(Rubiks &cube) const -> std::vector<step> override;

  private:
    void solve_1st_layer(Rubiks &cube, std::vector<step> &registry) const;
    void solve_2nd_layer(Rubiks &cube, std::vector<step> &registry) const;
    void solve_3rd_layer(Rubiks &cube, std::vector<step> &registry) const;
    void solve_l1_cross(Rubiks &cube, std::vector<step> &registry) const;
    void solve_l1_cross_a(Rubiks &cube, std::vector<step> &registry) const;
    void solve_l1_cross_b(Rubiks &cube, std::vector<step> &registry) const;
    void solve_l1_corners(Rubiks &cube, std::vector<step> &registry) const;
    void solve_l2_edges(Rubiks &cube, std::vector<step> &registry) const;
    void solve_l3_cross(Rubiks &cube, std::vector<step> &registry) const;
    void solve_l3_edges(Rubiks &cube, std::vector<step> &registry) const;
    void solve_l3_corners_permutation(Rubiks &cube, std::vector<step> &registry) const;
    void solve_l3_corners_orientation(Rubiks &cube, std::vector<step> &registry) const;
};

class CfopSolver final : public BaseSolver
{
  public:
    virtual ~CfopSolver() = default;

    auto strategy() const -> Strategy override { return CFOP; };

    auto solve(Rubiks &cube) const -> std::vector<step> override;
};

} // namespace detail
