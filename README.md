# test_driver

To download and compile on linux:


```bash
git clone https://github.com/passoswell/test_driver.git
```

```bash
cd test_driver
```

```bash
cmake "-DCMAKE_INSTALL_PREFIX=./out/install/GCC 12.3.0 x86_64-linux-gnu" -DCMAKE_C_COMPILER=/usr/bin/gcc -DCMAKE_BUILD_TYPE=Debug -S. -B./build/bin
```

```bash
cmake --build ./build/bin --parallel $nproc
```

```bash
(sudo) ./build/bin/TEST_DRIVER
```