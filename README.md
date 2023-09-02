# Multiplications plotted around a circle

I watched a video on YouTube about multiplication tables plotted around a circle.

A very simple algorithm creates these amazing patterns, so I thought I'd try my hand at it.  Written in C using libX11.

It looked especially intriguing on the high resolution monitor.  Some lower-res captures are here in a writeup I made if you want to get an idea:
https://brutelearning.wordpress.com/2020/04/25/multiplications-around-a-circle/

On the bottom right corner the multiplier is displayed.

Here was the YouTube video that inspired it:
https://youtu.be/qhbuKbxJsk8

## Usage
```bash
xtimestable [iter] [factor] [animate] [delay]
iter = number of lines around the graph
factor = multiplication factor
animate = mode of operation
              no argument -> only run one graph
              0 -> use any key to add one to the multiplier, p for previous, q to quit 
              1 -> automatically cycle through 
delay = delay between images in microseconds
```
## Example
**Draw a x2 cardioid with 200 lines (default), press any key to quit**
```bash
./xtimestable
```
**Draw the x61 plot with 270 lines, press any key to quit**
```bash
./xtimestable 270 61
```
**Draw the x77 plot with 800 lines in interactive mode**
```bash
./xtimestable 800 77 0
```
**Draw the animated plot starting at x2 with 400 lines, with a 20000 μsecond delay**
```bash
./xtimestable 400 2 1 20000
```
