# Test driver

## To download the repository:


```bash
git clone https://github.com/passoswell/test_driver.git
```

```bash
cd test_driver
```


## To compile on linux on the target machine (no cross compilation):

### Dependencies:

1. cmake 3.20 or later
2. gcc
3. git
4. build-essential
5. gdb (optional, for debugging)

To instal everything, run:

```bash
sudo apt update -y && sudo apt upgrade -y && sudo apt install cmake gcc git gdb -y
```

### Compilation process:

From the "test_driver" folder:

```bash
cmake -DCMAKE_C_COMPILER=/usr/bin/gcc -DCMAKE_BUILD_TYPE=Debug -DUSE_LINUX=1 -S. -B./build/gcc_linux
```

```bash
cmake --build ./build/gcc_linux --parallel $nproc
```

The "TEST_DRIVER" binary is on the "build/gcc_linux" folder.

### To run the Linux binary:

```bash
(sudo) ./build/gcc_linux/TEST_DRIVER
```



## To cross compile for the raspberry pi pico:

### Dependencies:

Instructions on how to setup the raspberry pi pico development environment can be found on [the raspberry pi website](https://datasheets.raspberrypi.com/pico/getting-started-with-pico.pdf).

### Compilation process:

From the "test_driver" folder:

```bash
cmake -DCMAKE_C_COMPILER=/usr/bin/arm-none-eabi-gcc -DCMAKE_BUILD_TYPE=Debug -DUSE_PIPICO=1 -S. -B./build/pipico
```

```bash
cmake --build ./build/pipico --parallel $nproc
```

The "TEST_DRIVER.elf" file is on the "build/pipico" folder.