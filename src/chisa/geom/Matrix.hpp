/* coding: utf-8 */
/**
 * Chisa
 *
 * Copyright 2012-2013, PSI
 */

#pragma once

#include "Decl.hpp"

namespace chisa {
namespace geom {

class Matrix{
private:
	float* mat_;
public:
	inline float& operator() (const size_t x, const size_t y) noexcept{
		return mat_[x<<2|y];
	}
	inline float const& operator()(const size_t x, const size_t y) const noexcept{
		return mat_[x<<2|y];
	}
	inline const float* raw() const noexcept
	{
		return mat_;
	}
public:
	Matrix()
	:mat_(new float[16])
	{
		for(size_t i=0;i<16;++i){
			mat_[i]=0.0f;
		}
		(*this)(0,0)=1.0f;
		(*this)(1,1)=1.0f;
		(*this)(2,2)=1.0f;
		(*this)(3,3)=1.0f;
	}
	Matrix(Matrix const& other)
	:mat_(new float[16])
	{
		memcpy(this->mat_, other.mat_, sizeof(other));
	}
	Matrix(
			float x00, float x01, float x02, float x03,
			float x10, float x11, float x12, float x13,
			float x20, float x21, float x22, float x23,
			float x30, float x31, float x32, float x33
			)
	:mat_(new float[16])
	{
		(*this)(0,0)=x00;
		(*this)(1,0)=x01;
		(*this)(2,0)=x02;
		(*this)(3,0)=x03;
		(*this)(0,1)=x10;
		(*this)(1,1)=x11;
		(*this)(2,1)=x12;
		(*this)(3,1)=x13;
		(*this)(0,2)=x20;
		(*this)(1,2)=x21;
		(*this)(2,2)=x22;
		(*this)(3,2)=x23;
		(*this)(0,3)=x30;
		(*this)(1,3)=x31;
		(*this)(2,3)=x32;
		(*this)(3,3)=x33;
	}
	Matrix(Matrix&& other) noexcept
	:mat_(other.mat_)
	{
		other.mat_=nullptr;
	}
	~Matrix() noexcept {
		if(this->mat_){
			delete [] this->mat_;
			this->mat_ = nullptr;
		}
	}
	Matrix& operator=(Matrix const& o)
	{
		memcpy(this->mat_, o.mat_, sizeof(float)*16);
		return *this;
	}
	Matrix& operator=(Matrix&& o) noexcept
	{
		if(this->mat_){
			delete [] this->mat_;
		}
		this->mat_ = o.mat_;
		o.mat_=nullptr;
		return *this;
	}
public:
	inline Matrix operator*(float const f) const noexcept
	{
		Matrix mat;
		for(size_t i=0;i<16;++i){
			mat.mat_[i]=this->mat_[i]*f;
		}
		return mat;
	}
	inline Matrix& operator*=(float const f) noexcept
	{
		for(size_t i=0;i<16;++i){
			this->mat_[i]*=f;
		}
		return *this;
	}
	inline Matrix operator/(float const f) const noexcept
	{
		return this->operator *(1/f);
	}
	inline Matrix& operator/=(float const f) noexcept
	{
		return this->operator *=(1/f);
	}
	inline Matrix operator*(Matrix const& other) const noexcept
	{
		Matrix mat;
		for(size_t x=0;x<4;++x){
			for(size_t y=0;y<4;++y){
				float ans = 0.0f;
				for(size_t j=0;j<4;++j){
					ans += (*this)(j,y)*other(x,j);
				}
				mat(x,y)=ans;
			}
		}
		return mat;
	}
	inline Matrix& operator*=(Matrix const& other) noexcept
	{
		(*this)=(*this)*other;
		return *this;
	}
};

}}