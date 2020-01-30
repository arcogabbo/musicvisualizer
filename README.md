## C and SDL music visualizer

This simple real-time visualizer calculates the dft of a given stream and output the magnitude of his frequencies

Sample usage with given options size and file (if you avoid the size it will be 2048 by default):

`./visualizer -s 2048 -f 'some file.wav'`

**NOTE: The audio format must be .wav (MP3 support still have to be implemented)**

Dependencies:
- SDL2
- FFTW3

You can easily find those in Arch extra packages:
```
sudo pacman -S sdl2
sudo pacman -S fftw
```
Or just install them yourself

**__Have fun!__**

>It should be compatible with windows but i have to try it yet
