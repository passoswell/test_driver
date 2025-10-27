/**
 * @file error_codes.cpp
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2025-10-25
 *
 * @copyright Copyright (c) 2025
 *
 */

#include <cstring>

#include "drivers.hpp"

ErrorChain testFunction()
{
  ErrorCode status = GenericErrorCode::kCksum;
  return status;
}

AP_MAIN()
{
  // Creating a DIO error variable with a success code
  ErrorCode error1(GenericErrorCode::kSuccess, DioErrorCategory::getCategory());
  // Creating a DIO error variable with a timeout error code
  ErrorCode error2(GenericErrorCode::kTimedOut, DioErrorCategory::getCategory());
  // Creating a IIC error variable with a timeout error code
  ErrorCode error3(GenericErrorCode::kTimedOut, IicErrorCategory::getCategory());

  // Modifying the error code on IIC error variable to not implemented
  error3.setValue(GenericErrorCode::kNotImplemented);
  // Modifying the error message on the IIC error variable
  error3.setMessage("This message was modified on the example code");

  // Printing error information to the standard output
  std::printf("%s (%d): %s\r\n", error1.category().name().data(), error1.value(), error1.message().data());
  std::printf("%s (%d): %s\r\n", error2.category().name().data(), error2.value(), error2.message().data());
  std::printf("%s (%d): %s\r\n", error3.category().name().data(), error3.value(), error3.message().data());

  // Creating an error chain variable and attributing the return value of a function to it
  ErrorChain error_chain1 = testFunction();
  // Printing the error information in the first position of the error chain
  std::printf("\r\n%s (%d): %s\r\n", error_chain1.errorCode(0).category().name().data(), error_chain1.errorCode(0).value(), error_chain1.errorCode(0).message().data());

  // Creating an error chain with its first position initialized as a IIC error variable
  ErrorChain error_chain2(GenericErrorCode::kBufferSize, IicErrorCategory::getCategory());
  // Adding a DIO error variable to the error chain
  error_chain2.push(error1);
  // Adding another DIO error variable to the error chain
  error_chain2.push(error2);
  // Adding another IIC error variable to the error chain
  error_chain2 = error3;


  // Printing all error information stored in the error chain
  for(uint8_t index = 0; index < error_chain2.size(); index++)
  {
    std::printf("\r\n%s (%d): %s\r\n", error_chain2.errorCode(index).category().name().data(), error_chain2.errorCode(index).value(), error_chain2.errorCode(index).message().data());
    // Evaluating if the error variable contains an error or not through its bool operator
    if(error_chain2.errorCode(index))
    {
      std::printf(" Is not success\r\n");
    }else
    {
      std::printf(" Is success\r\n");
    }
  }

  AP_EXIT();
}