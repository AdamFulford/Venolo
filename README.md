# Venolo
Stereo Tremolo Pedal with 3 assignable LFOs, tap tempo and midi control

2 seperate V2164 VCA channels controlled digitally using a Teensy 3.2 microcontroller and MCP4922 DAC.
Can be configured with jumpers on bottom of main PCB to be mono -> stereo, or full stereo. Low noise opamps with a fully analog audio path, and internally boosted rails (+/- 12v) give huge headroom before distortion, so the Venolo can also be used with line level and eurorack synths.

Runs off a standard guitar pedal 9V DC power supply with centre ground 2.1mm barrel connector. Approximate current consumption is 350mA.

Default mode:

LFO1 always affects the volume. Controls are:
- Frequency: Fully anti-clockwise is the slowest speed, fully clockwise is the fastest speed.
- Amplitude: Fully anti-clockwise is no volume modulation, fully clockwise is maximum modulation.
- Sway: Controls the ratio of time in the 'attack' phase and the 'decay' phase of the volume modulation waveshape. Fully anti- clockwise gives a very short attack and slow attack, much like the vintage Vox note repeater effect. Fully clockwise has a slow attack and fast decay, sounding like reverse attack, especially at slower frequencies. 12 o'clock gives a symmetrical waveshape.
- Attack phase waveshape: Choose between sine, triangle, square, first 1/2 sine, second 1/2 sine, and inverted sine.
- Decay phase waveshape: Same as attack phase waveshape, but for the decay phase.

LFO2 can be assigned to 1 of 4 functions (or turned off), by toggling the LFO2 assign switch to cycle through the settings:
- Lights off: Not assigned
- First light: Volume assignment
- Second light: Stereo pan assignment
- Third light: LFO1 frequency assignment. LFO2 varies the frequency of LFO1, with a mid point corresponding the current frequency of LFO1 (either set by the LFO1 pot or midi control).
- Fourth light: LFO3 frequency assignment. As above, but varies LFO3 frequency.

LFO2 controls:
- Frequency: Works over a generally slower range than LFO1.
- Amplitude: As for LFO1. When assigned to pan this controls the stereo width of the modulation, and when assigned to LFO1 frequency of LFO3 frequency this controls the amount of variation, within a range bounded by the minimum  and maximum frequency of the assigned LFO.
-Waveshape: Choose between sine, triangle and square.

LFO3 can be assigned to 1 of 2 functions (or turned off), by toggling the LFO3 assign switch to cycle through the settings:
- Lights off: Not assigned
- First light: Volume assignment
- Second light: Stereo pan assignment

LFO3 controls:
- Frequency (range as per LFO2)
- Amplitude (as per LFO1 & LFO2)
- Waveshape (as per LFO2)

Manual controls:

Volume: Sets the overall maximum level of the output when the pedal is engaged. Fully clockwise is a slight above unity gain. Fully anti-clockwise corresponds to almost no output signal (usually below noise level of typical guitar amps).

Pan: Sets the average pan position in the stereo field when the pedal is engaged.

Midi assign button: Can be used to map multiple controls to respond to an expression value received on CC#1 (channel 5). Mapping controls works like this:

1- Set all controls to the desired position at when CC#1 value = 0 
2- Press and hold the midi assign button (midi assign LED will light).
3- Whilst still holding the midi assign button, move any number of the knobs to a new position, corresponding to CC#1 = 127. The ranges can be inverted 1 or more controls if desired, but decreasing the knob position.
4- Release the midi assign button, and the settings will be mapped. If a knob change was mapped, the physical knob will no longer control the parameter.

To clear all midi expression mappings, tap the midi assign button without touching any of the knobs.
NB. waveshape controls cannot be mapped to midi.

The Left footswitch is a tap tempo for LFO1. One tap will reset LFO1 to it's peak position (i.e. max volume). Subsequent taps will vary LFO1's frequency to match the spacing between taps. Tempo can also be tapped in by sending a value of 127 to midi CC#2 (channel 5). Additionally, a tempo twice the tap speed can be tapped in by sending CC#3.

The Right footswitch toggles the tremolo effect on and off. When the engage LED is lit the tremolo is active, and when it is off the pedal outputs the signal at the bypass volume. NB. the LEDs above the tap tempo footswitch will continue to blink, to give visual feedback as to the frequency and waveshape assigned, even when it is not affecting the guitar signal.

Fixed Multiple Mode:

To enter fixed multiple mode hold down the tap and engage footswitches for 2 seconds. All LEDs will blink, and then display which mode you are changing to - LEDs on the right means Fixed Multple Mode, LEDs on the left means returning to default mode.

In fixed multiple mode, LFO2 and LFO3 frequencies are exact multiples (or ratios) of LFO1, with the following multiples selectable using the LFO2 and LFO3 frequency knobs, from slowest (fully counterclockwise) to fastest (fully clockwise): 1/36, 1/24, 1/12, 1/6, 1/2, 2/3, 1, 3/2, 2, 3.

By default, LFO2 and LFO3 will be in phase with LFO1, however you can alter the phase offset for each LFO by using the alternate functions of the volume and pan controls: Hold down the tap button (wait for 200ms), and whilst still holding tap adjust the phase offset from in phase (fully counter clockwise) to 180 degrees out of phase (fully counter clockwise). Release the tap button to return to normal function.

By selecting different ratios, waveshapes and phase offsets for each LFO, it is possible to create complex and interesting rhythmic patterns, which stay in sync with LFO1 (this can still be altered on the fly using tap tempo, with the LFO1 frequency control). Use the amplitude control to blend the amount of each LFO, allowing different accents in the rhythm.

NB. in fixed multiple mode LFO2 cannot be assigned to modulate the LFO1 or LFO3 frequencies. These modes will result in LFO2 not being assigned to anything.
