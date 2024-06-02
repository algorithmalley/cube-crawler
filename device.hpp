#pragma once

#include "rubiks.hpp"
#include <map>
#include <string>

// Represents the LEGO EV3 cube crawler.
//
// The device maintains state how its faces are oriented. The device calls the face at the turntable 'down', the
// face near the beam 'left', and the face near the cube sensor 'back'. Initially, that is, when placed, the cube
// has the same orientation. But after one or more operations on the cube through the device the faces of the cube
// will be permuted. How they are can be obtained using the query 'permutation()'.
//
class Device
{
  public:
    enum Face : int { LEFT, RIGHT, BACK, FRONT, DOWN, UP };
    using DeviceFace = Device::Face;
    using RubiksFace = Rubiks::Face;

    /*
        Construction & Destruction:
    */

    // Constructs a cube-crawler device
    explicit Device();

    // Destructor leaves device resources nicely
    ~Device();

    /*
        Queries:
    */

    // Returns true iff device is connected and configured correctly
    bool valid() const;

    // Map that tells at which DeviceFace a CubeFace is located
    auto permutation() const -> std::map<RubiksFace, DeviceFace> const &;

    /*
        Commands:
    */

    // Bring the given device face to the turn table
    void prepare(DeviceFace face);

    // scan face that is up
    void scan();

    // flip once, brings face at scanner's side up
    void flip();

    // turn bottom layer once ccw or cw
    void turn(bool ccw = true);

    // rotate whole cube around z-axis once ccw or cw
    void rotate(bool ccw = true);

    // let cube-crawler speak a msg
    void tell(std::string const &msg);

  private:
    std::map<RubiksFace, DeviceFace> _state;
};

inline std::map<Device::RubiksFace, Device::DeviceFace> const &Device::permutation() const { return _state; }
