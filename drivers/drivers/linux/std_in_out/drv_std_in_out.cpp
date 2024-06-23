/**
 * @file drv_std_in_out.cpp
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2024-06-22
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "drivers/linux/std_in_out/drv_std_in_out.hpp"

/**
 * @brief Constructor
 */
DrvStdInOut::DrvStdInOut()
{
  m_in_file = fopen("/dev/fd/0", "r+");
  m_out_file = fopen("/dev/fd/1", "w+");
}

/**
 * @brief Destructor
 */
DrvStdInOut::~DrvStdInOut()
{
  if (m_in_file != nullptr)
  {
    fclose(m_in_file);
  }
  if (m_out_file != nullptr)
  {
    fclose(m_out_file);
  }
}

Status_t DrvStdInOut::read(uint8_t *buffer, uint32_t size, uint8_t key, uint32_t timeout)
{
  uint32_t counter = 0;
  uint8_t byte;
  (void) key;
  (void) timeout;

  if (buffer == nullptr || m_in_file == nullptr)
  {
    return STATUS_DRV_NULL_POINTER;
  }
  if(size == 0)
  {
    return STATUS_DRV_ERR_PARAM_SIZE;
  }

  while (bytesRead() < size)
  {
  }
  fread(buffer, size, 1, m_in_file);
  return STATUS_DRV_SUCCESS;
}

Status_t DrvStdInOut::write(uint8_t *buffer, uint32_t size, uint8_t key, uint32_t timeout)
{
  (void) key;
  (void) timeout;

  if(buffer == nullptr || m_out_file == nullptr)
  {
    return STATUS_DRV_NULL_POINTER;
  }
  if(size == 0)
  {
    return STATUS_DRV_ERR_PARAM_SIZE;
  }

  for (uint32_t i = 0; i < size; i++)
  {
    std::cout << buffer[i];
  }
  std::cout << std::endl;
  return STATUS_DRV_SUCCESS;
}

uint32_t DrvStdInOut::bytesRead()
{
  uint32_t prev = ftell(m_in_file);
  fseek(m_in_file, 0L, SEEK_END);
  uint32_t sz = ftell(m_in_file);
  fseek(m_in_file, prev, SEEK_SET); // go back to where we were
  return sz;
}
