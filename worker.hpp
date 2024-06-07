#pragma once

#include "solver.hpp"
#include <functional>
#include <vector>

class Rubiks;
class Device;

// Scan and init cube as it is on the device
void scan(Rubiks &cube, Device &crawler);

// Apply the given steps to the cube on the device
void run(std::vector<Solver::Step> const &steps, Device &crawler, std::function<bool()> const &interrupted);
