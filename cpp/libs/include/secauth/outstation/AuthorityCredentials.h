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
#ifndef SECAUTH_AUTHORITY_CREDENTIALS_H
#define SECAUTH_AUTHORITY_CREDENTIALS_H

#include "secauth/AuthorityKey.h"

namespace secauth
{

class AuthorityCredentials
{
public:

	AuthorityCredentials();

	void SetSCSN(uint32_t statusChangeSeqNumber);

	void Configure(uint32_t statusChangeSeqNumber, const AuthorityKey& key);

	bool GetSymmetricKey(uint32_t& statusChangeSeqNumber, openpal::RSlice& keyView) const;

	openpal::RSlice GetSymmetricKey() const;

private:

	uint32_t m_statusChangeSeqNum;
	AuthorityKey m_authorityKey;


};

}

#endif

