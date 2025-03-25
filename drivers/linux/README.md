# Peripheral HAL for Linux systems

> C++ HAL for Linux Single Board Computers.

## Table of contents
* [General info](#general-info)
* [Technologies](#technologies)
* [Setup](#setup)
* [Features](#features)
* [Status](#status)

## General info

Under construction.

## Technologies

1. cmake 3.23 or later to setup the build
2. Ninja - build system
3. build-essential - meta-package that lists programs that will be needed if you want to develop debian packages
4. gcc compiler
5. gcc++ compiler
6. git - clone repositories
7. gpiod (optional) - access to gpio using linux terminal
8. libgpiod-dev - access to gpio header files
9. i2c-tools (optional) - access to i2c using linux terminal
9. gdb (optional) for debugging

## Setup

* To instal all dependencies, run:
  ```bash
  sudo apt update -y && sudo apt upgrade -y && sudo apt install cmake ninja-build build-essential git gpiod libgpiod-dev i2c-tools gdb -y
  ```

* Getting access to peripherals:
The linux peripherals are listed on `/dev` folder. The names are usually composed of a prefix followed by a number. The easy to identify prefixes are listed bellow:

    * `tty` - console terminals, uart ports and serial-to-usb converters
    * `i2c` - i2c ports
    * `spidev` - spi ports
    * `gpiochip` - bank of gpio pins
    * `video` - cameras and webcams

  These peripherals usually comme disabled by default and are not listed on `/dev` folder. In this case, use the documentation of the linux distribution you are using to look for information on how to enable it. It might be the case to configure device tree overlays.

  On occasion, the peripheral is accessible only to root user. In this case, the peripheral and the user must be added to the same group. The group name is most of the time the peripheral's type name: `i2c`, `spi`, `gpio` and so on. For terminal interfaces, such as uart ports, the group's name is `dialout`. To create a group and add your user to it:

    ```bash
    sudo addgroup <group_name>
    sudo usermod -a -G <group_name> <user_name>
    ```
  For instance, for spi peripherals:

    ```bash
    sudo addgroup spi
    sudo usermod -a -G spi <user_name>
    ```

  In the case of gpio, it comes dissociated of any group, that means it is not accessible to root by default in most distros. Use the bash commands bellow:

    ```bash
    sudo addgroup gpio
    sudo usermod -a -G gpio <user_name>
    sudo chown root:gpio /dev/gpiochip0 # Change the number of the gpiochip according to your needs
    ```

* Clone the repository
  ```bash
  git clone https://github.com/passoswell/test_driver.git
  ```

* Enter the project folder
  ```bash
  cd test_driver
  ```

### Native build

1. **To build using presets without vscode**

* From the "test_driver" folder, list the presets available
  ```bash
  cmake --list-presets
  ```

* Configure linux preset
  ```bash
  cmake --preset linux
  ```

  To configure in release mode, use the following instead:
  ```bash
  cmake --preset linux_release
  ```

  A folder called `linux` will be created inside folder `build`.


* Build the targets created in the cmake project
  ```bash
  cmake --build ./build/linux
  ```

  The binaries for all cmake targets are inside `build/linux/bin`.

2. **To build without preset and  without vscode**

* From the "test_driver" folder:
  ```bash
  cmake -DCMAKE_C_COMPILER=/usr/bin/gcc -DCMAKE_BUILD_TYPE=Debug -DUSE_LINUX=1 -S. -B./build/linux
  ```

  ```bash
  cmake --build ./build/linux --parallel $nproc
  ```

  The binaries for all cmake targets are inside `build/linux/bin`.

3. **To run the Linux binary for the main target:**
*
  ```bash
  (sudo) ./build/linux/bin/TEST_DRIVER
  ```
