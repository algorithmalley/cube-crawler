#include "worker.hpp"
#include "device.hpp"
#include "rubiks.hpp"

using namespace std;

namespace {

struct DeviceState {
};

void operate_device(Rubiks::Face face, int n, DeviceState &state)
{
    // TODO
    // - track which side of cube is at turn table
    // - this induces a delta w.r.t. the face turned now (step.first)
    //   (where the delta means a # of table and/or beam rotations)
    // - move the physical cube with this delta, so that the correct face is at the turn table
    // - apply the turn physically
}

} // namespace

void scan(Rubiks &cube, Device &crawler)
{
    // TODO: implement
}

void run(vector<Solver::step> const &steps, Device &crawler)
{
    // TODO: implement
}
