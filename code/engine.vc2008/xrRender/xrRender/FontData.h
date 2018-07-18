/*
	Date of creation: 15.07.18
	Author: Lord
	Copyright: GSC, OxyDev Team
*/
#pragma once

class FontData
{
private:
	// No comments
	int Width, Height;
	int Thickness;
	int BorderWidth;

	// Color data of font
	int ColorR, ColorG, ColorB;

	//
	int glyphHeight;

	ref_shader FontShader;
	ref_geom FontGeom;

public:

public:
	FontData();
	~FontData();

private:
};
