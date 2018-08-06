#include "stdafx.h"
#include "FontData.h"

#pragma comment(lib, "freetype.lib")
#include <ft2build.h>
#include FT_FREETYPE_H

FontData::FontData() : Width(0), Height(0), Thickness(0), BorderWidth(0), ColorR(0), ColorG(0), ColorB(0), glyphHeight(0)
{
}

FontData::~FontData()
{
}

void FontData::init(int MaxSize, string32 FontName)
{
	if (MaxSize < 0 || MaxSize > 72)
	{
		MaxSize = 72;
		Log("[FontManager] WARNING: Your current size of used font is large system maximum size value or less 0! MaxSize is set to 72");
	}

	string_path font_path;
	FT_Library lib;

	if (FT_Init_FreeType(&lib))
		R_ASSERT("Can't initalize Font Manager's Library!!!!");

	FS.update_path(font_path, "$game_data$", "fonts");

	FT_Face Font;
}