#pragma once
#define EXTMENU_BASEID				(WM_USER+0x1590)

const int ROI_FONTSIZE = 25;
const int TEXT_FONTSIZE = 15;
//const int G_ZOOMROLLNUM = 30;
const int G_ZOOMROLLNUM = 38;

//const float G_ZOOMROLLING[G_ZOOMROLLNUM] = {	-1.0f, 0.01f, 0.05f, 0.1f, 0.2f,
//												0.3f, 0.4f, 0.5f, 0.7f, 0.8f,
//												0.9f,
//												1.0f,
//												1.25f, 1.5f, 1.75f, 2.0f, 2.5f, 3.0f, 3.5f, 4.0f, 4.5f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f,12.5f, 15.0f, 20.0f};
const float G_ZOOMROLLING[G_ZOOMROLLNUM] = {	-1.0f,
												1/20.f, 1/15.f, 1/12.5f, 1/10.f, 1/9.f, 1/8.f, 1/7.f, 1/6.f, 1/5.f, 1/4.5f, 1/4.f, 1/3.5f, 1/3.f, 1/2.5f, 1/2.f, 1/1.75f, 1/1.5f, 1/1.25f,
												1.0f,
												1.25f, 1.5f, 1.75f, 2.0f, 2.5f, 3.0f, 3.5f, 4.0f, 4.5f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 12.5f, 15.0f, 20.0f};

class Overlay{
public:
	struct Shape
	{
		COLORREF crColor;
		int nPenStyle;
	};

	struct Rect : public Shape {
		float fLeft;		// px : closed bound
		float fTop;			// px : closed bound
		float fRight;		// px : closed bound
		float fBottom;		// px : closed bound
	};

	struct Line : public Shape {
		float fX1;
		float fY1;
		float fX2;
		float fY2;
	};

	struct Point : public Shape {
		float fX;
		float fY;
	};

	struct Ellipse : public Shape {
		float fLeft;		// px : closed bound
		float fTop;			// px : closed bound
		float fRight;		// px : closed bound
		float fBottom;		// px : closed bound
	};

	struct ArcDoughnut : public Shape {
		float fCenX;
		float fCenY;
		float fStartAngle;
		float fEndAngle;
		float fRadiusX;
		float fRadiusY;
		float fWidth;
	};

	struct ArcRing : public Shape {
		float fCenX;
		float fCenY;
		float fStartAngle;
		float fEndAngle;
		float fInnerLength;
		float fOuterLength;
	};
	
	struct Text : public Shape {
		float		fX;
		float		fY;
		LOGFONT*	pFont;
		CString		str;
	};

};

enum enumMouseControlMode
{
	M_MODE_NOTHING = 0,
	M_MODE_DRAG,
	M_MODE_NAVIGATION,
	M_MODE_DISPLAY_POSITION,
};

enum enumZoomFitMode
{
	ZOOMFIT_MODE_WIDTH = 1,
	ZOOMFIT_MODE_IMAGE = 2,
	ZOOMFIT_MODE_BOUND = 3,
};