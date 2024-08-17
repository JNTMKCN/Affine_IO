#pragma once

/*
   CHUNITHM CUSTOM IO API

   Changelog:

   - 0x0100: Initial API version (assumed if chuni_io_get_api_version is not
     exported)
   - 0x0101: Fix IR beam mappings
   - 0x0102: Add air tower led and billboard support
*/

#include <windows.h>

#include <stdbool.h>
#include <stdint.h>

enum {
    CHUNI_IO_OPBTN_TEST = 0x01,
    CHUNI_IO_OPBTN_SERVICE = 0x02,
    CHUNI_IO_OPBTN_COIN = 0x04,
};

/* Get the version of the Chunithm IO API that this DLL supports. This
   function should return a positive 16-bit integer, where the high byte is
   the major version and the low byte is the minor version (as defined by the
   Semantic Versioning standard).

   The latest API version as of this writing is 0x0101. */

uint16_t chuni_io_get_api_version(void);

/* Initialize JVS-based input. This function will be called before any other
   chuni_io_jvs_*() function calls. Errors returned from this function will
   manifest as a disconnected JVS bus.

   All subsequent calls may originate from arbitrary threads and some may
   overlap with each other. Ensuring synchronization inside your IO DLL is
   your responsibility.

   Minimum API version: 0x0100 */

HRESULT chuni_io_jvs_init(void);

/* Poll JVS input.

   opbtn returns the cabinet test/service state, where bit 0 is Test and Bit 1
   is Service.

   beam returns the IR beams that are currently broken, where bit 0 is the
   lowest IR beam and bit 5 is the highest IR beam, for a total of six beams.

   Both bit masks are active-high.

   Note that you cannot instantly break the entire IR grid in a single frame to
   simulate hand movement; this will be judged as a miss. You need to simulate
   a gradual raising and lowering of the hands. Consult the proof-of-concept
   implementation for details.

   NOTE: Previous releases of Segatools mapped the IR beam inputs incorrectly.
   Please ensure that you advertise an API version of at least 0x0101 so that
   the correct mapping can be used.

   Minimum API version: 0x0100
   Latest API version: 0x0101 */

void chuni_io_jvs_poll(uint8_t *opbtn, uint8_t *beams);

/* Read the current state of the coin counter. This value should be incremented
   for every coin detected by the coin acceptor mechanism. This count does not
   need to persist beyond the lifetime of the process.

   Minimum API version: 0x0100 */

void chuni_io_jvs_read_coin_counter(uint16_t *total);


/* Initialize touch slider emulation. This function will be called before any
   other chuni_io_slider_*() function calls.

   All subsequent calls may originate from arbitrary threads and some may
   overlap with each other. Ensuring synchronization inside your IO DLL is
   your responsibility.

   Minimum API version: 0x0100 */

HRESULT chuni_io_slider_init(void);

/* Chunithm touch slider layout:

                               ^^^ Toward screen ^^^

----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
 31 | 29 | 27 | 25 | 23 | 21 | 19 | 17 | 15 | 13 | 11 |  9 |  7 |  5 |  3 |  1 |
----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
 32 | 30 | 28 | 26 | 24 | 22 | 20 | 18 | 16 | 14 | 12 | 10 |  8 |  6 |  4 |  2 |
----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+

   There are a total of 32 regions on the touch slider. Each region can return
   an 8-bit pressure value. The operator menu allows the operator to adjust the
   pressure level at which a region is considered to be pressed; the factory
   default value for this setting is 20. */

/* Callback function supplied to your IO DLL. This must be called with a
   pointer to a 32-byte array of pressure values, one byte per slider cell.
   See above for layout and pressure threshold information.

   The callback will copy the pressure state data out of your buffer before
   returning. The pointer will not be retained. */

typedef void (*chuni_io_slider_callback_t)(const uint8_t *state);

/* Start polling the slider. Your DLL must start a polling thread and call the
   supplied function periodically from that thread with new input state. The
   update interval is up to you, but if your input device doesn't have any
   preferred interval then 1 kHz is a reasonable maximum frequency.

   Note that you do have to have to call the callback "occasionally" even if
   nothing is changing, otherwise the game will raise a comm timeout error.

   Minimum API version: 0x0100 */

void chuni_io_slider_start(chuni_io_slider_callback_t callback);

/* Stop polling the slider. You must cease to invoke the input callback before
   returning from this function.

   This *will* be called in the course of regular operation. For example,
   every time you go into the operator menu the slider and all of the other I/O
   on the cabinet gets restarted.

   Following on from the above, the slider polling loop *will* be restarted
   after being stopped in the course of regular operation. Do not permanently
   tear down your input driver in response to this function call.

   Minimum API version: 0x0100 */

void chuni_io_slider_stop(void);

/* Update the RGB lighting on the slider. A pointer to an array of 32 * 3 = 96
  bytes is supplied, organized in BRG format.
   The first set of bytes is the right-most slider key, and from there the bytes
   alternate between the dividers and the keys until the left-most key. 
   There are 31 illuminated sections in total.

   Minimum API version: 0x0100 */

void chuni_io_slider_set_leds(const uint8_t *rgb);

/* Initialize LED emulation. This function will be called before any
   other chuni_io_led_*() function calls.

   All subsequent calls may originate from arbitrary threads and some may
   overlap with each other. Ensuring synchronization inside your IO DLL is
   your responsibility. 

   Minimum API version: 0x0102 */

HRESULT chuni_io_led_init(void);

/* Update the RGB LEDs. rgb is a pointer to an array of up to 63 * 3 = 189 bytes.

   Chunithm uses two chains/boards with WS2811 protocol (each logical led corresponds to 3 physical leds). 
   board 0 is on the left side and board 1 on the right side of the cab

   Board 0 has 53 LEDs:
     [0]-[49]: snakes through left half of billboard (first column starts at top)
     [50]-[52]: left side partition LEDs
   
   Board 1 has 63 LEDs:
     [0]-[59]: right half of billboard (first column starts at bottom)
     [60]-[62]: right side partition LEDs
   
   Board 2 is the slider and has 31 LEDs:
     [0]-[31]: slider LEDs right to left BRG, alternating between keys and dividers 

   Each rgb value is comprised of 3 bytes in R,G,B order

   NOTE: billboard strips have alternating direction (bottom to top, top to bottom, ...)

   Minimum API version: 0x0102 */

void chuni_io_led_set_colors(uint8_t board, uint8_t *rgb);