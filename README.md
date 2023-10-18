# 3DGameBridgeWrapper

This repo contains our wrapper for commom SR SDK functions required for weaving a stereoscopic image.
On top of this, some extra helper functions were added for handling the following cases:
* Lens switching
* Eye tracking
* Hotkey managing
* Event handling

The class diagram of the wrapper project can be seen below:
![SR Game Bridge Wrapper drawio](https://github.com/BramTeurlings/3DGameBridge/assets/25816950/f9aac6ae-a156-424c-b790-aad3e9248618)

### Build instructions:
1. Install the SR SDK from Dimenco's website (you will need to make an account): https://www.srappstore.com/develop/sdk/sr
2. Download CMAKE
3. Download Visual studio 2019 or up and install the C++ compiler
4. Download googletest from the official repository and copy googletest-{version} into "unit_test/lib/"
5. Open a terminal (like git bash) 
6. ``git submodule sync --recursive``
7. ``git submodule update --init --recursive``
8. ``"cmake . -B build"``

You should now be able to load the generated project inside the created folder named "build".

## Additional information
If you want to get an understanding of the underlying SR SDK, you can view the documentation and samples at the link below:
https://developer.srappstore.com/

For any questions or contributions you may have, please refer to our Discord server.

### Related Projects
3D Game Bridge Projects:
https://github.com/JoeyAnthony/3DGameBridgeProjects

3D Game Bridge GUI/API:
https://github.com/BramTeurlings/3DGameBridgeGUI

## Code conventions

### Include statements

Include statements should be ordered in the following way:
* Matching header file
* C++ standard headers
* Other included headers
* Defines

### Include guards
Every header file should be using #pragma once at the top of the file before any include statements.