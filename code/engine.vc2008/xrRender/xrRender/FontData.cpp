#include "stdafx.h"
#include "FontData.h"

#pragma comment(lib, "freetype.lib")
#include <ft2build.h>
#include FT_FREETYPE_H
// needed
#include <filesystem>

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

	if (!xr_strlen(FontName) || xr_strcmp(FontName, "Arial"))
	{
		FontName = "Arial";
		Log("[Font Manager] WARNING: Name of loading Font is setting to \"Arial\"");
	}

	string_path font_path;
	FT_Library lib;

	FT_Error err = FT_Init_FreeType(&lib);

	if (err)
		R_ASSERT("[Font Manager] ERROR: Can't initialize font manager!");

	FS.update_path(font_path, "$game_data$", "fonts");

	FT_Face Font;
	err = FT_New_Face(lib, FontName, 0, &Font);
	if (err == FT_Err_Unknown_File_Format)
		R_ASSERT("[Font Manager] ERROR: Invalid type of Font!");
	else if (err)
		R_ASSERT("[Font Manager] ERROR: Invalid Font!");

	// Устанавливаем действительный размер
	FT_Set_Char_Size(Font, MaxSize << 6, MaxSize << 6, 96, 96);
}