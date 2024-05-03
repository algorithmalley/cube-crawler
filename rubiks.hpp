#pragma once

class Rubiks
{
public:
    enum Face
    {
        LEFT,
        RIGHT,
        BACK,
        FRONT,
        DOWN,
        UP
    };

    void turn(Face face, int n);
};