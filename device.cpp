#include "device.hpp"
#include "ev3dev.h"
#include <thread>

using namespace std;
using namespace ev3dev;

sensor g_sensors[3]{{INPUT_1},  // (1) EV3 touch (device lego-ev3-touch, port
                                // ev3-ports:in1, mode TOUCH)
                    {INPUT_2},  // (2) EV3 color (device lego-ev3-color, port
                                // ev3-ports:in2, mode COL-REFLECT)
                    {INPUT_4}}; // (4) EV3 infrared (device lego-ev3-ir, port
                                // ev3-ports:in4, mode IR-PROX)

motor g_motors[3] = {{OUTPUT_A},  // (1) lego-ev3-l-motor motor on port ev3-ports:outA (BEAM)
                     {OUTPUT_B},  // (2) lego-ev3-m-motor motor on port ev3-ports:outB (TABLE)
                     {OUTPUT_C}}; // (3) lego-ev3-m-motor motor on port ev3-ports:outC (SCANNER)

enum Sensors : int { BUTTON, COLOR, CUBE };

enum Actuators : int { BEAM, TURNTABLE, SCANNER };

void waitidle(motor const &motor)
{
    while (motor.state().count("running") > 0)
    {
        chrono::milliseconds waitfor = chrono::milliseconds(10);
        this_thread::sleep_for(waitfor);
    }
}

Device::Device()
{
    // Init beam
    motor &beam = g_motors[Actuators::BEAM];
    beam.reset();
    beam.set_stop_action("hold");
    beam.set_speed_sp(150);
    beam.run_forever();
    while (beam.state().count("stalled") == 0)
    {
        chrono::milliseconds waitfor = chrono::milliseconds(10);
        this_thread::sleep_for(waitfor);
    }
    beam.stop();
    beam.set_position_sp(-35);
    beam.run_to_rel_pos();

    // Init turntable
    motor &turntable = g_motors[Actuators::TURNTABLE];
    turntable.reset();
    turntable.set_stop_action("hold");
    turntable.set_speed_sp(500);
    turntable.set_position_sp(-268);
}

Device::~Device()
{
    motor &beam = g_motors[Actuators::BEAM];
    beam.reset();

    motor &turntable = g_motors[Actuators::TURNTABLE];
    turntable.reset();
}

void Device::scan() {}

void Device::flip()
{
    motor &beam = g_motors[Actuators::BEAM];

    beam.set_position_sp(-215);
    beam.run_to_rel_pos();
    waitidle(beam);

    beam.set_position_sp(60);
    beam.run_to_rel_pos();
    waitidle(beam);

    beam.set_speed_sp(325);
    beam.set_position_sp(155);
    beam.run_to_rel_pos();
    beam.set_speed_sp(150);
    waitidle(beam);
}

void Device::turn(bool ccw)
{
    motor &beam = g_motors[Actuators::BEAM];

    beam.set_position_sp(-215);
    beam.run_to_rel_pos();
    waitidle(beam);

    beam.set_position_sp(60);
    beam.run_to_rel_pos();
    waitidle(beam);

    rotz(ccw);

    beam.set_speed_sp(325);
    beam.set_position_sp(155);
    beam.run_to_rel_pos();
    beam.set_speed_sp(150);
    waitidle(beam);
}

void Device::rotz(bool ccw)
{
    motor &turntable = g_motors[Actuators::TURNTABLE];
    int curr_pos_sp = turntable.position_sp();
    if ((ccw && curr_pos_sp > 0) || (!ccw && curr_pos_sp < 0))
    {
        turntable.set_position_sp(-curr_pos_sp);
    }
    turntable.run_to_rel_pos();
    waitidle(turntable);
}

void Device::tell(std::string const &msg) { sound::speak(msg, true); }
