#include "device.hpp"
#include "ev3dev.h"

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

enum class Sensors : int { BUTTON, COLOR, CUBE };

enum class Actuators : int { BEAM, TURNTABLE, SCANNER };

void Device::flip_cube() {}

void Device::turn_cube() {}
