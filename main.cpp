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

        cout << "querying cube...\n";
        for (auto face : {Rubiks::LEFT, Rubiks::RIGHT, Rubiks::BACK, Rubiks::FRONT, Rubiks::DOWN, Rubiks::UP})
        {
            cout << face << ":\n";
            Rubiks::Nibble center_piece;
            std::tie(center_piece) = cube.center_piece(face);
            cout << "Color of center piece: " << center_piece.color << "\n";
            auto scps = cube.side_center_pieces(center_piece.color);
            cout << "Side center pieces with color " << center_piece.color << ":\n";
            cout << " - " << scps[0] << "\n";
            cout << " - " << scps[1] << "\n";
            cout << " - " << scps[2] << "\n";
            cout << " - " << scps[3] << "\n";
            auto cps = cube.corner_pieces(center_piece.color);
            cout << "Corner pieces with color " << center_piece.color << ":\n";
            cout << " - " << cps[0] << "\n";
            cout << " - " << cps[1] << "\n";
            cout << " - " << cps[2] << "\n";
            cout << " - " << cps[3] << "\n";
        }

        cout << "scrambling cube...\n";
        auto steps = solver->scramble(cube);
        cout << cube;
        cout << "cube status: " << (cube.valid() ? "valid" : "invalid") << "\n";
        cout << "cube status: " << (cube.solved() ? "solved" : "scrambled") << "\n";
        cout << "cube entropy: " << cube.entropy() << " (" << steps.size() << " steps)\n";
        for (auto const &step : steps)
        {
            cout << " - turn " << step.first << " " << step.second << " steps\n";
        }

        /*
        cout << "solving cube...\n";
        solver->solve(cube);
        cout << cube;
        cout << "cube status: " << (cube.valid() ? "valid" : "invalid") << "\n";
        cout << "cube status: " << (cube.solved() ? "solved" : "scrambled") << "\n";
        cout << "cube entropy: " << cube.entropy() << "\n";
        */

        return 0;
    }
    catch (exception const &e)
    {
        cerr << e.what() << '\n';
        return -1;
    }
}
