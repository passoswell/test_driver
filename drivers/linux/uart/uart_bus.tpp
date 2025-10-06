/**
 * @file uart_bus.tpp
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2025-07-13
 *
 * @copyright Copyright (c) 2025
 *
 */

#include "linux/uart/uart_bus.hpp"

#include <cstdio>
#include <cstring>

#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include "linux/utils/linux_io.hpp"

static speed_t convertSpeed(uint32_t speed);

/**
 * @brief Configure a list of parameters
 *
 * @param list List of parameter-value pairs
 * @param list_size Number of parameters on the list
 * @return ErrorCode
 */
template<UartHandle_t PORT_NUMBER>
ErrorCode UartBus<PORT_NUMBER>::configure(const SettingsList_t *list, uint8_t list_size)
{
  ErrorCode status(GenericErrorCode::kSuccess, UartErrorCategory::getCategory());
  static Mutex mutex;
  struct termios termios_structure;
  speed_t speed = B1152000;
  uint32_t stop_bits_count = 1;
  bool use_hw_flow_ctrl = false, result;
  uint32_t parity = PARITY_NONE;
  char port_name[100];
  int n_bytes;

  mutex.lock();
  if(m_is_configured)
  {
    mutex.unlock();
    return status;
  }


  if(list != nullptr && list_size != 0)
  {
    for(uint8_t i = 0; i < list_size; i++)
    {
      switch(list[i].parameter)
      {
        case COMM_PARAM_STOP_BITS:
          if(list[i].value == 2) { stop_bits_count = 2;}
          break;

        case COMM_PARAM_PARITY:
          if(list[i].value == PARITY_NONE) { parity = PARITY_NONE;}
          if(list[i].value == PARITY_EVEN) { parity = PARITY_EVEN;}
          if(list[i].value == PARITY_ODD) { parity = PARITY_ODD;}
          break;

        case COMM_PARAM_BAUD:
          speed = convertSpeed(list[i].value);
          break;

        case COMM_PARAM_CLOCK_SPEED:
          speed = convertSpeed(list[i].value);
          break;

        case COMM_PARAM_LINE_MODE:
          if((list[i].value & 1) == 1){parity = PARITY_EVEN;}
          else{parity = PARITY_NONE;}

          if((list[i].value & 2) == 2){stop_bits_count = 2;}
          else{stop_bits_count = 1;}

          if((list[i].value & 4) == 4){use_hw_flow_ctrl = true;}
          else{use_hw_flow_ctrl = false;}
          break;

        case COMM_WORK_ASYNC_RX:
          m_is_async_mode_rx = (bool) list[i].value;
          break;

        case COMM_WORK_ASYNC_TX:
          m_is_async_mode_tx = (bool) list[i].value;
          break;

        default:
          break;
      }
    }
  }

  if(m_is_async_mode_rx)
  {
    result = m_rx_thread_handle.create(UartBus::asyncReadThread, this, 0);
    if(!result)
    {
      status.setValue(GenericErrorCode::kFailed);
      status.setMessage("Failed to launch UartBus task for reception");
      mutex.unlock();
      return status;
    }
  }else
  {
    (void) m_rx_thread_handle.terminate();
  }

  if(m_is_async_mode_tx)
  {
    result = m_tx_thread_handle.create(UartBus::asyncWriteThread, this, 0);
    if(!result)
    {
      status.setValue(GenericErrorCode::kFailed);
      status.setMessage("Failed to launch UartBus task for transmission");
      mutex.unlock();
      return status;
    }
  }else
  {
    (void) m_tx_thread_handle.terminate();
  }

  // For more information on how to configure serial ports:
  // https://blog.mbedded.ninja/programming/operating-systems/linux/linux-serial-ports-using-c-cpp/
  n_bytes = std::snprintf(port_name, sizeof(port_name) - 1, "/dev/serial%03u", PORT_NUMBER);
  if(n_bytes < 0)
  {
    status.setValue(GenericErrorCode::kFailed);
    status.setMessage("Failed to find the file name for UartBus driver");
    mutex.unlock();
    return status;
  }
  m_fd = open(port_name, O_RDWR | O_NOCTTY);
  if (m_fd < 0)
  {
    status.setValue(GenericErrorCode::kFailed);
    status.setMessage("Failed to open file for UartBus driver");
    mutex.unlock();
    return status;
  }
  tcgetattr(m_fd, &termios_structure);
  cfmakeraw(&termios_structure);
  cfsetispeed(&termios_structure, speed);
  cfsetospeed(&termios_structure, speed);
  if(parity == PARITY_NONE)
  {
    termios_structure.c_cflag &= ~PARENB;
  }else
  {
    termios_structure.c_cflag |= PARENB;
    if(parity == PARITY_ODD)
    {
      termios_structure.c_cflag |= PARODD;
    }else
    {
      termios_structure.c_cflag &= ~PARODD;
    }
  }
  if(use_hw_flow_ctrl)
  {
    termios_structure.c_cflag |= CRTSCTS;
  }else
  {
    termios_structure.c_cflag &= ~CRTSCTS;
  }
  if(stop_bits_count == 1)
  {
    termios_structure.c_cflag &= ~CSTOPB;
  }else
  {
    termios_structure.c_cflag |= CSTOPB;
  }
  termios_structure.c_cflag &= ~CSIZE; // Clear all the size bits
  termios_structure.c_cflag |= CS8;    // 8 bits per byte (most common)
  termios_structure.c_cflag |= CREAD;  // Turn on READ
  termios_structure.c_cflag |= CLOCAL; // Ignore ctrl lines (CLOCAL = 1)

  // For more info on how to setup VMIN and VTIME,
  // please refer to http://www.unixwiz.net/techtips/termios-vmin-vtime.html
  if(m_is_async_mode_rx)
  {
    termios_structure.c_cc[VMIN] = 1;  // Wait for at least 1 byte
    termios_structure.c_cc[VTIME] = 1; // 10 ms intercharacter timeout
  }else
  {
    termios_structure.c_cc[VMIN] = 0;
    termios_structure.c_cc[VTIME] = 0;
  }
  tcflush(m_fd, TCIFLUSH);
  tcflush(m_fd, TCIFLUSH);
  tcsetattr(m_fd, TCSANOW, &termios_structure);

  mutex.unlock();
  return status;
}

/**
 * @brief Read data
 *
 * @param rs485_pin
 * @param data
 * @param timeout
 * @param cb_function
 * @param cb_arg
 * @return ErrorCode
 */
template<UartHandle_t PORT_NUMBER>
ErrorCode UartBus<PORT_NUMBER>::read(iDIO &rs485_pin, Buffer_t data, uint32_t timeout, iCallback &event_handler)
{
  ErrorCode status(GenericErrorCode::kSuccess, UartErrorCategory::getCategory());
  int bytes_read = 0;
  UartDataBundle_t data_bundle;

  status = checkInputs(data, timeout);
  if(status) { return status;}


  if(m_is_async_mode_rx)
  {
    data_bundle.rx.run = true;
    data_bundle.rx.data = data;
    data_bundle.rx.timeout = timeout;
    data_bundle.rx.rs485_pin = &rs485_pin;
    data_bundle.rx.event_handler = &event_handler;
    if(m_rx_thread_handle.setInputData(data_bundle, timeout))
    {
      status.setValue(GenericErrorCode::kSuccess);
    }else
    {
      status.setValue(GenericErrorCode::kBusy);
    }
  }else
  {
    if(m_rx_mutex.lock(timeout))
    {
      status = blockingRead(data, timeout, false);
      m_rx_mutex.unlock();
    }else
    {
      status.setValue(GenericErrorCode::kBusy);
    }
  }

  return status;
}

/**
 * @brief Write data
 *
 * @param rs485_pin
 * @param data
 * @param timeout
 * @param cb_function
 * @param cb_arg
 * @return ErrorCode
 */
template<UartHandle_t PORT_NUMBER>
ErrorCode UartBus<PORT_NUMBER>::write(iDIO &rs485_pin, Buffer_t data, uint32_t timeout, iCallback &event_handler)
{
  ErrorCode status(GenericErrorCode::kSuccess, UartErrorCategory::getCategory());
  int bytes_written, drain_status;
  UartDataBundle_t data_bundle;

  status = checkInputs(data, timeout);
  if(status) { return status;}


  if(m_is_async_mode_tx)
  {
    data_bundle.tx.run = true;
    data_bundle.tx.data = data;
    data_bundle.tx.timeout = timeout;
    data_bundle.tx.rs485_pin = &rs485_pin;
    data_bundle.tx.event_handler = &event_handler;
    if(m_tx_thread_handle.setInputData(data_bundle, 0))
    {
      status.setValue(GenericErrorCode::kSuccess);
    }else
    {
      status.setValue(GenericErrorCode::kBusy);
    }
  }else
  {
    if(m_tx_mutex.lock(timeout))
    {
      status = blockingWrite(data, timeout, true);
      m_tx_mutex.unlock();
    }else
    {
      status.setValue(GenericErrorCode::kBusy);
    }
  }

  return status;
}

/**
 * @brief Return the number of bytes available on the reception buffer
 *
 * @return Size_t
 */
template<UartHandle_t PORT_NUMBER>
uint32_t UartBus<PORT_NUMBER>::getBytesAvailable()
{
  return static_cast<uint32_t>(bytesAvailableSyscall(m_fd));
}

/**
 * @brief Get number of bytes successfully read in the last operation
 *
 * @return Size_t
 */
template<UartHandle_t PORT_NUMBER>
uint32_t UartBus<PORT_NUMBER>::getBytesRead()
{
  return m_bytes_read;
}

/**
 * @brief Constructor
 */
template<UartHandle_t PORT_NUMBER>
UartBus<PORT_NUMBER>::UartBus()
{
  m_fd = -1;
  m_is_async_mode_rx = false;
  m_is_async_mode_tx = false;
  m_is_configured = false;
  m_bytes_read = 0;
}

/**
 * @brief Destructor
 */
template<UartHandle_t PORT_NUMBER>
UartBus<PORT_NUMBER>::~UartBus()
{
  if(m_fd >= 0)
  {
    (void) close(m_fd);
  }
}

/**
 * @brief Read data synchronously
 *
 * @param data Buffer to store the data
 * @param byte_count Number of bytes to read
 * @param timeout Time to wait in milliseconds before returning an error
 * @param use_idle_line_detection
 * @return ErrorCode
 */
template<UartHandle_t PORT_NUMBER>
ErrorCode UartBus<PORT_NUMBER>::blockingRead(DrvBuffer_t data, uint32_t timeout, bool use_idle_line_detection)
{
  ErrorCode status(GenericErrorCode::kSuccess, UartErrorCategory::getCategory());
  int bytes_read = 0;

  if(use_idle_line_detection)
  {
    // timeout = 10;
    // bytes_read = readOnTimeoutSyscall(m_fd, data.data(), data.size_bytes(), timeout);
    bytes_read = readSyscall(m_fd, data.data(), data.size_bytes());
  }else
  {
    if (timeout == 0)
    {
      bytes_read = readSyscall(m_fd, data.data(), data.size_bytes());
    }
    else
    {
      bytes_read = readOnTimeoutSyscall3(m_fd, data.data(), data.size_bytes(), timeout);
    }
  }

  if(bytes_read < 0)
  {
    m_bytes_read = 0;
    status.setValue(GenericErrorCode::kFailed);
    status.setMessage(std::strerror(errno));
    return status;
  }else
  {
    m_bytes_read = bytes_read;
    if (bytes_read == 0)
    {
      status.setValue(GenericErrorCode::kTimedOut);
    }
  }

  return status;
}

/**
 * @brief Function called from a thread to read data asynchronously
 *
 * @param data_bundle Data needed to perform the operation
 * @param user_arg Parameter to be passed to a callback function
 * @return ErrorCode
 */
template<UartHandle_t PORT_NUMBER>
ErrorCode UartBus<PORT_NUMBER>::asyncReadThread(UartDataBundle_t data_bundle, void *user_arg)
{
  ErrorCode status(GenericErrorCode::kSuccess, UartErrorCategory::getCategory());
  UartBus *obj = static_cast<UartBus *>(user_arg);

  if(obj != nullptr)
  {
    status = obj->blockingRead(data_bundle.rx.data, data_bundle.rx.timeout, true);
    if (data_bundle.rx.event_handler != nullptr)
    {
      data_bundle.rx.event_handler->onEvent(status, EVENT_READ, {data_bundle.rx.data.data(), obj->m_bytes_read});
    }
  }
  return status;
}

/**
 * @brief Write data synchronously
 *
 * @param data Buffer where data is stored
 * @param byte_count Number of bytes to write
 * @param timeout Time to wait in milliseconds before returning an error
 * @return ErrorCode
 */
template<UartHandle_t PORT_NUMBER>
ErrorCode UartBus<PORT_NUMBER>::blockingWrite(DrvBuffer_t data, uint32_t timeout, bool wait_end_of_transmission)
{
  ErrorCode status(GenericErrorCode::kSuccess, UartErrorCategory::getCategory());
  int bytes_written, drain_status;
  bytes_written = writeSyscall(m_fd, data.data(), data.size_bytes());
  if(bytes_written != data.size_bytes())
  {
    status.setValue(GenericErrorCode::kFailed);
    status.setMessage(std::strerror(errno));
    return status;
  }
  if (wait_end_of_transmission)
  {
    drain_status = tcdrain(m_fd); // Wait until all data is written to the port
  }
  status.setValue(GenericErrorCode::kFailed);
  status.setMessage(std::strerror(errno));
  return status;
}

/**
 * @brief Function called from a thread to write data asynchronously
 *
 * @param data_bundle Data needed to perform the operation
 * @param user_arg Parameter to be passed to a callback function
 * @return ErrorCode
 */
template<UartHandle_t PORT_NUMBER>
ErrorCode UartBus<PORT_NUMBER>::asyncWriteThread(UartDataBundle_t data_bundle, void *user_arg)
{
  ErrorCode status(GenericErrorCode::kSuccess, UartErrorCategory::getCategory());
  UartBus *obj = static_cast<UartBus *>(user_arg);

  if(obj != nullptr)
  {
    status = obj->blockingWrite(data_bundle.tx.data, data_bundle.tx.timeout, true);
    if (data_bundle.tx.event_handler != nullptr)
    {
      data_bundle.tx.event_handler->onEvent(status, EVENT_WRITE, data_bundle.tx.data);
    }
  }
  return status;
}

/**
 * @brief Verify if the inputs are in ther expected range
 *
 * @param buffer Data buffer
 * @param size Number of bytes in the data buffer
 * @param timeout Operation timeout value
 * @param key Parameter
 * @return ErrorCode
 */
template<UartHandle_t PORT_NUMBER>
ErrorCode UartBus<PORT_NUMBER>::checkInputs(const DrvBuffer_t data, uint32_t timeout)
{
  ErrorCode status(GenericErrorCode::kSuccess, UartErrorCategory::getCategory());
  if(!m_is_configured) { status.setValue(GenericErrorCode::kNotConfigured);}
  if(data.data() == nullptr) { status.setValue(GenericErrorCode::kNullPointer);}
  if(m_fd < 0) { status.setValue(GenericErrorCode::kBadHandle);}
  if(data.size_bytes() == 0) { status.setValue(GenericErrorCode::kInvalidParameter);}
  return status;
}

/**
 * @brief Convert a speed value into something linux can understand
 *
 * @param speed
 * @return speed_t
 */
speed_t convertSpeed(uint32_t speed)
{
  speed_t output;

  switch(speed)
  {
    // POSIX compliant options
    case 0: output = B0; break;
    case 50: output = B50; break;
    case 75: output = B75; break;
    case 110: output = B110; break;
    case 134: output = B134; break;
    case 150: output = B150; break;
    case 200: output = B200; break;
    case 300: output = B300; break;
    case 600: output = B600; break;
    case 1200: output = B1200; break;
    case 1800: output = B1800; break;
    case 2400: output = B2400; break;
    case 4800: output = B4800; break;
    case 9600: output = B9600; break;
    case 19200: output = B19200; break;
    case 38400: output = B38400; break;
    case 57600: output = B57600; break;
    case 115200: output = B115200; break;
    case 230400: output = B230400; break;
    case 460800: output = B460800; break;

    // Extra output baud rates (not in POSIX)
    case 500000: output = B500000; break;
    case 576000: output = B576000; break;
    case 921600: output = B921600; break;
    case 1000000: output = B1000000; break;
    case 1152000: output = B1152000; break;
    case 1500000: output = B1500000; break;
    case 2000000: output = B2000000; break;
    case 2500000: output = B2500000; break;
    case 3000000: output = B3000000; break;
    case 3500000: output = B3500000; break;
    case 4000000: output = B4000000; break;
    default: output = B115200; break;
  }
  return output;
}
