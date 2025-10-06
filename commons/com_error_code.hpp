/**
 * @file com_error_code.hpp
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2025-09-10
 *
 * @copyright Copyright (c) 2025
 *
 */

#ifndef COMMONS_COM_ERROR_CODE_HPP
#define COMMONS_COM_ERROR_CODE_HPP

#include <array>
#include <cstdbool>
#include <cstdint>
#include <string_view>

/**
 * @brief Base class for error categories
 */
class ErrorCategory
{
public:

  virtual constexpr std::string_view name() const noexcept = 0;

  virtual constexpr std::string_view message(int error_value) const noexcept = 0;
};

// Forward declare trait (default = false)
template <typename T>
struct is_error_enum : std::false_type {};

/**
 * @brief Class for reporting errors
 */
class ErrorCode
{
public:
  // Default constructor
  constexpr ErrorCode()
      : m_value(0), m_category(nullptr), m_custom_message(nullptr) {}

  // Complete constructor
  constexpr ErrorCode(int value, const ErrorCategory &category, const char *custom_message = nullptr)
      : m_value(value), m_category(&category), m_custom_message(custom_message) {}

  // Templated constructor for automatic conversion of enums into ErrorCode
  template <typename Enum, typename = std::enable_if_t<is_error_enum<Enum>::value>>
  constexpr ErrorCode(Enum error_enum_item)
    : ErrorCode(makeErrorCode(error_enum_item)) {}

  // Complete templated constructor for automatic conversion of enums into ErrorCode
  template <typename Enum, typename = std::enable_if_t<is_error_enum<Enum>::value>>
  constexpr ErrorCode(Enum error_enum_item, const ErrorCategory &category, const char *custom_message = nullptr)
      : m_value(static_cast<int>(error_enum_item)), m_category(&category), m_custom_message(custom_message) {}

  // Get the error value
  constexpr int value() const noexcept
  {
    return m_value;
  }

  // Get the error category
  constexpr const ErrorCategory& category() const noexcept
  {
    return *m_category;
  }

  // True if there is an error (value != 0), false if success
  constexpr explicit operator bool() const noexcept
  {
    return m_value != 0;
  }

  // Get an error message
  std::string_view message() const noexcept
  {
    if (m_custom_message != nullptr)
    {
      return m_custom_message;
    }else if(m_category != nullptr)
    {
      return m_category->message(m_value);
    }else
    {
      return "No category";
    }
  }

  // Set a custom error message
  void setMessage(const std::string_view message)
  {
    m_custom_message = message.data();
  }

  // Set a category
  void setCategory(const ErrorCategory &category)
  {
    m_category = &category;
  }

  // Set an error value
  template <typename Enum, typename = std::enable_if_t<is_error_enum<Enum>::value>>
  void setValue(Enum error_enum_item)
  {
    m_value = static_cast<int>(error_enum_item);
  }

private:
  int m_value;
  const ErrorCategory *m_category;
  const char *m_custom_message;
};




/**
 * @brief List of common execution status values.
 */
enum class GenericErrorCode
{
/* NAME -------------------------- DESCRIPTION ------------------------------ */
  kSuccess                    = 0, /*!< Operation ended successfully          */

  kUnknown                       , /*!< An unknown error occurred             */
  kFailed                        , /*!< Requested operation failed            */
  kFault                         , /*!< Fault detected                        */
  kBusy                          , /*!< Device is busy                        */
  kEnabled                       , /*!< Device is enabled                     */
  kDisabled                      , /*!< Device is disabled                    */
  kTimedOut                      , /*!< Expected time expired                 */
  kOverflowed                    , /*!< Buffer overflow occurred              */
  kMath                          , /*!< Mathematical error                    */
  kCrc                           , /*!< CRC error detected                    */
  kCksum                         , /*!< Check sum error detected              */
  kParity                        , /*!< Parity error is detected              */
  kCommand                       , /*!< Invalid command or request            */
  kNullPointer                   , /*!< A unexpected null pointer was found   */
  kVariableCorrupted             , /*!< Variable(s) has unexpected value      */
  kListOrder                     , /*!< List items not following the enum     */
  kNotAvailable                  , /*!< Resource not available                */
  kNotImplemented                , /*!< Function or method not implemented    */
  kNotConfigured                 , /*!< Resource isn't properly configured    */
  kBufferSize                    , /*!< Buffer is smaller than data size      */
  kRegisterSize                  , /*!< Register is smaller than data size    */
  kNotFound                      , /*!< Resource requested not found          */
  kDeviceNotFound                , /*!< Device was no found                   */
  kWriteProtected                , /*!< Device is write protected             */
  kInvalidFileSystem             , /*!< No valid file system found            */
  kResourceDepleted              , /*!< Cannot allocate hardware or memory    */
  kNotLocked                     , /*!< Resource is empty                     */
  kAlreadyLocked                 , /*!< Resource is in use                    */
  kOwned                         , /*!< Resource is in use by the caller      */
  kNotOwned                      , /*!< Resource is in use by someone else    */
  kInterruptionDisabled          , /*!< Interruption system is disabled       */
  kDmaNotAvailable               , /*!< DMA's hardware not available          */
  kTransmission                  , /*!< Error during data transmission        */
  kReception                     , /*!< Error during data reception           */
  kTransfer                      , /*!< Error during data transfer            */
  kAcknowledged                  , /*!< A unexpected NACK was received        */
  kNotAcknowledged               , /*!< A unexpected ACK was received         */
  kBadHandle                     , /*!< Invalid handle to the resource        */
  kFull                          , /*!< Container is full                     */
  kEmpty                         , /*!< Container is empty                    */
  kMemFull                       , /*!< Memory is full                        */
  kMemEmpty                      , /*!< Memory is empty                       */
  kQueueFull                     , /*!< Queue is full                         */
  kQueueEmpty                    , /*!< Queue is empty                        */
  kAddressNotAcknowledged        , /*!< The address was not acknowledged      */
  kBusError                      , /*!< A dev on the bus needs fault recovery */
  kArbitrationLost               , /*!< Multi-master arbitration lost         */
  kOverrun                       , /*!< Data exceeds buffer storage capacity  */
  kUnderrun                      , /*!< Buffer empties before data arrival    */
  kFrame                         , /*!< Frame error                           */

  kInvalidParameter              , /*!< Invalid parameter                     */

  kInvalidId                     , /*!< Invalid ID                            */
  kInvalidValue                  , /*!< Invalid value                         */
  kInvalidSize                   , /*!< Invalid size                          */
  kInvalidRange                  , /*!< Invalid parameter's range             */
  kInvalidCommand                , /*!< Invalid command parameter             */
  kInvalidName                   , /*!< Name string entered is invalid        */
  kInvalidOffset                 , /*!< Invalid offset                        */

};


/**
 * @brief Custom error category for GenericErrorCode
 */
class GenericErrorCategory : public ErrorCategory
{
public:
  constexpr std::string_view name() const noexcept override { return "no_source"; }

  constexpr std::string_view message(int error_value) const noexcept override
  {
    switch (static_cast<GenericErrorCode>(error_value))
    {
      case GenericErrorCode::kSuccess:            return "Operation ended successfully";

      case GenericErrorCode::kUnknown:            return "An unknown error occurred";
      case GenericErrorCode::kFailed:             return "Requested operation failed";
      case GenericErrorCode::kFault:              return "Fault detected";
      case GenericErrorCode::kBusy:               return "Device is busy";
      case GenericErrorCode::kEnabled:            return "Device is enabled";
      case GenericErrorCode::kDisabled:           return "Device is disabled";
      case GenericErrorCode::kTimedOut:           return "Expected time expired";
      case GenericErrorCode::kOverflowed:         return "Buffer overflow occurred";
      case GenericErrorCode::kMath:               return "Mathematical error";
      case GenericErrorCode::kCrc:                return "CRC error detected";
      case GenericErrorCode::kCksum:              return "Checksum error detected";
      case GenericErrorCode::kParity:             return "Parity error detected";
      case GenericErrorCode::kCommand:            return "Invalid command or request";
      case GenericErrorCode::kNullPointer:        return "Unexpected null pointer found";
      case GenericErrorCode::kVariableCorrupted:  return "Variable has unexpected value";
      case GenericErrorCode::kListOrder:          return "List items out of order";
      case GenericErrorCode::kNotAvailable:       return "Resource not available";
      case GenericErrorCode::kNotImplemented:     return "Function not implemented";
      case GenericErrorCode::kNotConfigured:      return "Resource not configured";
      case GenericErrorCode::kBufferSize:         return "Buffer too small for data";
      case GenericErrorCode::kRegisterSize:       return "Register too small for data";
      case GenericErrorCode::kNotFound:           return "Resource not found";
      case GenericErrorCode::kDeviceNotFound:     return "Device not found";
      case GenericErrorCode::kWriteProtected:     return "Device is write protected";
      case GenericErrorCode::kInvalidFileSystem:  return "Invalid file system";
      case GenericErrorCode::kResourceDepleted:   return "Cannot allocate hardware/memory";
      case GenericErrorCode::kNotLocked:          return "Resource is not locked";
      case GenericErrorCode::kAlreadyLocked:      return "Resource already locked";
      case GenericErrorCode::kOwned:              return "Resource is owned by caller";
      case GenericErrorCode::kNotOwned:           return "Resource is owned by someone else";
      case GenericErrorCode::kInterruptionDisabled:return "Interrupt system disabled";
      case GenericErrorCode::kDmaNotAvailable:    return "DMA hardware not available";
      case GenericErrorCode::kTransmission:       return "Transmission error";
      case GenericErrorCode::kReception:          return "Reception error";
      case GenericErrorCode::kTransfer:           return "Transfer error";
      case GenericErrorCode::kAcknowledged:       return "Unexpected NACK received";
      case GenericErrorCode::kNotAcknowledged:    return "Unexpected ACK received";
      case GenericErrorCode::kBadHandle:          return "Invalid resource handle";
      case GenericErrorCode::kFull:               return "Container is full";
      case GenericErrorCode::kEmpty:              return "Container is empty";
      case GenericErrorCode::kMemFull:            return "Memory is full";
      case GenericErrorCode::kMemEmpty:           return "Memory is empty";
      case GenericErrorCode::kQueueFull:          return "Queue is full";
      case GenericErrorCode::kQueueEmpty:         return "Queue is empty";
      case GenericErrorCode::kAddressNotAcknowledged: return "The address was not acknowledged";
      case GenericErrorCode::kBusError:           return "Some device on the bus or the peripheral needs fault recovery";
      case GenericErrorCode::kArbitrationLost:    return "Multi-master arbitration lost";
      case GenericErrorCode::kOverrun:            return "New data arrived before old data was read from the hardware";
      case GenericErrorCode::kUnderrun:           return "Hardware is ready for new data, but no data is available for transmission";
      case GenericErrorCode::kFrame:              return "Frame error";

      case GenericErrorCode::kInvalidParameter:   return "Invalid parameter";
      case GenericErrorCode::kInvalidId:          return "Invalid ID";
      case GenericErrorCode::kInvalidValue:       return "Invalid value";
      case GenericErrorCode::kInvalidSize:        return "Invalid size";
      case GenericErrorCode::kInvalidRange:       return "Invalid range";
      case GenericErrorCode::kInvalidCommand:     return "Invalid command parameter";
      case GenericErrorCode::kInvalidName:        return "Invalid name string";
      case GenericErrorCode::kInvalidOffset:      return "Invalid offset";
      default:                                    return "Unrecognized error code";
    }
  }

  // Get an instance of the error category
  static inline const ErrorCategory &getCategory()
  {
    static GenericErrorCategory instance;
    return instance;
  }
};

// Instance of GenericErrorCategory
inline constexpr GenericErrorCategory generic_category{};

/**
 * @brief Function overload, convert enum class into an ErrorCode
 *
 * @param error_code A value from enum GenericErrorCode
 * @param category A custom error category, default is generic_category
 * @return ErrorCode
 */
constexpr ErrorCode makeErrorCode(GenericErrorCode error_code, const ErrorCategory& category = GenericErrorCategory::getCategory(), const char *custom_message = nullptr)
{
  return ErrorCode(static_cast<int>(error_code), category);
}

/**
 * @brief Specializing ErrorCode to use the specialized makeErrorCode's definition above
 */
template <>
struct is_error_enum<GenericErrorCode> : std::true_type {};



/**
 * @brief Wraps ErrorCode holding multiple layers of error context
 */
class ErrorChain
{
public:
  static constexpr int kMaxCHainQty = 4; // configurable

  // Default constructor
  ErrorChain() : m_size(0) {}

  // Constructor initializes the first error code
  ErrorChain(const ErrorCode &error_code) : m_size(0)
  {
    push(error_code);
  }

  // Constructor initializes the first error code
  template <typename Enum, typename = std::enable_if_t<is_error_enum<Enum>::value>>
  constexpr ErrorChain(Enum error_enum_item, const ErrorCategory &category, const char *custom_message = nullptr) : m_size(0)
  {
    push(error_enum_item, category, custom_message);
  }

  // Add an error code to the chain
  void push(const ErrorCode &error_code)
  {
    if (m_size < kMaxCHainQty)
    {
      m_chain[m_size] = error_code;
      m_size++;
    }
  }

  // Add an error code to the chain
  template <typename Enum, typename = std::enable_if_t<is_error_enum<Enum>::value>>
  void push(Enum error_enum_item, const ErrorCategory &category, const char *custom_message = nullptr)
  {
    if (m_size < kMaxCHainQty)
    {
      m_chain[m_size] = makeErrorCode(error_enum_item, category, custom_message);
      m_size++;
    }
  }

  // Get the number of error codes chained
  int size() const { return m_size;}

  // Get the error code saved on the chain index
  const ErrorCode &errorCode(int index) const
  {
    if(index < size())
    {
      return m_chain[index];
    }else
    {
      return m_chain[0];
    }
  }

  // Overload the '=' operator
  ErrorChain operator=(const ErrorCode &error_code)
  {
    this->push(error_code);
    return *this;
  }

private:
  std::array<ErrorCode, kMaxCHainQty> m_chain;
  int m_size;
};



#endif /* COMMONS_COM_ERROR_CODE_HPP */