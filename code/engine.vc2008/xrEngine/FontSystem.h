/*
	Date of creation: 15.07.18
	Author: Lord 
	Description: Bla bla bla of Font Engine bla bla bla (I'm very bored to completely write this section, later) 
	Copyright: GSC, OxyDev Team. 
*/

#pragma once



/*
	@ -> Uses for auto initialize all fonts
*/
class FontSystem
{
private: 

public: 

public:
	FontSystem();
	~FontSystem();

	// @ Path to main config 
	void InitializeSystem(const string1024 path);

	void DestroySystem(void);

	// @ Only a local function for render and only for using debug information!!!
	void DrawString(const string128 FontName, Ivector2 pos, int Size);


	void OnRender(void);

private: 
};

