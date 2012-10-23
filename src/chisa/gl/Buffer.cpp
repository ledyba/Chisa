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

#include "./Buffer.h"

namespace chisa {
namespace gl {

Buffer::Buffer(const int width, const int height)
:data_(new unsigned char[width*height*4])
,width_(width)
,height_(height)
,stride_(width*4)
{
}

Buffer::~Buffer() noexcept
{
	delete this->data_;
}

}}
