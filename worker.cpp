#include "worker.hpp"
#include "device.hpp"
#include "rubiks.hpp"
#include <cmath>
#include <tuple>

#include <iostream>

using namespace std;

void scan(Rubiks &cube, Device &crawler)
{
    // TODO: implement
}

void run(vector<Solver::Step> const &steps, Device &crawler, std::function<bool()> const &interrupted)
{
    for (auto step : steps)
    {
        cout << step;

        Solver::Operation op;
        Rubiks::Face face;
        int n;
        tie(op, face, n) = step;

        if (interrupted())
            break;

        auto const &cube_to_device_face_map = crawler.permutation();
        crawler.down(cube_to_device_face_map.at(face));

        if (interrupted())
            break;

        //        cout << "check correct face is down and press enter\n";
        //        cin.ignore();

        crawler.turn(n * -1, op != Solver::Rotate); // cube ccw <=> table cw
    }
}
