/**
 * Saccubus
 * Copyright (C) 2012 psi
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef SACCUBUS_TESTCOMMON_H_
#define SACCUBUS_TESTCOMMON_H_

#include <memory>
using std::shared_ptr;
#include <gtest/gtest.h>
#include <iostream>
#include "../src/chisa/logging/Exception.h"
#include "../src/chisa/logging/Logger.h"
using namespace chisa::logging;

#define MATERIAL_DIR "__testfiles__"

#define NULL_STREAM (*((std::ostream*)0))
static Logger log_trace(std::cout, Logger::TRACE_);
static Logger log_err(std::cout, Logger::ERROR_);

#include <tinyxml2.h>
std::shared_ptr<tinyxml2::XMLDocument> parse(std::string const& str);

template <int... Args>
void static_debug(){
	static_assert(sizeof...(Args) < 0, "see type for compiler");
}

template <bool... Args>
void static_debug(){
	static_assert(sizeof...(Args) < 0, "see type for compiler");
}

template <typename... Args>
void static_debug(){
	static_assert(sizeof...(Args) < 0, "see type for compiler");
}

#endif /* SACCUBUS_TESTCOMMON_H_ */
