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

#include <string>

#include "drivers.hpp"

ErrorChain testFunction()
{
  ErrorCode status = GenericErrorCode::kCksum;
  return status;
}

AP_MAIN()
{
  // Creating a DIO error variable with a success code
  ErrorCode ec1(GenericErrorCode::kSuccess, DioErrorCategory::getCategory());
  // Creating a DIO error variable with a timeout error code
  ErrorCode ec2(GenericErrorCode::kTimedOut, DioErrorCategory::getCategory());
  // Creating a IIC error variable with a timeout error code
  ErrorCode ec3(GenericErrorCode::kTimedOut, IicErrorCategory::getCategory());

  // Modifying the error code on IIC error variable to not implemented
  ec3.setValue(GenericErrorCode::kNotImplemented);
  // Modifying the error message on the IIC error variable
  ec3.setMessage("This message was modified on the example code");

  // Printing error information to the standard output
  std::printf("%s (%d): %s\r\n", ec1.category().name(), ec1.value(), ec1.message());
  std::printf("%s (%d): %s\r\n", ec2.category().name(), ec2.value(), ec2.message());
  std::printf("%s (%d): %s\r\n", ec3.category().name(), ec3.value(), ec3.message());

  // Creating an error chain variable and attributing the return value of a function to it
  ErrorChain ech2 = testFunction();
  // Printing the error information in the first position of the error chain
  std::printf("\r\n%s (%d): %s\r\n", ech2.errorCode(0).category().name(), ech2.errorCode(0).value(), ech2.errorCode(0).message());

  // Creating an error chain with its first position initialized as a IIC error variable
  ErrorChain ech(GenericErrorCode::kBufferSize, IicErrorCategory::getCategory());
  // Adding a DIO error variable to the error chain
  ech.push(ec1);
  // Adding another DIO error variable to the error chain
  ech.push(ec2);
  // Adding another IIC error variable to the error chain
  ech = ec3;


  // Printing all error information stored in the error chain
  for(uint8_t index = 0; index < ech.size(); index++)
  {
    std::printf("\r\n%s (%d): %s\r\n", ech.errorCode(index).category().name(), ech.errorCode(index).value(), ech.errorCode(index).message());
    // Evaluating if the error variable contains an error or not through its bool operator
    if(!ech.errorCode(index))
    {
      std::printf(" Is success\r\n");
    }else
    {
      std::printf(" Is not success\r\n");
    }
  }

  AP_EXIT();
}