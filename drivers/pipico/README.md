


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