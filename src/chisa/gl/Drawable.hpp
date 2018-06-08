/* coding: utf-8 */
/**
 * Chisa
 *
 * Copyright 2012-2013, PSI
 */

#pragma once
#include <map>
#include <cmath>
#include "../geom/Area.hpp"
#include "Color.hpp"
#include <cinamo/Handler.h>
#include <cinamo/Logger.h>
#include "Font.hpp"

namespace chisa {
namespace gl {
class Canvas;
class Sprite;
class DrawableManager;

#ifdef M_PI
constexpr const double PI = M_PI;
#else
constexpr const double PI = 3.1415926535897932384626433832795028841971693993751;
#endif

class Drawable : public HandlerBody<Drawable> {
	HandlerW<DrawableManager> manager_;
protected:
	Drawable(HandlerW<DrawableManager> manager):manager_(manager){};
	HandlerW<DrawableManager>& manager() { return this->manager_; };
public:
	virtual ~Drawable() noexcept = default;
	inline bool onFree() noexcept { return false; };
	virtual float width();
	virtual float height();
	virtual geom::Box size() = 0;
	virtual void notifyDrawnSize(geom::Box const& size) {};
	virtual void draw(Canvas& canvas, geom::Point const& ptInRoot, geom::Area const& mask, const float depth=0.0f) = 0;
	virtual void draw(Canvas& canvas, geom::Point const& ptInRoot, const float depth=0.0f)
	{
		this->draw(canvas, ptInRoot, geom::Area(geom::ZERO, size()), depth);
	}
	virtual std::string toString() const = 0;
};

class NullDrawable : public Drawable {
private:
	geom::Box size_;
protected:
	NullDrawable( HandlerW<DrawableManager> manager, geom::Box const& size ):Drawable(manager), size_(size){};
public:
	virtual ~NullDrawable() noexcept = default;
	virtual geom::Box size() override { return this->size_; };
	virtual void draw(Canvas& canvas, geom::Point const& ptInRoot, geom::Area const& mask, const float depth=0.0f) override {};
	virtual std::string toString() const override;
	static Handler<Drawable> create( HandlerW<DrawableManager> manager, geom::Box const& size, std::string const& repl );
};

class ColorDrawable : public Drawable {
private:
	geom::Box size_;
	Color color_;
	ColorDrawable( HandlerW<DrawableManager> manager, const geom::Box size, gl::Color const& c);
public:
	static Handler<Drawable> create( HandlerW<DrawableManager> manager, geom::Box const& size, std::string const& repl );
	virtual ~ColorDrawable() noexcept = default;
public:
	Color color() const noexcept;
	virtual void notifyDrawnSize(geom::Box const& size) override final { if(!this->size_.isSpecified()){ this->size_=size; } };
	virtual geom::Box size() override { return this->size_; };
	virtual void draw(Canvas& canvas, geom::Point const& ptInRoot, geom::Area const& mask, const float depth=0.0f) override;
	virtual std::string toString() const override;
};

class ImageDrawable : public Drawable {
private:
	geom::Box size_;
	Handler<gl::Sprite> sprite_;
	std::string filename_;
protected:
	ImageDrawable( HandlerW<DrawableManager> manager, geom::Box const& size, std::string const& filename);
	virtual ~ImageDrawable() noexcept = default;
public:
	static Handler<Drawable> create(  HandlerW<DrawableManager> manager, geom::Box const& size, std::string const& repl );
	Handler<gl::Sprite> sprite();
	virtual geom::Box size() override;
	virtual void draw(Canvas& canvas, geom::Point const& ptInRoot, geom::Area const& mask, const float depth=0.0f) override;
	virtual std::string toString() const override;
};

class RepeatDrawable : public Drawable {
private:
	geom::Box size_;
	Handler<Drawable> child_;
	RepeatDrawable(HandlerW<DrawableManager> manager, geom::Box const& size, Handler<Drawable> child);
public:
	static Handler<Drawable> create( HandlerW<DrawableManager> manager, geom::Box const& size, std::string const& repl );
	virtual ~RepeatDrawable() noexcept = default;
	Handler<Drawable> child() const;
	virtual void notifyDrawnSize(geom::Box const& size) override final { if(!this->size_.isSpecified()){ this->size_=size; } };
	virtual geom::Box size() override;
	virtual void draw(Canvas& canvas, geom::Point const& ptInRoot, geom::Area const& mask, const float depth=0.0f) override;
	virtual std::string toString() const override;
};

class StretchDrawable : public Drawable {
private:
	geom::Box size_;
	Handler<Drawable> child_;
	StretchDrawable(HandlerW<DrawableManager> manager, const geom::Box size, Handler<Drawable> child);
public:
	static Handler<Drawable> create( HandlerW<DrawableManager> manager, geom::Box const& size, std::string const& repl );
	virtual ~StretchDrawable() noexcept = default;
	Handler<Drawable> child() const;
	virtual geom::Box size() override;
	virtual void notifyDrawnSize(geom::Box const& size) override final { if(!this->size_.isSpecified()){ this->size_=size; } };
	virtual void draw(Canvas& canvas, geom::Point const& ptInRoot, geom::Area const& mask, const float depth=0.0f) override;
	virtual std::string toString() const override;
};

class TextDrawable : public Drawable {
public:
	enum Style {
		Regular = 0,
		Bold = 1,
		Italic = 2
	};
	enum Decoration {
		None = 0,
		Underline = 1,
		Strike = 2
	};
	static const float DefaultFontSize;
private: /* テキストの内容 */
	std::string str_;
	bool vertical_;
	float fontSize_;
	Handler<Font> font_;
	Style style_;
	Decoration deco_;
	gl::Color color_;
	gl::Color backColor_;
private: /* レンダリングのための一時的なオブジェクト */
	Handler<gl::Sprite> sprite_;
	geom::IntBox tsize_;
	float font_ascent_;
	float font_descent_;
	float font_height_;
	geom::Box size_;
private:
	TextDrawable( HandlerW<DrawableManager> manager, std::string const& str, bool vertical, const float size, Handler<Font> font, TextDrawable::Style style, TextDrawable::Decoration deco, gl::Color const& color, gl::Color const& backColor);
	void revalidate();
	Handler<gl::Sprite> sprite();
public:
	static Handler<TextDrawable> create(HandlerW<DrawableManager> manager, std::string const& str, bool vertical, const float size, Handler<Font> font, TextDrawable::Style style, TextDrawable::Decoration deco, gl::Color const& color, gl::Color const& backColor);
	virtual ~TextDrawable() noexcept = default;
	virtual geom::Box size() override final;
	virtual float width() override final;
	virtual float height() override final;
	virtual void draw(Canvas& canvas, geom::Point const& ptInRoot, geom::Area const& mask, const float depth=0.0f) override;
	virtual void draw(Canvas& canvas, geom::Point const& ptInRoot, const float depth=0.0f) override;
	virtual std::string toString() const override;
public:
	inline std::string const& str() const noexcept { return this->str_; };
};

}}