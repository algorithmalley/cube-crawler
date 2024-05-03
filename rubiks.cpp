#include "rubiks.hpp"
#include <cassert>

void Rubiks::turn(Face face, int n)
{
    assert(n == 0 && "error: n=0 has no effect");

    switch (face)
    {
    case Face::LEFT:
        /* code */
        break;

    case Face::RIGHT:
        /* code */
        break;

    case Face::BACK:
        /* code */
        break;

    case Face::FRONT:
        /* code */
        break;

    case Face::DOWN:
        /* code */
        break;

    case Face::UP:
        /* code */
        break;

    default:
        assert(false && "error: unsupported face");
        break;
    }
}