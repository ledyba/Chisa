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

#include "DonutHelper.h"
#include <math.h>

namespace chisa {
namespace donut {

TEST(SeekTest, SeekTest)
{
	INIT_DONUT
	Handler<Source> src1 = Parser::fromString("test = 1;", "<MEM>", 0)->parseProgram();
	Handler<Object> result = machine.start( src1 );

	Handler<Source> src2 = Parser::fromString("test = 2;", "<MEM>", 0)->parseProgram();
	result = machine.start( src2 );
}

}}


