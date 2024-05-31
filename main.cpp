#include "device.hpp"
#include "rubiks.hpp"
#include "solver.hpp"
#include "worker.hpp"
#include <chrono>
#include <cstring>
#include <exception>
#include <fstream>
#include <iostream>
#include <thread>

using namespace std;

int run_pc()
{
    ofstream log_file;
    log_file.open("cube-crawler.log");

    auto solver = Solver::Create(Solver::L123);
    solver->set_log(log_file); // enable to get extensive debug info in log file

    Rubiks cube;

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

int run_brick()
{
    Device crawler;

    crawler.tell("cube detected!");

    for (int i = 0; i < 2; i++)
    {
        auto waitfor = chrono::seconds(1);

        crawler.turn(false);
        this_thread::sleep_for(waitfor);
        crawler.rotz(true);
        this_thread::sleep_for(waitfor);
        crawler.flip();
        this_thread::sleep_for(waitfor);
        crawler.turn(true);
        this_thread::sleep_for(waitfor);
        crawler.rotz(false);
        this_thread::sleep_for(waitfor);
        crawler.flip();
        this_thread::sleep_for(waitfor);
    }

    crawler.tell("cube solved!");

    /*
    Rubiks cube;
    auto solver = Solver::Create(Solver::L123);
    scan(cube, crawler);
    auto solution = solver->solve(cube);
    run(solution, crawler);
    */
    return 0;
}

int main(int argc, char *argv[])
{
    try
    {
        if (argc == 2 && strcmp(argv[1], "pc") == 0)
        {
            return run_pc();
        }
        else if (argc == 2 && strcmp(argv[1], "brick") == 0)
        {
            return run_brick();
        }
        else
        {
            cout << "USAGE: cube-crawler [pc|brick]\n";
            return 0;
        }
    }
    catch (exception const &e)
    {
        cerr << e.what() << '\n';
        return -1;
    }
}
