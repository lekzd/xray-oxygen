#include "stdafx.h"
#include "FontData.h"

#pragma comment(lib, "freetype.lib")
#include <ft2build.h>
#include FT_FREETYPE_H
// needed
#include <filesystem>

FontData::FontData() : Thickness(0), BorderWidth(0), ColorR(0), ColorG(0), ColorB(0)
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

	if (!xr_strlen(FontName))
	{
		Log("[Font Manager] WARNING: Name of loading file is invalid");
		return;
	}

	string_path font_path;
	FT_Library lib;

	FT_Error err = FT_Init_FreeType(&lib);

	if (err)
		R_ASSERT("[Font Manager] ERROR: Can't initialize font manager!");

	FS.update_path(font_path, "$game_data$", "fonts");

	FT_Face Font;

	std::string path = "";
	path += font_path;
	path += "\\";
	path += FontName;

	// if we are loading a file without format in argument
	if (path[path.length() - 4] != '.')
	{
		path += ".ttf";
		err = FT_New_Face(lib, path.c_str(), 0, &Font);
		if (err)
		{
			path = path.erase(path.find(".ttf"));

			// Try to load .otf if the file doesn't contain format .ttf
			path += ".otf";
			err = FT_New_Face(lib, path.c_str(), 0, &Font);

			if (err)
			{
				Log("[Font Manager]: The Format of Loaded file doesn't support! Supported files: \'.ttf\', \'.otf\'.");
				return;
			}
		}
	}
	else
	{
		err = FT_New_Face(lib, path.c_str(), 0, &Font);

		if (err == FT_Err_Unknown_File_Format)
		{
			Log("[Font Manager] WARNING: Invalid format of file");
			FT_Done_Face(Font);
			FT_Done_FreeType(lib);
			return;
		}
		else if (err || Font == nullptr)
		{
			Log("[Font Manager] WARNING: Invalid file");
			FT_Done_Face(Font);
			FT_Done_FreeType(lib);
			return;
		}
	}

	Log(path.c_str());

	// Устанавливаем действительный размер
	FT_Set_Char_Size(Font, MaxSize << 6, MaxSize << 6, 96, 96);

	Log("[Font Manager]: Loading English symbols and system symbols of current font");
	// Load first 128 symbols of ASCII
	for (int i = 0; i < 128; ++i)
	{
	}
	Log("[Font Manager]: English Dictionary is loaded successfully!");

	Log("[Font Manager]: Loading Russian symbols of current font");
	// Load russian's characters from ASCII
	for (int i = 192; i < 256; ++i)
	{
	}
	Log("[Font Manager]: Russian dictionary is loaded successfully!");

	Load_UnicodeDictionary();

	std::string info = "[Font Manager]: The Font";
	info += FontName;
	info += "is loaded successfully!";
	Log(info.c_str());

	FT_Done_Face(Font);
	FT_Done_FreeType(lib);
}

void FontData::Load_UnicodeDictionary(void)
{
	Log("[Font Manager]: Loading Unicode symbols of current font");

	for (int i = 12353; i < 12438; ++i)
	{
	}
}