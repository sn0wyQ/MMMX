# MMMX GAME

MMMX Game is a multiplayer 2D shooter with view from above written on C++ using QT framework and OpenGL.
![image](https://user-images.githubusercontent.com/45922618/119276771-99327f00-bc24-11eb-864a-9105292ec3d2.png)
 
## Features

- Server-rooms-clients connection
- Collisions
- Interpolation and lag compensation
- Hero leveling
- Smooth UI animations
- Animations on svg with own language to play them variatively
- Optimized data packets
- In-game player statistics
- 4 heroes with unique weapons
- Smart spawn point generation
- Rending on graphics card using OpenGL
- Smart creeps (IQ 200+)
- Kill feed
- Key rebind ability

### Prerequisites

To run project you need at least 5.15.2 QT.

### Installing

Clone project from repository. Build project with Cmake on MINGW, QMINGW or Clang, link it with QT.

Run MMMX-Server. By default, the game server will be running on localhost:1337.
Then run MMMX-Client with -local flag to connect to localhost, otherwise client will try to connect to global server in Moscow.

### Master branch build status
[![Build Status](https://github.com/sn0wyQ/MMMX/actions/workflows/ci.yml/badge.svg?branch=master)](https://github.com/sn0wyQ/MMMX/actions/workflows/ci.yml)

## Running the codding style tests

Github Actions in use 
- cppcheck - static code analysis
- cpplint - static code analysis
- cmake - building project

## Authors

* **Kliment Markevich**
    - *Server-rooms-clients connection*
    - *Animations*

    [sn0wyQ](https://github.com/sn0wyQ)
* **Konstantin Mekhovich** 
    - *Collisions*
    - *Leveling System*
    - *Interpolation and lag compensation*
    
    [kmekhovich](https://github.com/kmekhovich)
* **Matvey Maximov** 
    - *FOV*
    - *Player stats*
    - *Creep intelligence*
    - *All smooth UI animations*
    - *Spawn generator*
    - *Packet collector*
    - *Rendering on graphics card using Open GL*
    - *Key rebinds*
    - *Kill feed*
    - *Savable settings*

    [OS-M](https://github.com/OS-M)
* **Alexander Khrapko** 
    - *All system of weapons and bullets*
    - *Game Balance*
    - *Beautiful pictures in player bar*

    [Cal-lTel-luK](https://github.com/Cal-lTel-luK)

See also the list of [contributors](https://github.com/sn0wyQ/MMMX/contributors) who participated in this project.
