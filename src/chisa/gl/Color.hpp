/* coding: utf-8 */
/**
 * Chisa
 *
 * Copyright 2012-2013, PSI
 */

#pragma once
#include <cinamo/ClassUtil.h>
#include <cmath>
#include <utility>
#include <string>
#undef isnan
#undef isnanf

namespace chisa {
namespace gl {

typedef float color_t;

class Color {
	STACK_OBJECT(Color);
public:
	DEFINE_MEMBER_LITERAL(public, public, color_t, red);
	DEFINE_MEMBER_LITERAL(public, public, color_t, green);
	DEFINE_MEMBER_LITERAL(public, public, color_t, blue);
	DEFINE_MEMBER_LITERAL(public, public, color_t, alpha);
public:
	constexpr Color() noexcept:red_(0), green_(0), blue_(0), alpha_(0){};
	constexpr Color(color_t r, color_t g, color_t b, color_t a) noexcept:red_(r), green_(g), blue_(b), alpha_(a){};
	constexpr Color(Color const& other) = default;
	constexpr Color(Color&& other) = default;
	static Color fromString( std::string const& name );
	Color& operator=(Color const& other) = default;
	Color& operator=(Color&& other) = default;
	~Color() noexcept = default;
	std::string toString() const noexcept;
private:
	static constexpr color_t DELTA = 1.0f/255.0f;
public:
	constexpr Color multAlpha(float const& alpha) const noexcept{
		return Color(red_,green_, blue_, alpha_*alpha);
	}
	bool isInvalid() const noexcept{
		return
				std::isnan(red_) ||
				std::isnan(blue_) ||
				std::isnan(green_) ||
				std::isnan(alpha_);
	}
	bool operator==(Color const& o) const noexcept{
		return std::fabs(this->red_ - o.red_) < DELTA &&
				std::fabs(this->blue_ - o.blue_) < DELTA &&
				std::fabs(this->green_ - o.green_) < DELTA &&
				std::fabs(this->alpha_ - o.alpha_) < DELTA;
	}
	bool operator!=(Color const& o) const noexcept{
		return std::fabs(this->red_ - o.red_) >= DELTA ||
				std::fabs(this->blue_ - o.blue_) >= DELTA ||
				std::fabs(this->green_ - o.green_) >= DELTA ||
				std::fabs(this->alpha_ - o.alpha_) >= DELTA;
	}
	constexpr gl::Color darker(float const percent=0.25f) const noexcept {
		return Color(
				red_*(1-percent),
				green_*(1-percent),
				blue_*(1-percent),
				alpha_);
	}
	constexpr gl::Color lighter(float const percent=0.25f) const noexcept {
		return Color(
				red_+((1.0f-red_)*percent),
				green_+((1.0f-green_)*percent),
				blue_+((1.0f-blue_)*percent),
				alpha_);
	}
};

constexpr Color InvalidColor(NAN, NAN, NAN, NAN);
constexpr Color White(1,1,1,1);
constexpr Color Black(0,0,0,1);
constexpr Color Red(1,0,0,1);
constexpr Color DarkRed(.7f,0,0,1);
constexpr Color Blue(0,0,1,1);
constexpr Color DarkBlue(0,0,.7f,1);
constexpr Color LightBlue(0.3f,0.3f,1,1);
constexpr Color Green(0,1,0,1);
constexpr Color DarkGreen(0,.7f,0,1);
constexpr Color Yellow(1,1,0,1);
constexpr Color DarkYellow(0.85f,0.85f,0,1);
constexpr Color Gray(.5,.5,.5,1);
constexpr Color LightGray(.9,.9,.9,1);
constexpr Color DarkGray(.3,.3,.3,1);
constexpr Color Transparent(0.0f,0.0f,0.0f,0.0f);

const std::pair<std::string, gl::Color> COLOR_TABLE[] = {
		std::pair<std::string, gl::Color>("white", gl::White),
		std::pair<std::string, gl::Color>("black", gl::Black),
		std::pair<std::string, gl::Color>("red", gl::Red),
		std::pair<std::string, gl::Color>("darkred", gl::DarkRed),
		std::pair<std::string, gl::Color>("blue", gl::Blue),
		std::pair<std::string, gl::Color>("darkblue", gl::DarkBlue),
		std::pair<std::string, gl::Color>("lightblue", gl::LightBlue),
		std::pair<std::string, gl::Color>("green", gl::Green),
		std::pair<std::string, gl::Color>("darkgreen", gl::DarkGreen),
		std::pair<std::string, gl::Color>("yellow", gl::Yellow),
		std::pair<std::string, gl::Color>("darkyellow", gl::DarkYellow),
		std::pair<std::string, gl::Color>("gray", gl::Gray),
		std::pair<std::string, gl::Color>("lightgray", gl::LightGray),
		std::pair<std::string, gl::Color>("darkgray", gl::DarkGray),
		std::pair<std::string, gl::Color>("transparent", gl::Transparent)
};
}}