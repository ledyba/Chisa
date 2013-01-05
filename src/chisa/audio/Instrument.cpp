/**
 * Chisa
 * Copyright (C) 2013 psi
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

#include "Instrument.h"

namespace chisa {

Instrument::Instrument(Handler<Quartet> const& quartet)
:quartet_(quartet)
{
}

Handler<Quartet> Instrument::quartet() const
{
	return this->quartet_.lock();
}

SoundSpec Instrument::spec() noexcept
{
	if(this->spec_.isInvalid()){
		this->spec_ = this->querySpec();
	}
	return this->spec_;
}
void Instrument::play(unsigned char *stream, int len)
{
	this->playImpl(stream, len);
}

}
