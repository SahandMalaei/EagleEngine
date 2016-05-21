# Eagle Engine

**Eagle engine** is a lightweight game engine for creating games using **C++** and **[EagleScript][]**, created to facilitate the process of making games for the **Windows** and **Windows Phone** platforms.

## Features

- A fairly capable 2D/3D graphics system, featuring:
	- Animated sprites
	- Bitmap fonts
	- A camera system
	- Heavily optimized particle systems
	- A 3D model loading and rendering system
	- Basic GUI controls, such as text boxes and buttons
	- A 2D lighting system
- A powerful audio system, supporting 3D sound
- An input management system, with support for joysticks
- A helper library for mathematical operations
- A simple networking system allowing for communications using the UDP protocol
- [EagleScript][], a full-fledged scripting language featuring :
	- C-like syntax
	- A compiler and assembler toolchain for generating virtual machine bytecode
	- A virtual machine for executing script bytecodes
	- An interface fully embeddable and extendable through C++
- A powerful data-driven, component-based entity system, completely integrated into the EagleScript
- Debugging facilities, including a drop-down console which allows for input using the EagleScript language

#### Supported Platforms

- **Windows** (7 and higher) (x86/x64)
- **Windows Phone 8**

## Development Status

**This project is no longer under development.**

EagleEngine started merely as a high school project, though its development continued for over three years.
At the time of development it was particularly significant for the developer because of its role as an environment for testing the newly learned techniques in game development. That being said, this code is by no means well-structured as it has just grown haphazardly over time, without a clear long-term plan in mind.

**Here is why one should _not_ consider using this engine in their game project:**

- It suffers from complete lack of documentation;
- The coding conventions are old and ineffective at best;
- The code does not make a good use of C++;
- The different parts of the engine are heavily coupled which makes refactoring and maintaining code cumbersome;
- The code is not optimized, except for a few parts like the particle system;
- After the port from Direct3D 9 to Direct3D 11, some parts (like the mesh rendering system) stopped functioning and no attempt was made to restore them to their functional state;
- The code comes with no license or warranty;
- Its use of git is simply horrible.

## Libraries used

- **[Direct3D 11][Direct3D 11]** for graphics rendering
- **[FMOD Ex][FMOD Ex]** for the audio system

[EagleScript]: https://github.com/SahandMalaei/EagleScript "EagleScript"
[Direct3D 11]: https://msdn.microsoft.com/en-us/library/windows/desktop/ff476080(v=vs.85).aspx "Direct3D 11"
[FMOD Ex]: http://www.fmod.org/fmod-ex/ "FMOD Ex"