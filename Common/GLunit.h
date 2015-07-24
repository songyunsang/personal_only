#pragma once

#include "unit.h"
#include <string>

using namespace std;

//typedef tagRect<float>	GLRect;
//typedef tagPoint<float>	GLPoint;

typedef tagRect<float>		GLRectF;
typedef tagPoint<float>		GLPointF;
typedef tagPoint<float>		GLSizeF;
typedef tagPoint<long>		GLPointL;
typedef tagPoint<long>		GLSizeL;


typedef struct
{
	float x, y, z;
	float r, g, b, a;
} GLColorVertex;

struct GLColor
{
	float r, g, b, a;
	GLColor() : r(0), g(0), b(0), a(0) {}
	GLColor(float _r, float _g, float _b, float _a)
		: r(_r), g(_g), b(_b), a(_a)	{}

	GLColor(COLORREF rgb)
		: r(GetRValue(rgb) / 255.0f), g(GetGValue(rgb) / 255.0f), b(GetBValue(rgb) / 255.0f), a(1)	{}

	GLColor &operator=(const GLColor &color)
	{
		r = color.r;
		g = color.g;
		b = color.b;
		a = color.a;

		return *this;
	}

	GLColor &operator=(COLORREF rgb)
	{
		r = GetRValue(rgb) / 255.0f;
		g = GetGValue(rgb) / 255.0f;
		b = GetBValue(rgb) / 255.0f;
		a = 1;

		return *this;
	}
};
