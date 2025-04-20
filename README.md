# Test driver

> This repository keeps the history of trials, errors and successes on how to lay out the structure of a layered architecture for embedded systems.

## Table of contents
* [General info](#general-info)
* [Technologies](#technologies)
* [Setup](#setup)
* [Features](#features)
* [Status](#status)

## General info

This project is for planning, implementing and maintaining a firmware architecture supporting multiple embedded platforms. To reach this goal, this project uses cmake to configure the build and a self-proposed hardware abstraction layer (HAL).

## Technologies

* Linux host system for the build, tested only on Ubuntu 22 (might work with [WSL2](https://learn.microsoft.com/en-us/windows/wsl/install))
* CMake version 3.23 or latter because of support to presets ([this link](https://cmake.org/download/))
* Ninja ([this link](https://ninja-build.org/))
* Visual Studio Code with the tools to build c/c++ projects and cmake tools ((this link)[https://code.visualstudio.com/docs/setup/setup-overview])
* Platform dependent dependencies, including tool chains and compilers

Code can be built for the following platforms:
* [Linux](drivers/linux) - HAL to access peripherals on linux systems

## Setup

For a more complete information on how to setup and run, visit [this readme file](drivers/linux/README.md).

### 1 - Download the repository:

* To instal all dependencies, run:
  ```bash
  sudo apt update -y && sudo apt upgrade -y && sudo apt install cmake ninja-build build-essential git gpiod libgpiod-dev i2c-tools gdb -y
  ```

* Clone the repository
```bash
git clone -b v1.0.0 https://github.com/passoswell/test_driver.git
```

* Enter the folder
```bash
cd test_driver
```

### 2 - Compile on linux using presets using the linux terminal

* List the presets available
```bash
cmake --list-presets
```

* Configure the desired preset. A corresponding folder will be created inside folder `build`.
```bash
cmake --preset <preset_name>
```

* Build the targets created in the cmake project
```bash
cmake --build ./build/<preset_name>
```

* The binaries are saved inside `build/<preset_name>/bin`

For specific instructions about a platform, please read the readme files on the respective folder.

## Features

Under construction

## Status

On-going.