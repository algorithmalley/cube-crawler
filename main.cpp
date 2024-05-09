#include "device.hpp"
#include "rubiks.hpp"
#include "solver.hpp"
#include "worker.hpp"
#include <exception>
#include <iostream>

using namespace std;

int main()
{
    try
    {
        auto solver = Solver::Create(Solver::EASY);

        cout << "creating default cube...\n";
        Rubiks cube;
        cout << cube;
        cout << "cube status: " << (cube.valid() ? "valid" : "invalid") << "\n";
        cout << "cube status: " << (cube.solved() ? "solved" : "scrambled") << "\n";
        cout << "cube entropy: " << cube.entropy() << "\n";

        cout << "scrambling cube...\n";
        auto steps = solver->scramble(cube);
        cout << cube;
        cout << "cube status: " << (cube.valid() ? "valid" : "invalid") << "\n";
        cout << "cube status: " << (cube.solved() ? "solved" : "scrambled") << "\n";
        cout << "cube entropy: " << cube.entropy() << " (" << steps.size() << " steps)\n";

        cout << "rotating cube CW at up...\n";
        cube.rotate(Rubiks::UP, 1);
        cout << cube;

        /*
                cout << "solving cube...\n";
                solver->solve(cube);
                cout << cube;
                cout << "cube status: " << (cube.valid() ? "valid" : "invalid") << "\n";
                cout << "cube status: " << (cube.solved() ? "solved" : "scrambled") << "\n";
                cout << "cube entropy: " << cube.entropy() << "\n";
        */
        return 0;
    } catch (exception const &e)
    {
        cerr << e.what() << '\n';
        return -1;
    }
}
