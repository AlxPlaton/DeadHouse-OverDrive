Welcome to the DeadHouse Overdrive project. This is a custom-built distortion/overdrive pedal developed using C++ and the JUCE framework.
WARNING! This is a Beta Build, unfinished and mostly nonfunctional. I aim at full functionallity asap.
I am a junior developer, any issues regarding this project please message me!

Current Status: Unfinished Beta
This project is currently in active development.

Standalone App: Works well and is stable for testing.

VST3/AU Versions: Currently being edited and optimized. You may experience some bugs or "glitchy" audio in certain DAWs (like FL Studio) while we refine the buffer logic.

Features (In-Progress)
Hard/Soft Clipping: Custom distortion algorithms for a gritty, Thrash Metal sound.

Responsive UI: Custom-designed knobs and interface.

Optimized for Mono/Stereo: Actively working on the signal path logic to ensure perfect phase alignment.

🛠️ For Developers
If you want to compile this yourself:

Clone the repository.

Open DeadHouse Overdrive.jucer in the Projucer.

Ensure you have the JUCE modules installed.

Export to Visual Studio 2022 (or your preferred IDE) and build.

📝 To-Do List
Fix VST3 buffer synchronization issues.

Finalize the Tone circuit logic.

Add Oversampling to reduce aliasing at high gain.
