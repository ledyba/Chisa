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
#include <string>
#include <deque>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <freetype/ftcache.h>

#include <cinamo/Logger.h>
#include <cinamo/Handler.h>
#include <cinamo/ClassUtil.h>

#define FLOAT_TO_26_6(d) ((FT_F26Dot6)((d) * 64.0))
#define FLOAT_FROM_26_6(t) ((float)(t) / 64.0)

#define FLOAT_TO_16_16(d) ((FT_F26Dot6)((d) * 65536.0))
#define FLOAT_FROM_16_16(t) ((float)(t) / 65536.0)

#define INT_TO_16_16(i) (i<<16)
#define INT_FROM_16_16_FLOOR(t) (t>>16)

namespace chisa {
using namespace cinamo;
namespace gl {
class Font;
class BitmapGlyph;

namespace internal {

class FontManager;
class FreeType : public HandlerBody<FreeType> {
private:
	FT_Library library_;
	FTC_Manager cache_;
	FTC_CMapCache cmap_;
	FTC_ImageCache image_;
public:
	FreeType();
	~FreeType() noexcept;
public:
	inline bool onFree() noexcept { return false; };
private:
	friend class FontManager;
	friend class gl::Font;
	FT_Library raw() const noexcept { return this->library_; };
	void removeFace(FT_Face face);
	unsigned int lookupGlyphIndex(Font& font, unsigned int ucs4);
	std::vector<Handler<BitmapGlyph> > lookupBitmap(Font& font, float size, std::vector<unsigned int> const& ucs4, float& ascent, float& descent, float& height);
};

class FontManager : public HandlerBody<FontManager> {
	DEFINE_MEMBER_REF(private, Logger, log);
	DEFINE_MEMBER_CONST(protected, std::string, fontdir);
private:
	static constexpr std::size_t MaxUnusedFonts = 100;
	Handler<internal::FreeType> freetype_;
	std::deque<Handler<Font> > unusedFonts_;
	Handler<Font> defaultFont_;
public:
	FontManager(Logger& log, std::string const& fontdir);
	Handler<Font> queryFont(std::string const& name = std::string());
private:
	~FontManager() noexcept;
private:
	Font* searchFont( std::string const& name );
	Font* seachDefaultFont();
public:
	inline bool onFree() noexcept { return false; };
	void backFont(Font* font);
};


}}}
