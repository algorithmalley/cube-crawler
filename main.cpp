#include "device.hpp"
#include "rubiks.hpp"
#include "solver.hpp"
#include "worker.hpp"
#include <exception>
#include <fstream>
#include <iostream>

using namespace std;

int main()
{
    try
    {
        ofstream log_file;
        log_file.open("cube-crawler.log");

        auto solver = Solver::Create(Solver::L123);
        solver->set_log(log_file);

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
        cout << "cube entropy: " << cube.entropy() << "\n";

        cout << "solving cube...\n";
        auto steps_solve = solver->solve(cube);
        cout << cube;
        cout << "cube status: " << (cube.valid() ? "valid" : "invalid") << "\n";
        cout << "cube status: " << (cube.solved() ? "solved" : "scrambled") << "\n";
        cout << "# steps: " << steps_solve.size() << "\n";

        return 0;
    }
    catch (exception const &e)
    {
        cerr << e.what() << '\n';
        return -1;
    }
}
