/**
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define CATCH_CONFIG_MAIN
#include <catch.hpp>
#include <iostream>

#include <yaml-cpp/yaml.h>

#include "opendnp3/app/ControlRelayOutputBlock.cpp"
#include "opendnp3/gen/ControlCode.cpp"
#include "opendnp3/app/AnalogOutput.h"

#include "../../../outstation/AsyncCommand.cpp"

#define SUITE(name) "TestOutstationApp - " name

using namespace std;

TEST_CASE(SUITE("AsyncCommand"))
{
	ControlRelayOutputBlock* crob = new ControlRelayOutputBlock(ControlCode::LATCH_ON, (uint8_t)1, (uint32_t) 2, (uint32_t) 3, CommandStatus::SUCCESS);
	AsyncCommand* ac = new AsyncCommand(crob, (char *) "testout", 123);
	REQUIRE(ac->AOInt16() == NULL);
}

TEST_CASE(SUITE("YAMLConfig"))
{
	YAML::Node config = YAML::LoadFile("cpp/tests/outstation/demo.yml");
	REQUIRE(config["master"]);
	REQUIRE(config["outstations"]);
	std::cout << config << std::endl;
}


