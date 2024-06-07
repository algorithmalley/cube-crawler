#include "device.hpp"
#include "rubiks.hpp"
#include "solver.hpp"
#include "worker.hpp"
#include <csignal>
#include <cstring>
#include <exception>
#include <fstream>
#include <iostream>

using namespace std;

#pragma region extern "C"

namespace {

volatile bool _interrupted = false;
bool interrupted() { return _interrupted; };

void sigint_ignore(int /*s*/)
{
    std::cout << "Busy shutting down, one moment please...\n";
    signal(SIGINT, sigint_ignore); // ignore subsequent ctrl-c's
}

void sigint_graciously(int /*s*/)
{
    std::cout << "...\n";
    _interrupted = true;
    signal(SIGINT, sigint_ignore); // ignore subsequent ctrl-c's
}

} // namespace

#pragma endregion

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

    if (!crawler.valid())
        throw runtime_error("LEGO Cube-Crawler not valid");

    crawler.tell("cube detected!");

    Rubiks cube;
    auto solver = Solver::Create(Solver::L123);

    crawler.tell("scrambling!");

    if (!interrupted())
    {
        auto problem = solver->scramble(cube);
        run(problem, crawler, interrupted);
    }

    crawler.tell("solving!");

    if (!interrupted())
    {
        auto solution = solver->solve(cube);
        run(solution, crawler, interrupted);
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
    int retcode = 0;

    signal(SIGINT, sigint_graciously);

    try
    {
        if (argc == 2 && strcmp(argv[1], "pc") == 0)
        {
            retcode = run_pc();
        }
        else if (argc == 2 && strcmp(argv[1], "brick") == 0)
        {
            retcode = run_brick();
        }
        else
        {
            cout << "USAGE: cube-crawler [pc|brick]\n";
        }
    }
    catch (exception const &e)
    {
        cerr << e.what() << '\n';
        retcode = -1;
    }

    signal(SIGINT, nullptr);

    return retcode;
}
