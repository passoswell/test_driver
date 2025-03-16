/**
 * @file iic_types.hpp
 * @author your name (you@domain.com)
 * @brief Useful type definitions for IIC on linux
 * @version 0.1
 * @date 2024-10-13
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef DRIVERS_LINUX_IIC_IIC_TYPES_HPP
#define DRIVERS_LINUX_IIC_IIC_TYPES_HPP

/* ----- commands for the ioctl like i2c_command call:
 * note that additional calls are defined in the algorithm and hw
 *  dependent layers - these can be listed here, or see the
 *  corresponding header files.
 */
        /* -> bit-adapter specific ioctls  */
#define I2C_RETRIES  0x0701  /* number of times a device address      */
        /* should be polled when not            */
                                /* acknowledging       */
#define I2C_TIMEOUT  0x0702  /* set timeout - call with int     */


/* this is for i2c-dev.c  */
#define I2C_PERIPHERAL_7BITS_ADDRESS  0x0703  /* Change slave address      */
        /* Attn.: Slave address is 7 or 10 bits */
#define I2C_PERIPHERAL_7BITS_ADDRESS_FORCE  0x0706  /* Change slave address   */
        /* Attn.: Slave address is 7 or 10 bits */
        /* This changes the address, even if it */
        /* is already taken!      */
#define I2C_TENBIT  0x0704  /* 0 for 7 bit addrs, != 0 for 10 bit  */

#define I2C_FUNCS  0x0705  /* Get the adapter functionality */
#define I2C_RDWR  0x0707  /* Combined R/W transfer (one stop only)*/
#define I2C_PEC    0x0708  /* != 0 for SMBus PEC                   */
#if 0
#define I2C_ACK_TEST  0x0710  /* See if a slave is at a specific address */
#endif

#define I2C_SMBUS  0x0720  /* SMBus-level access */

#endif /* DRIVERS_LINUX_IIC_IIC_TYPES_HPP */