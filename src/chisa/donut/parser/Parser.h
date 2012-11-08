/**
 * Chisa
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

#pragma once
#include <memory>
#include "../../Handler.h"
#include "../code/Code.h"

namespace chisa {
namespace donut {

class ParserImpl;
class CompilerImpl;
class Parser
{
private:
	ParserImpl* parserImpl;
	CompilerImpl* compilerImpl;
	explicit Parser(ParserImpl* impl);
public:
	~Parser();
	static std::shared_ptr<Parser> fromFile(const std::string& filename);
	static std::shared_ptr<Parser> fromString(const std::string& src, const std::string& filename="<ON MEMORY>", int line=0);
	static std::shared_ptr<Parser> fromStream(std::istream& stream_, const std::string& filename);
	unsigned int parseProgram(Handler<donut::Code> code);
};

}}