# Periodic Interval Timer Driver (DrvPIT)

## Table of Contents

- [Introduction](#introduction)
- [Installation](#installation)
- [Usage](#usage)
- [Example](#example)
- [API Reference](#api-reference)
- [Contributing](#contributing)
- [Contact](#contact)

## Introduction

The `DrvPIT` class provides a C++ implementation for handling the Periodic Interval Timer (PIT). This timer is commonly used for generating periodic interrupts or time delays in embedded systems. The class allows configuration, control, and utilization of the PIT for various timing-related functionalities.

## Installation

To use the `DrvPIT` class, follow these steps:

1. Clone the repository containing `drv_pit.hpp` into the source folder of your project.
2. Include the `drv_pit.hpp` header file in your project.
3. Ensure that the necessary dependencies and configurations are in place.

## Usage


```c++
#include "drv_pit.hpp"

int main() 
{
  // Instantiate the DrvPIT object
  DrvPIT pitTimer;

  // Initialize the PIT timer
  pitTimer.initialize();

  // Configure the PIT for a 1-second timeout
  uint32_t counts = 1000;  // Count for a 1-second timeout
  driverPITUnits_t unit = MilliSec;  // Specifies the time unit (milliseconds in this case)
  pitTimer.write(counts, unit);

  while (1) 
  {
    // The main code can execute here
    // ...

    // Wait until the timer is completed
    if (pitTimer.read()) 
    {
      // Reconfiguring the timer
      pitTimer.write(counts, unit);
      
      // Logic to be executed when the timer is completed
      // For example, toggle an LED or perform some periodic action
      // ...
    }
  }

  return 0;
}

```

To use the `DrvPIT` driver, follow these steps:

1. Create an instance of the `DrvPIT` class.
2. Initialize the PIT using the `initialize` method.
3. Configure the PIT with desired parameters using the `write` method.
4. Read the status of the PIT using the `read` method.
5. Halt or resume the PIT using the `halt` and `resume` methods.
6. Retrieve the current counter value using the `readCounter` method.
7. Optionally, use the static `readPITCounter` method for global PIT counter access.
8. Utilize the `delay` method for introducing delays in your application.

## Example



## API Reference

### Constructor

- `DrvPIT()`

   Constructor for the `DrvPIT` class.

### Public Methods

1. **Initialize Method**

   - `void initialize()`

   Initializes the PIT driver.

2. **Write Method**

   - `bool write(uint32_t CountsToPerform, driverPITUnits_t Unit)`

   Configures the PIT with the specified counts and time unit.

   - Parameters:
      - `CountsToPerform` (uint32_t): Number of counts to perform.
      - `Unit` (driverPITUnits_t): Time unit for the counts.

   - Returns:
      - `true` if configuration is successful, `false` otherwise.

3. **Read Method**

   - `bool read()`

   Reads the status of the PIT.

   - Returns:
      - `true` if the timer has completed counting, `false` otherwise.

4. **Halt Method**

   - `bool halt()`

   Halts the PIT, pausing the timer.

   - Returns:
      - `true` if the operation is successful, `false` otherwise.

5. **Resume Method**

   - `bool resume()`

   Resumes the PIT after a halt.

   - Returns:
      - `true` if the operation is successful, `false` otherwise.

6. **Read Counter Method**

   - `uint32_t readCounter()`

   Retrieves the current counter value of the PIT.

   - Returns:
      - The current counter value.

7. **Static Read PIT Counter Method**

   - `static uint32_t readPITCounter()`

   Retrieves the global PIT counter value.

   - Returns:
      - The current global PIT counter value.

8. **Delay Method**

   - `static void delay(uint32_t value)`

   Introduces a delay using the PIT.

   - Parameters:
      - `value` (uint32_t): The delay value.

## Contributing

Contributions are welcome! If you find any issues or have suggestions for improvements, please open an issue or submit a pull request in the repository.

## Contact

For any questions or further assistance, please feel free to contact the author:

* Contact (contact@email.com)