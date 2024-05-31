#pragma once

#include <string>

class Device
{
  public:
    explicit Device();
    ~Device();

    // scan face that is up
    void scan();

    // flip once, brings face at scanner's side up
    void flip();

    // turn bottom layer once ccw or cw
    void turn(bool ccw = true);

    // rotate whole cube around z-axis once ccw or cw
    void rotz(bool ccw = true);

    // let cube-crawler speak a msg
    void tell(std::string const &msg);
};