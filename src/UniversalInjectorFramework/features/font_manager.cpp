#include "pch.h"
#include "font_manager.h"

#include "config.h"
#include "encoding.h"
#include "hooks.h"
#include "injector.h"
#include "utils.h"

#pragma region EnumFontFamiliesEx

struct enum_fonts_info
{
	FONTENUMPROCA fontEnumProcA;
	FONTENUMPROCW fontEnumProcW;
	LPARAM originalLParam;
	int totalCount;
	int enabledCount;

	const uif::features::font_manager* fontManager;
	const uif::features::enum_fonts_settings* settings;
};

static int CALLBACK enum_fonts_proc_a(const LOGFONTA* lplf, const TEXTMETRICA* lptm, DWORD FontType, LPARAM lParam) {
	auto* info = reinterpret_cast<enum_fonts_info*>(lParam);
	info->totalCount++;

	//const std::wstring fontName = encoding::shiftjis_to_utf16(lplf->lfFaceName);
	//std::cout << *info->fontManager << " Font: ";
	//std::wcout << fontName << L'\n';

	if(info->settings->filterPitchAndFamily && !(info->settings->filterPitchAndFamilyValue & lplf->lfPitchAndFamily))
		return 1;

	info->enabledCount++;

	LOGFONTA lf = *lplf;

	if(info->settings->spoofCharSet)
		lf.lfCharSet = info->settings->spoofCharSetValue;

	return info->fontEnumProcA(&lf, lptm, FontType, info->originalLParam);
}

static int CALLBACK enum_fonts_proc_w(const LOGFONTW* lplf, const TEXTMETRICW* lptm, DWORD FontType, LPARAM lParam) {
	auto* info = reinterpret_cast<enum_fonts_info*>(lParam);
	info->totalCount++;

	//std::cout << *info->fontManager << " Font: ";
	//std::wcout << lplf->lfFaceName << L'\n';

	if(info->settings->filterPitchAndFamily && !(info->settings->filterPitchAndFamilyValue & lplf->lfPitchAndFamily))
		return 1;

	info->enabledCount++;

	LOGFONTW lf = *lplf;

	if(info->settings->spoofCharSet)
		lf.lfCharSet = info->settings->spoofCharSetValue;

	return info->fontEnumProcW(&lf, lptm, FontType, info->originalLParam);
}

int WINAPI EnumFontFamiliesExAHook(HDC hdc, LPLOGFONTA lpLogfont, FONTENUMPROCA lpProc, LPARAM lParam, DWORD dwFlags)
{
	const auto& fontManager = uif::injector::instance().feature<uif::features::font_manager>();

	enum_fonts_info info{};
	info.fontEnumProcA = lpProc;
	info.originalLParam = lParam;
	info.fontManager = &fontManager;
	info.settings = &fontManager.enum_fonts_settings;

	LOGFONTA logFont = *lpLogfont;
	if(info.settings->filterCharSet)
		logFont.lfCharSet = info.settings->filterCharSetValue;

	const int result = EnumFontFamiliesExA(hdc, &logFont, enum_fonts_proc_a, reinterpret_cast<LPARAM>(&info), dwFlags);
	std::cout << fontManager << " Enumerated " << info.totalCount << " fonts, " << info.enabledCount << " passed all filters\n";

	return result;
}

int WINAPI EnumFontFamiliesExWHook(HDC hdc, LPLOGFONTW lpLogfont, FONTENUMPROCW lpProc, LPARAM lParam, DWORD dwFlags)
{
	const auto& fontManager = uif::injector::instance().feature<uif::features::font_manager>();

	enum_fonts_info info{};
	info.fontEnumProcW = lpProc;
	info.originalLParam = lParam;
	info.fontManager = &fontManager;
	info.settings = &fontManager.enum_fonts_settings;

	LOGFONTW logFont = *lpLogfont;
	if(info.settings->filterCharSet)
		logFont.lfCharSet = info.settings->filterCharSetValue;

	const int result = EnumFontFamiliesExW(hdc, &logFont, enum_fonts_proc_w, reinterpret_cast<LPARAM>(&info), dwFlags);
	std::cout << fontManager << " Enumerated " << info.totalCount << " fonts, " << info.enabledCount << " passed all filters\n";

	return result;
}

#pragma endregion

#pragma region CreateFont

HFONT WINAPI CreateFontAHook(
	int    cHeight,
	int    cWidth,
	int    cEscapement,
	int    cOrientation,
	int    cWeight,
	DWORD  bItalic,
	DWORD  bUnderline,
	DWORD  bStrikeOut,
	DWORD  iCharSet,
	DWORD  iOutPrecision,
	DWORD  iClipPrecision,
	DWORD  iQuality,
	DWORD  iPitchAndFamily,
	LPCSTR pszFaceName
)
{
	const auto& fontManager = uif::injector::instance().feature<uif::features::font_manager>();

	if(fontManager.create_font_settings.overrideCharSet)
		iCharSet = fontManager.create_font_settings.overrideCharSetValue;

	if(fontManager.create_font_settings.overrideFace)
		pszFaceName = fontManager.create_font_settings.overrideFaceValueA.c_str();

	if (fontManager.create_font_settings.overrideHeight)
		cHeight = fontManager.create_font_settings.overrideHeightValue;
	if (fontManager.create_font_settings.overrideWidth)
		cWidth = fontManager.create_font_settings.overrideWidthValue;

	return CreateFontA(cHeight, cWidth, cEscapement, cOrientation, cWeight, bItalic, bUnderline, bStrikeOut,
		iCharSet, iOutPrecision, iClipPrecision, iQuality, iPitchAndFamily, pszFaceName);
}

HFONT WINAPI CreateFontWHook(
	int    cHeight,
	int    cWidth,
	int    cEscapement,
	int    cOrientation,
	int    cWeight,
	DWORD  bItalic,
	DWORD  bUnderline,
	DWORD  bStrikeOut,
	DWORD  iCharSet,
	DWORD  iOutPrecision,
	DWORD  iClipPrecision,
	DWORD  iQuality,
	DWORD  iPitchAndFamily,
	LPCWSTR pszFaceName
)
{
	const auto& fontManager = uif::injector::instance().feature<uif::features::font_manager>();

	if(fontManager.create_font_settings.overrideCharSet)
		iCharSet = fontManager.create_font_settings.overrideCharSetValue;

	if(fontManager.create_font_settings.overrideFace)
		pszFaceName = fontManager.create_font_settings.overrideFaceValueW.c_str();

	if (fontManager.create_font_settings.overrideHeight)
		cHeight = fontManager.create_font_settings.overrideHeightValue;
	if (fontManager.create_font_settings.overrideWidth)
		cWidth = fontManager.create_font_settings.overrideWidthValue;

	return CreateFontW(cHeight, cWidth, cEscapement, cOrientation, cWeight, bItalic, bUnderline, bStrikeOut,
		iCharSet, iOutPrecision, iClipPrecision, iQuality, iPitchAndFamily, pszFaceName);
}

#pragma endregion

#pragma region CreateFontIndirect

HFONT WINAPI CreateFontIndirectAHook(const LOGFONTA* lplf)
{
	const auto& fontManager = uif::injector::instance().feature<uif::features::font_manager>();

	LOGFONTA logFont = *lplf;

	if(fontManager.create_font_settings.overrideCharSet)
		logFont.lfCharSet = fontManager.create_font_settings.overrideCharSetValue;

	if(fontManager.create_font_settings.overrideFace)
		strcpy_s(logFont.lfFaceName, fontManager.create_font_settings.overrideFaceValueA.c_str());

	if (fontManager.create_font_settings.overrideHeight)
		logFont.lfHeight = fontManager.create_font_settings.overrideHeightValue;
	if (fontManager.create_font_settings.overrideWidth)
		logFont.lfWidth = fontManager.create_font_settings.overrideWidthValue;

	return CreateFontIndirectA(&logFont);
}

HFONT WINAPI CreateFontIndirectWHook(const LOGFONTW* lplf)
{
	const auto& fontManager = uif::injector::instance().feature<uif::features::font_manager>();

	LOGFONTW logFont = *lplf;

	if(fontManager.create_font_settings.overrideCharSet)
		logFont.lfCharSet = fontManager.create_font_settings.overrideCharSetValue;

	if(fontManager.create_font_settings.overrideFace)
		wcscpy_s(logFont.lfFaceName, fontManager.create_font_settings.overrideFaceValueW.c_str());

	if (fontManager.create_font_settings.overrideHeight)
		logFont.lfHeight = fontManager.create_font_settings.overrideHeightValue;
	if (fontManager.create_font_settings.overrideWidth)
		logFont.lfWidth = fontManager.create_font_settings.overrideWidthValue;

	return CreateFontIndirectW(&logFont);
}

#pragma endregion

#pragma region CreateFontIndirectEx

HFONT WINAPI CreateFontIndirectExAHook(const ENUMLOGFONTEXDVA* lplf)
{
	const auto& fontManager = uif::injector::instance().feature<uif::features::font_manager>();

	ENUMLOGFONTEXDVA logFont = *lplf;

	if(fontManager.create_font_settings.overrideCharSet)
		logFont.elfEnumLogfontEx.elfLogFont.lfCharSet = fontManager.create_font_settings.overrideCharSetValue;

	if(fontManager.create_font_settings.overrideFace)
		strcpy_s(logFont.elfEnumLogfontEx.elfLogFont.lfFaceName, fontManager.create_font_settings.overrideFaceValueA.c_str());

	if (fontManager.create_font_settings.overrideHeight)
		logFont.elfEnumLogfontEx.elfLogFont.lfHeight = fontManager.create_font_settings.overrideHeightValue;
	if (fontManager.create_font_settings.overrideWidth)
		logFont.elfEnumLogfontEx.elfLogFont.lfWidth = fontManager.create_font_settings.overrideWidthValue;

	return CreateFontIndirectExA(&logFont);
}

HFONT WINAPI CreateFontIndirectExWHook(const ENUMLOGFONTEXDVW* lplf)
{
	const auto& fontManager = uif::injector::instance().feature<uif::features::font_manager>();

	ENUMLOGFONTEXDVW logFont = *lplf;

	if(fontManager.create_font_settings.overrideCharSet)
		logFont.elfEnumLogfontEx.elfLogFont.lfCharSet = fontManager.create_font_settings.overrideCharSetValue;

	if(fontManager.create_font_settings.overrideFace)
		wcscpy_s(logFont.elfEnumLogfontEx.elfLogFont.lfFaceName, fontManager.create_font_settings.overrideFaceValueW.c_str());

	if (fontManager.create_font_settings.overrideHeight)
		logFont.elfEnumLogfontEx.elfLogFont.lfHeight = fontManager.create_font_settings.overrideHeightValue;
	if (fontManager.create_font_settings.overrideWidth)
		logFont.elfEnumLogfontEx.elfLogFont.lfWidth = fontManager.create_font_settings.overrideWidthValue;

	return CreateFontIndirectExW(&logFont);
}

#pragma endregion

void uif::features::font_manager::initialize()
{
	if(config().contains("resource_files"))
	{
		auto& resourceFiles = config()["resource_files"];

		if(resourceFiles.is_array()) {

			for(const auto& resourceFile : resourceFiles)
			{
				if(!resourceFile.is_string()) continue;

				std::string fileName;
				resourceFile.get_to(fileName);

				if(const int result = AddFontResourceExA(fileName.c_str(), FR_PRIVATE, nullptr))
				{
					std::cout << *this << " Imported " << result << " font(s) from " << fileName << '\n';
				}
				else
				{
					std::cout << *this << " Failed to load fonts from " << fileName << '\n';
				}
			}
		}
	}

	if(config().value("/spoof_enumeration/enable"_json_pointer, false))
	{
		spoof_enumeration = true;

		if(config().contains("/spoof_enumeration/filter_pitch_and_family"_json_pointer))
		{
			const auto& value = config()["/spoof_enumeration/filter_pitch_and_family"_json_pointer];
			if(value.is_number())
			{
				enum_fonts_settings.filterPitchAndFamily = true;
				enum_fonts_settings.filterPitchAndFamilyValue = value.get<BYTE>();
			}
		}

		if(config().contains("/spoof_enumeration/filter_charset"_json_pointer))
		{
			const auto& value = config()["/spoof_enumeration/filter_charset"_json_pointer];
			if(value.is_number())
			{
				enum_fonts_settings.filterCharSet = true;
				enum_fonts_settings.filterCharSetValue = value.get<BYTE>();
			}
		}

		if(config().contains("/spoof_enumeration/spoof_charset"_json_pointer))
		{
			const auto& value = config()["/spoof_enumeration/spoof_charset"_json_pointer];
			if(value.is_number())
			{
				enum_fonts_settings.spoofCharSet = true;
				enum_fonts_settings.spoofCharSetValue = value.get<BYTE>();
			}
		}

		hooks::hook_import(this, "EnumFontFamiliesExA", EnumFontFamiliesExAHook);
		hooks::hook_import(this, "EnumFontFamiliesExW", EnumFontFamiliesExWHook);
	}

	if(config().value("/spoof_creation/enable"_json_pointer, false))
	{
		spoof_creation = true;

		if(config().contains("/spoof_creation/override_charset"_json_pointer))
		{
			const auto& value = config()["/spoof_creation/override_charset"_json_pointer];
			if(value.is_number())
			{
				create_font_settings.overrideCharSet = true;
				create_font_settings.overrideCharSetValue = value.get<BYTE>();
			}
		}

		if(config().contains("/spoof_creation/override_face"_json_pointer))
		{
			const auto& value = config()["/spoof_creation/override_face"_json_pointer];
			if(value.is_string())
			{
				create_font_settings.overrideFace = true;
				value.get_to(create_font_settings.overrideFaceValueA);
				create_font_settings.overrideFaceValueW.assign(encoding::utf8_to_utf16(create_font_settings.overrideFaceValueA));
			}
		}

		if (config().contains("/spoof_creation/override_height"_json_pointer))
		{
			const auto& value = config()["/spoof_creation/override_height"_json_pointer];
			if (value.is_number())
			{
				create_font_settings.overrideHeight = true;
				create_font_settings.overrideHeightValue = value.get<INT>();
			}
		}

		if (config().contains("/spoof_creation/override_width"_json_pointer))
		{
			const auto& value = config()["/spoof_creation/override_width"_json_pointer];
			if (value.is_number())
			{
				create_font_settings.overrideWidth = true;
				create_font_settings.overrideWidthValue = value.get<INT>();
			}
		}

		hooks::hook_import(this, "CreateFontA", CreateFontAHook);
		hooks::hook_import(this, "CreateFontW", CreateFontWHook);
		hooks::hook_import(this, "CreateFontIndirectA", CreateFontIndirectAHook);
		hooks::hook_import(this, "CreateFontIndirectW", CreateFontIndirectWHook);
		hooks::hook_import(this, "CreateFontIndirectExA", CreateFontIndirectExAHook);
		hooks::hook_import(this, "CreateFontIndirectExW", CreateFontIndirectExWHook);
	}
}

void uif::features::font_manager::finalize()
{
	if(spoof_enumeration)
	{
		hooks::unhook_import(this, "EnumFontFamiliesExA", EnumFontFamiliesExAHook);
		hooks::unhook_import(this, "EnumFontFamiliesExW", EnumFontFamiliesExWHook);
	}

	if(spoof_creation)
	{
		hooks::unhook_import(this, "CreateFontA", CreateFontAHook);
		hooks::unhook_import(this, "CreateFontW", CreateFontWHook);
		hooks::unhook_import(this, "CreateFontIndirectA", CreateFontIndirectAHook);
		hooks::unhook_import(this, "CreateFontIndirectW", CreateFontIndirectWHook);
		hooks::unhook_import(this, "CreateFontIndirectExA", CreateFontIndirectExAHook);
		hooks::unhook_import(this, "CreateFontIndirectExW", CreateFontIndirectExWHook);
	}
}
