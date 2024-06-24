#pragma once
#include "_feature_base.h"

#include <string>

namespace uif::features
{
	struct enum_fonts_settings
	{
		BYTE filterPitchAndFamilyValue;
		BYTE filterCharSetValue;
		BYTE spoofCharSetValue;

		bool filterPitchAndFamily;
		bool filterCharSet;
		bool spoofCharSet;
	};

	struct create_font_settings
	{
		std::string overrideFaceValueA;
		std::wstring overrideFaceValueW;
		BYTE overrideCharSetValue;
		int overrideHeightValue;
		int overrideWidthValue;

		bool overrideFace;
		bool overrideCharSet;
		bool overrideHeight;
		bool overrideWidth;
	};

	class font_manager : public feature_base
	{
	public:
		font_manager(uif::injector& injector) : feature_base(injector, "font_manager") { }
		void initialize() override;
		void finalize() override;
		
		bool spoof_enumeration = false;
		bool spoof_creation = false;

		enum_fonts_settings enum_fonts_settings{};
		create_font_settings create_font_settings{};
	};
}