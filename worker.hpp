#pragma once

class Rubiks;
class Device;

class Worker
{
  public:
    // Scan and init cube as it is on the device
    void scan(Rubiks &cube, Device &crawler);

    // Solve cube, optionally physically on the crawler too
    void solve(Rubiks &cube, Device *crawler = nullptr);

    // Scamble cube, optionally physically on the crawler too
    void scramble(Rubiks &cube, Device *crawler = nullptr);
};
