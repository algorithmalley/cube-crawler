#include "device.hpp"
#include "rubiks.hpp"
#include "worker.hpp"
#include <exception>
#include <iostream>

using namespace std;

int main()
{
    try {
        Worker worker;
        Device crawler;
        Rubiks cube;

        // Use this to init cube as it is on device
        // worker.scan(cube, &crawler);

        cout << cube;
        cout << "cube status: " << (cube.valid() ? "valid" : "invalid") << "\n";
        cout << "cube status: " << (cube.solved() ? "solved" : "scrambled") << "\n";

        cout << "turn left CW 1x...\n";
        cout << "turn right CW 1x...\n";
        cout << "turn back CW 1x...\n";
        cout << "turn front CW 1x...\n";
        cout << "turn down CW 1x...\n";
        cout << "turn up CW 1x...\n";
        cube.turn(Rubiks::LEFT, 2);
        cube.turn(Rubiks::RIGHT, 2);
        cube.turn(Rubiks::BACK, 2);
        cube.turn(Rubiks::FRONT, 2);
        cube.turn(Rubiks::DOWN, 2);
        cube.turn(Rubiks::UP, 2);
        cout << cube;
        cout << "cube status: " << (cube.valid() ? "valid" : "invalid") << "\n";
        cout << "cube status: " << (cube.solved() ? "solved" : "scrambled") << "\n";

        /*
                cout << "scrambling cube...\n";
                worker.scramble(cube);

                cout << "cube status: " << (cube.solved() ? "solved" : "scrambled") << "\n";

                cout << "solving cube...\n";
                worker.solve(cube);

                cout << "cube status: " << (cube.solved() ? "solved" : "scrambled") << "\n";
        */
        return 0;
    } catch (exception const &e) {
        cerr << e.what() << '\n';
        return -1;
    }
}
