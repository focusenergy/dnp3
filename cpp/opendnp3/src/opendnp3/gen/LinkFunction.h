//  _   _         ______    _ _ _   _             _ _ _
// | \ | |       |  ____|  | (_) | (_)           | | | |
// |  \| | ___   | |__   __| |_| |_ _ _ __   __ _| | | |
// | . ` |/ _ \  |  __| / _` | | __| | '_ \ / _` | | | |
// | |\  | (_) | | |___| (_| | | |_| | | | | (_| |_|_|_|
// |_| \_|\___/  |______\__,_|_|\__|_|_| |_|\__, (_|_|_)
//                                           __/ |
//                                          |___/
// Copyright 2013 Automatak LLC
// 
// Automatak LLC (www.automatak.com) licenses this file
// to you under the the Apache License Version 2.0 (the "License"):
// 
// http://www.apache.org/licenses/LICENSE-2.0.html

#ifndef __OPENDNP3_GENERATED_LINKFUNCTION_H_
#define __OPENDNP3_GENERATED_LINKFUNCTION_H_

#include <string>
#include <cstdint>

namespace opendnp3 {

enum class LinkFunction : uint8_t
{
  PRI_RESET_LINK_STATES = 0x40,
  PRI_TEST_LINK_STATES = 0x42,
  PRI_CONFIRMED_USER_DATA = 0x43,
  PRI_UNCONFIRMED_USER_DATA = 0x44,
  PRI_REQUEST_LINK_STATUS = 0x49,
  SEC_ACK = 0x0,
  SEC_NACK = 0x1,
  SEC_LINK_STATUS = 0xB,
  SEC_NOT_SUPPORTED = 0xF,
  INVALID = 0xFF
};

std::string LinkFunctionToString(LinkFunction arg);
uint8_t LinkFunctionToType(LinkFunction arg);
LinkFunction LinkFunctionFromType(uint8_t arg);

}

#endif
