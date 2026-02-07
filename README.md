<b>Version 1.0.1 
Developed by Alexander Platon</b>

Welcome to the DeadHouse Overdrive project. This is a custom-built distortion/overdrive pedal developed using C++ and the JUCE framework.
WARNING! This is a Beta Build, unfinished and mostly functional. I aim at full functionallity asap.
I am a junior developer, any issues regarding this project please message me!
Or Email me, Ralexplaton@msn.com

Current Status: Unfinished Beta Ver 1.0.1
This project is in active development and is entirely self developed.


Standalone App: Works well and is stable for testing. 
VST3/AU Versions: Currently being edited and optimized. You may experience some bugs or "glitchy" audio in certain DAWs (like FL Studio) while I refine the buffer logic.

Features (In-Progress)
Hard/Soft Clipping: Custom distortion algorithms for a gritty, Thrash Metal sound.

Responsive UI: Custom-designed knobs and interface.

Optimized for Mono/Stereo: Actively working on the signal path logic to ensure perfect phase alignment.


<b>** For Developers **</b>

If you want to compile this yourself:

1) Clone the repository.
   (All files were complied within Visual Studio 18, so I reccomend cloning the repo there)

2) Open DeadHouse Overdrive.jucer in Projucer.

3) Ensure you have the JUCE modules installed.


<b>** To-Do List **</b>

Optimize code and memory usage.

Fix VST3 buffer synchronization issues.

Finalize the Tone circuit logic.

Add Oversampling to reduce aliasing at high gain.
