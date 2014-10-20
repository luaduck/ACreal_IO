ACreal IO
========
ACreal IO is a firmware code for Arduino to build Konami ACIO compatible boards.

Konami ACIO is a protocol used for communication between arcade games and harware components such as e-Amusement readers.
Its uses rs232 and allows several "nodes" to be connected a single serial port.

Fork notes
-------------
**This fork is currently only compatible with 5x5 matriced keypads (those with 10 pins).** Support will be added for switching between 5x5 and 3x4 at a later point, or feel free to fork this yourself and add it to be merged back in.

To facilitate this, the default pinouts are changed from 27-35 to **30-48 even**. See pinoutconfig.h for more information.

We also have [bemanidave's LCD support](https://github.com/bemanidave/ACreal_IO) merged in, with the same pins (53-43 odd). If you want to use these pins for something else, change them to other values in pinoutconfig.h (disabling use_lcd breaks compilation for some reason).

Games support
-------------
Currently this project supports the following games :

pop'n music 15-18 (one old slotted reader)

pop'n music 19-21 (one new wavepass reader with encrypted communication (only game I know which uses this encryption) + optional card dispenser)

iidx 13-18 (2 old slotted readers)

iidx 19-20 (2 new wavepass readers)

jubeat 1 - knit (one old slotted reader (no keypad) + one led board (game won't boot if the led board is not present) 

jubeat copious - saucer (one new reader (no keypad) + one led board (game won't boot if the led board is not present) 

DDR SN-SN2 (2 old slotted readers)

DDR X-X3 (2 readers (old or new) in sd, 2 readers (old or new) + led board ? in HD)

drum mania (1 old reader)

guitar freaks (2 old readers)

Sound voltex (1 new wavepass reader + 1 IoBoard )