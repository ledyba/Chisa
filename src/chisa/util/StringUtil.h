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

#include <cstdarg>
#include <string>
#include <vector>

#ifndef SACCUBUS_STRINGUTIL_H_
#define SACCUBUS_STRINGUTIL_H_
/* FIXME: さきゅばすから取ってきた */

namespace chisa {
namespace util {

std::string format(const std::string& fmt, ...);
std::string formatv(const std::string& fmt, va_list args);
std::string decodePercent(const std::string& str);
void split(const std::string& str, const std::string& sep, std::vector<std::string>& list);
void split(const std::string& str, const std::string* sep, size_t n, std::vector<std::string>& list);
template <size_t N>
void split(const std::string& str, const std::string (&sep)[N], std::vector<std::string>& list)
{
	split(str, sep, N, list);
}
void splitSpace(const std::string& str, std::vector<std::string>& list);
void splitLine(const std::string& str, std::vector<std::string>& list);
bool startsWith(const std::string& target, const std::string& prefix);
bool endsWith(const std::string& target, const std::string& suffix);

}}
#endif /* SACCUBUS_STRINGUTIL_H_ */
