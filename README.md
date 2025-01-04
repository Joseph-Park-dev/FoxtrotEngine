# Foxtrot Engine 2D [![License](https://img.shields.io/badge/license-GNUGPLv3-green)](./LICENSE)

<p align="center" href = "https://github.com/Joseph-Park-dev/FoxtrotEngine/">
<img src="./Logo/FoxtrotEngine_LOGO.png" width="55%" height="55%" title="LOGO">
</p>

<p align="center" href = "https://goocatstudio.itch.io/foxtrot-engine-2d">
<img src="./Logo/itch.io_badge-color.svg" width="20%" height="20%" title="itchioBadge">
</p>

Foxtrot Engine 2D is a 2D game engine based on DirectX 11 for Windows.
It includes Foxtrot Editor which allows users to create & modify scene data, and Foxtrot Engine which executes the produced game based on those data. Scene data are stored as a ".chunk" file, which is a file structure that describes the game data and the required resources.

Foxtrot Engine has been developed by JungBae Park from GooCat Studio for the game in development, **"SPARED"**.

## List of Features
- A component-based structure that allows to create custom Actors<br>

- Various Components to create behaviors for Actors<br>
&nbsp; - Sprite renderer <br>
&nbsp; - Tilemap renderer with .csv input <br>
&nbsp; - Sprite animation<br>
&nbsp; - Rigidbody simulation based on [box2D](https://box2d.org/) <br>
&nbsp; - 2D colliders in basic shapes (e.g. box colliders) <br>
&nbsp; - User input Management & Moving characters<br>
&nbsp; - etc.<br>

- User-defined Components can be added as "Behavior"s<br>

- Create & Modify pre-defined Actors, "Premade" <br> (a similar concept to Unity Engine's "Prefab") 

## Getting Started
### 0. Development environment
Visual Studio 2022 is recommended since the project is not built and tested on other development environments.
### 1. Downloading the repository
Simply clone the repository or download as a .zip file.
### 2. Check for project configuration
Windows SDK Version: 10.0 <br>
Platform Toolset: Visual Studio 2022 (v143) <br>
C++ Language Standard: ISO C++17 Standard (/std:c++17) <br>
Vcpkg is enabled, using Manifest. Check out the vcpkg file for package info.

### 3. Use the correct solution configuration
Use **Foxtrot_Debug** when building a game in production. <br>
**Foxtrot_Editor_Debug** is used for updating Foxtrot Editor so unless you are working on improving the Editor, it is not necessary.

### 4. Build the Projects
#### a. Building Foxtrot Editor
Set the configuration to **"Foxtrot_Editor_Debug"**. <br>
Build the project **"FoxtrotEngine"** first, and then build the project **"FoxtrotEditor"**. The output "FoxtrotEditor.exe" should be in **"{root}/FoxtrotEditor/Builds"**.<br>
The user manual is not provided yet, but you can still play around with the editor. <br>

**IMPORTANT:** the files, "Game Data.gdpack" and "imgui.ini" are **CRUCIAL** for Foxtrot Editor to work properly. Keep them in the Builds folder. <br>

## User Manual
This section will be updated.

## Plans and Goals
Foxtrot Engine has been developed not only as a portfolio but also as a tool of education to obtain game development experiences. The developer also expects to develop this project until it can produce a finished 2D game title. Here is the list of upcoming features and goals.

- Artificial intelligence for characters (current WIP)<br>
- Spine animation renderer <br>
- Particle system<br>
- UI testing (this was implemented, but not tested)<br>
- Establishment of the workflow of Foxtrot Engine; The development routine <br>
- Finishing the prototype version of the game, **"SPARED"**.

## Additional Info.
Some rendering utility functions related to DirectX 11 were made using educational resources from [HongLab](https://honglab.co.kr/). <br>
몇몇 DX11 렌더링 함수와 관련하여, [홍정모 연구소](https://honglab.co.kr/)의 컴퓨터 그래픽스 새싹코스를 참고하였습니다.