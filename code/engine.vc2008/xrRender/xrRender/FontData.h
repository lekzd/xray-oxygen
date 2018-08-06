/*
	Date of creation: 15.07.18
	Author: Lord
	Copyright (C): GSC, OxyDev Team
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
	string32 FontName;

public:

public:
	FontData();
	~FontData();

	void init(int Maxsize = 72, string32 FontName = "Arial");

private:
};
