# MMMX GAME

Multiplayer 2D shooter with upper view.
![image](https://user-images.githubusercontent.com/45922618/119276771-99327f00-bc24-11eb-864a-9105292ec3d2.png)

## Getting Started

These instructions will get you a copy of the project up and running on your local machine for development and testing purposes. See deployment for notes on how to deploy the project on a live system.

### Prerequisites

To run project you need at least 5.15.2 QT.

### Installing

```
git pull origin master
```

Build project using MINGW by QT and CMake.

Run MMMX-Server. By default, the game server will be running on localhost:1337.
After you need to run MMMX-Client with -local flag to connect to localhost, otherwise client will try to connect to global server in Moscow.

## Running the codding style tests

Github Actions use 
- cppcheck - static code analysis
- cpplint - static code analysis
- cmake - building project

## Built With

* QT
* OpenGL

## Authors

* **Kliment Markevich**
    - *Initial server-client work*
    - *Animations*

    [sn0wyQ](https://github.com/sn0wyQ)
* **Konstantin Mekhovich** 
    - *Server-room-client connection system rework*
    - *Collisions*
    - *Leveling System*
    - *Interpolation and lag compensation*
    
    [kmekhovich](https://github.com/kmekhovich)
* **Matvey Maximov** 
    - *FOV*
    - *Player Stats*
    - *Creeps*
    - *Creep Settings*
    - *All smooth UI animations*
    - *Spawn Generator*

    [OS-M](https://github.com/OS-M)
* **Alexander Khrapko** 
    - *All system of weapons and bullets*
    - *Game Balance*
    - *Beautiful pictures in player bar*

    [Cal-lTel-luK](https://github.com/Cal-lTel-luK)

See also the list of [contributors](https://github.com/sn0wyQ/MMMX/contributors) who participated in this project.
