/*
 * iobufferTest.cpp
 *
 * Copyright (C) 2011 IBR, TU Braunschweig
 *
 * Written-by: Johannes Morgenroth <morgenroth@ibr.cs.tu-bs.de>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include "iobufferTest.h"
#include <ibrcommon/data/iobuffer.h>
#include <sstream>

CPPUNIT_TEST_SUITE_REGISTRATION(iobufferTest);

void iobufferTest::setUp()
{
}

void iobufferTest::tearDown()
{
}

void iobufferTest::basicTest()
{
	ibrcommon::iobuffer buf;
	std::istream is(&buf);
	std::ostream os(&buf);

	os << "Hallo Welt" << std::flush;
	buf.finalize();

	std::stringstream ss; ss << is.rdbuf();

	CPPUNIT_ASSERT_EQUAL(ss.str(), std::string("Hallo Welt"));
}
