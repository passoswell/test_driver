/**
 * @file com_status.hpp
 * @author user (contact@email.com)
 * @brief Insert a brief description here
 * @date 2024-06-09
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef COM_STATUS_HPP_
#define COM_STATUS_HPP_


#include <stdint.h>
#include <stdbool.h>

/**
 * @brief Data type for reporting operation results and statuses
 */
typedef struct
{
  bool success;      /*!< Represent success if true, failure if false */
  uint8_t source;    /*!< The source of the code */
  uint16_t code;     /*!< The status code */
  char *description; /*!< Short description as a NULL terminated string */
}Status_t;

/**
 * @brief Suggestion for the source of the status code
 */
typedef enum
{
  SRC_INTERFACE = 0,  /*!< SRC_INTERFACE */
  SRC_DRIVER,         /*!< SRC_DRIVER */
  SRC_DEVICE,         /*!< SRC_DEVICE */
  SRC_MIDDLEWARE,     /*!< SRC_MIDDLEWARE */
  SRC_LIB,            /*!< SRC_LIB */
  SRC_TASK,           /*!< SRC_TASK */
  SRC_APP,            /*!< SRC_APP */
}StatusSouce_t;


/**
 * @brief List of common execution status values.
 */
typedef enum
{
/***** INFORMATION TYPE RETURN LIST *******************************************/
/* NAME --------------------------- VALUE ---- DESCRIPTION                    */
  OPERATION_OK              = 0x0000, /*!< Operation ended successfully       */
  OPERATION_IDLE                    , /*!< No operation running               */
  OPERATION_RUNNING                 , /*!< Operation still running            */
  OPERATION_BUSY                    , /*!< Busy with other caller's operation */
/******** TYPE DELIMITER ITEM *************************************************/
  RETURN_ERROR_VALUE        = 0x8000, /*!< Error codes follow this item       */
/******* ERROR TYPE RETURN LIST ***********************************************/
  ERR_UNKNOWN_ERROR                 , /*!< An unknown error occurred           */
  ERR_FAILED                        , /*!< Requested operation failed         */
  ERR_FAULT                         , /*!< Fault detected                     */
  ERR_BUSY                          , /*!< Device is busy                     */
  ERR_ENABLED                       , /*!< Device is enabled                  */
  ERR_DISABLED                      , /*!< Device is disabled                 */
  ERR_TIMEOUT                       , /*!< Expected time expired              */
  ERR_OVERFLOW                      , /*!< Buffer overflow occurred           */
  ERR_MATH                          , /*!< Mathematical error                 */
  ERR_CRC                           , /*!< CRC error detected                 */
  ERR_CKSUM                         , /*!< Check sum error detected           */
  ERR_PARITY                        , /*!< Parity error is detected           */
  ERR_COMMAND                       , /*!< Invalid command or request         */
  ERR_NULL_POINTER                  , /*!< A unexpected null pointer was found*/
  ERR_VARIABLE_CORRUPTED            , /*!< Variable(s) has unexpected value   */
  ERR_LIST_ORDER                    , /*!< List items not following the enum  */
  ERR_NOT_AVAILABLE                 , /*!< Resource not available             */
  ERR_NOT_IMPLEMENTED               , /*!< Function or method not implemented */
  ERR_BUFFER_SIZE                   , /*!< Buffer is smaller than data size   */
  ERR_REGISTER_SIZE                 , /*!< Register is smaller than data size */
  ERR_DEVICE_NOT_FOUND              , /*!< Device was no found                */
  ERR_WRITE_PROTECTED               , /*!< Device is write protected          */
  ERR_INVALID_FILE_SYSTEM           , /*!< No valid file system found         */
  ERR_RESOURCE_DEPLETED             , /*!< Cannot alocate hardware or memory  */
  ERR_NOT_LOCKED                    , /*!< Resource is empty                  */
  ERR_ALREADY_LOCKED                , /*!< Resource is in use                 */
  ERR_OWNED                         , /*!< Resource is in use by the caller   */
  ERR_NOT_OWNED                     , /*!< Resource is in use by someone else */
  ERR_INTERRUPTION_DISABLED         , /*!< Interruption system is disabled    */
  ERR_DMA_NOT_AVAILABLE             , /*!< DMA's hardware not available       */
  ERR_TRANSMISSION                  , /*!< Error during data transmission     */
  ERR_RECEPTION                     , /*!< Error during data reception        */
  ERR_TRANSFER                      , /*!< Error during data transfer         */
  ERR_ACKNOWLEDGED                  , /*!< A unexpected NACK was received     */
  ERR_NOT_ACKNOWLEDGED              , /*!< A unexpected ACK was received      */

  ERR_PARAM                 = 0x8080, /*!< Invalid parameter                  */

  ERR_PARAM_ID                      , /*!< Invalid ID                         */
  ERR_PARAM_VALUE                   , /*!< Invalid value                      */
  ERR_PARAM_SIZE                    , /*!< Invalid size                       */
  ERR_PARAM_RANGE                   , /*!< Invalid parameter's range          */
  ERR_PARAM_COMMAND                 , /*!< Invalid command parameter          */
  ERR_PARAM_NAME                    , /*!< Name string entered is invalid     */
  ERR_PARAM_OFFSET                  , /*!< Invalid offset                     */

} StatusCode_t;

#define SET_STATUS(var, success, source, status, message) \
{ \
  var = {success, source, status, message}; \
}

constexpr Status_t STATUS_DRV_SUCCESS = {true, SRC_DRIVER, OPERATION_OK, (char *)"Request processed successfully.\r\n"};
constexpr Status_t STATUS_DRV_IDLE = {true, SRC_DRIVER, OPERATION_IDLE, (char *)"No operation is being processed.\r\n"};
constexpr Status_t STATUS_DRV_RUNNING = {true, SRC_DRIVER, OPERATION_RUNNING, (char *)"The requested operation is on-going, keep calling this method to update the status.\r\n"};
constexpr Status_t STATUS_DRV_BUSY = {true, SRC_DRIVER, OPERATION_BUSY, (char *)"The resource is busy processing other request.\r\n"};

constexpr Status_t STATUS_DRV_UNKNOWN_ERROR = {false, SRC_DRIVER, ERR_UNKNOWN_ERROR, (char *)"An unknown or unverified error occurred.\r\n"};
constexpr Status_t STATUS_DRV_NOT_IMPLEMENTED = {false, SRC_DRIVER, ERR_NOT_IMPLEMENTED, (char *)"Method not implemented.\r\n"};
constexpr Status_t STATUS_DRV_NULL_POINTER = {false, SRC_DRIVER, ERR_NULL_POINTER, (char *)"A null pointer was detected.\r\n"};
constexpr Status_t STATUS_DRV_ERR_TIMEOUT = {false, SRC_DRIVER, ERR_TIMEOUT, (char *)"Operation timed out.\r\n"};
constexpr Status_t STATUS_DRV_ERR_BUSY = {false, SRC_DRIVER, ERR_BUSY, (char *)"Underlying driver was reported to be busy.\r\n"};
constexpr Status_t STATUS_DRV_ERR_NOT_OWNED = {false, SRC_INTERFACE, ERR_NOT_OWNED, (char *)"Resource is in use by someone else.\r\n"};
constexpr Status_t STATUS_DRV_ERR_PARAM = {false, SRC_DRIVER, ERR_PARAM, (char *)"An unspecified parameter was reported as illegal.\r\n"};
constexpr Status_t STATUS_DRV_ERR_PARAM_SIZE = {false, SRC_DRIVER, ERR_PARAM_SIZE, (char *)"The size parameter is invalid or out of range.\r\n"};

#endif /* COM_STATUS_HPP_ */
