/**
 * @file drv_i2c.cpp
 * @author user (contact@email.com)
 * @brief Insert a brief description here
 * @date 2024-06-09
 *
 * @copyright Copyright (c) 2024
 *
 */


#if __has_include("em_i2c.h")
#if !__has_include("drivers/silabs/pit/drv_pit.hpp")
#error "File 'drivers/silabs/pit/drv_pit.hpp' not found"
#endif

#include "drivers/silabs/i2c/drv_i2c.hpp"

#include "drivers/silabs/pit/drv_pit.hpp"
#include "em_i2c.h"


Status_t convertErrorCode(I2C_TransferReturn_TypeDef code);


/**
 * @brief Constructor
 * @param port_handle Vendor specific handle
 */
DrvI2C::DrvI2C(void *port_handle)
{
  m_handle = port_handle;
  m_address = 1;
}

/**
 * @brief Destructor
 */
DrvI2C::~DrvI2C()
{
  // Nothing is done here
}

/**
 * @brief Read data synchronously
 * @param buffer Buffer to store the data
 * @param size Number of bytes to read
 * @param timeout Time to wait in milliseconds before returning an error
 * @return Status_t
 */
Status_t DrvI2C::read(uint8_t *buffer, uint32_t size, uint8_t address_8bits, uint32_t timeout)
{
  DrvPIT timer;
  I2C_TransferSeq_TypeDef i2c_sequence;
  I2C_TransferReturn_TypeDef returncode;
  I2C_TypeDef *handle = (I2C_TypeDef *) m_handle;

  if(handle == nullptr || buffer == nullptr) return STATUS_DRV_NULL_POINTER;
  if(size == 0) return STATUS_DRV_ERR_PARAM_SIZE;
  if(!m_is_read_done) return STATUS_DRV_ERR_BUSY;

  if(!lock(address_8bits).success) return STATUS_DRV_ERR_NOT_OWNED;

  // Initialize I2C transfer
  i2c_sequence.addr          = address_8bits;
  i2c_sequence.flags         = I2C_FLAG_READ;
  i2c_sequence.buf[0].data   = buffer;
  i2c_sequence.buf[0].len    = size;
  i2c_sequence.buf[1].data   = nullptr;
  i2c_sequence.buf[1].len    = 0;

  m_is_read_done = false;
  m_is_operation_done = false;
  returncode = I2C_TransferInit(handle, &i2c_sequence);

  // Send data
  timer.write(timeout);
  while (returncode == i2cTransferInProgress)
  {
    if(timer.read())
    {
      handle->CMD = I2C_CMD_ABORT;
      break;
    }
    returncode = I2C_Transfer(handle);
  }

  (void) unlock(address_8bits); // TODO: Test for the error returned

  m_is_read_done = true;
  m_is_operation_done = true;
  return convertErrorCode(returncode);
}

/**
 * @brief Write data synchronously
 * @param buffer Buffer where data is stored
 * @param size Number of bytes to write
 * @param timeout Time to wait in milliseconds before returning an error
 * @return Status_t
 */
Status_t DrvI2C::write(uint8_t *buffer, uint32_t size, uint8_t address_8bits, uint32_t timeout)
{
  DrvPIT timer;
  I2C_TransferSeq_TypeDef i2c_sequence;
  I2C_TransferReturn_TypeDef returncode;
  I2C_TypeDef *handle = (I2C_TypeDef *) m_handle;

  if(handle == nullptr || buffer == nullptr) return STATUS_DRV_NULL_POINTER;
  if(size == 0) return STATUS_DRV_ERR_PARAM_SIZE;
  if(!m_is_write_done) return STATUS_DRV_ERR_BUSY;

  if(!lock(address_8bits).success) return STATUS_DRV_ERR_NOT_OWNED;

  i2c_sequence.addr          = address_8bits;
  i2c_sequence.flags         = I2C_FLAG_WRITE;
  i2c_sequence.buf[0].data   = buffer;
  i2c_sequence.buf[0].len    = size;
  i2c_sequence.buf[1].data   = nullptr;
  i2c_sequence.buf[1].len    = 0;

  m_is_write_done = false;
  m_is_operation_done = false;
  returncode = I2C_TransferInit(handle, &i2c_sequence);

  // Send data
  timer.write(timeout);
  while (returncode == i2cTransferInProgress)
  {
    if(timer.read())
    {
      handle->CMD = I2C_CMD_ABORT;
      break;
    }
    returncode = I2C_Transfer(handle);
  }

  (void) unlock(address_8bits); // TODO: Test for the error returned

  m_is_write_done = true;
  m_is_operation_done = true;
  return convertErrorCode(returncode);
}

/**
 * @brief Convert Silabs' error code to Status_t
 * @param code Silabs's error code
 * @return Status_t
 */
Status_t convertErrorCode(I2C_TransferReturn_TypeDef code)
{
  Status_t status = STATUS_DRV_NOT_IMPLEMENTED;
  switch(code)
  {
    case i2cTransferDone:
      status = STATUS_DRV_SUCCESS;
      break;
    case i2cTransferInProgress:
      status = STATUS_DRV_ERR_TIMEOUT;
      break;
    case i2cTransferNack:
      status.code = ERR_NOT_ACKNOWLEDGED;
      status.description = (char *)"NACK received during transfer";
      break;
    default:
     status = STATUS_DRV_UNKNOWN_ERROR;
     break;
  }
  return status;
}

#endif /* DRV_I2C_HPP_ */
