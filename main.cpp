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
        solver->set_log(log_file); // enable to get extensive debug info in log file

        Rubiks cube;
        log_file << cube << "\n";

        auto problem = solver->scramble(cube);

        log_file << "scrambling steps:\n";
        for (auto step : problem)
            log_file << step;
        log_file << "\n" << cube << "\n";

        auto solution = solver->solve(cube);

        log_file << "\nsolving steps:\n";
        for (auto step : solution)
            log_file << step;
        log_file << "\n" << cube << "\n";

        log_file.flush();

        return 0;
    }
    catch (exception const &e)
    {
        cerr << e.what() << '\n';
        return -1;
    }
}
