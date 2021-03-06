/*
 * Licensed to Green Energy Corp (www.greenenergycorp.com) under one or
 * more contributor license agreements. See the NOTICE file distributed
 * with this work for additional information regarding copyright ownership.
 * Green Energy Corp licenses this file to you under the Apache License,
 * Version 2.0 (the "License"); you may not use this file except in
 * compliance with the License.  You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * This project was forked on 01/01/2013 by Automatak, LLC and modifications
 * may have been made to this file. Automatak, LLC licenses these modifications
 * to you under the terms of the License.
 */
#ifndef OPENPAL_ICRYPTOPROVIDER_H
#define OPENPAL_ICRYPTOPROVIDER_H

#include "ISecureRandom.h"
#include "IHMACAlgo.h"
#include "IKeyWrapAlgo.h"

namespace openpal
{
/**
* An abstract provider of cryptographic services. All function are assumed
* to  be thread-safe such that multiple threads can safely share a single
* instance of this class.
*
* The default implementation of this class is based on libcrypto (openssl).
*/
class ICryptoProvider : public ISecureRandom
{
public:

	virtual ~ICryptoProvider() {}

	/// --- Getters for HMAC algorithms ----

	virtual IHMACAlgo& GetSHA1HMAC() = 0;
	virtual IHMACAlgo& GetSHA256HMAC() = 0;

	/// --- Getters for key wrap algorithms ----

	virtual IKeyWrapAlgo& GetAESKeyWrap() = 0;
};

}

#endif

