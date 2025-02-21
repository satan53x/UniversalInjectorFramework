#include "pch.h"
#include "tunnel_decoder.h"

#include "ansi.h"
#include "config.h"
#include "encoding.h"
#include "hooks.h"
#include "injector.h"

using namespace uif::ansi;

BOOL __stdcall TextOutAHook(HDC hdc, int x, int y, LPCSTR lpString, int c)
{
	const auto& decoder = uif::injector::instance().feature<uif::features::tunnel_decoder>();
	const auto& decoded = encoding::decode_shiftjis_tunnel(lpString, c, decoder.mapping);

	return TextOutW(hdc, x, y, decoded.c_str(), static_cast<int>(decoded.length()));
}

int __stdcall MultiByteToWideCharHook(UINT CodePage, DWORD dwFlags, LPCCH lpMultiByteStr, int cbMultiByte, LPWSTR lpWideCharStr, int cchWideChar)
{
	if (CodePage != CP_ACP && CodePage != CP_THREAD_ACP && CodePage != 932)
		return MultiByteToWideChar(CodePage, dwFlags, lpMultiByteStr, cbMultiByte, lpWideCharStr, cchWideChar);

	const auto& decoder = uif::injector::instance().feature<uif::features::tunnel_decoder>();
	const auto& decoded = encoding::decode_shiftjis_tunnel(lpMultiByteStr, cbMultiByte, decoder.mapping);

	const int decodedCount = static_cast<int>(cbMultiByte < 0 ? decoded.length() + 1 : decoded.length());
	if (decodedCount > cchWideChar)
		return 0;

	wcscpy_s(lpWideCharStr, cchWideChar, decoded.c_str());
	return decodedCount;
}

int __stdcall GetTextExtentPoint32AHook(HDC hdc, LPCSTR lpString, int c, LPSIZE psizl)
{
	const auto& decoder = uif::injector::instance().feature<uif::features::tunnel_decoder>();
	const auto& decoded = encoding::decode_shiftjis_tunnel(lpString, c, decoder.mapping);

	return GetTextExtentPoint32W(hdc, decoded.c_str(), static_cast<int>(decoded.size()), psizl);
}

int __stdcall IsDBCSLeadByteExHook(UINT CodePage, BYTE TestChar)
{
	return IsDBCSLeadByteEx(932, TestChar);
}

int __stdcall IsDBCSLeadByteHook(BYTE TestChar)
{
	return IsDBCSLeadByteEx(932, TestChar);
}

void uif::features::tunnel_decoder::initialize()
{
	if (!config()["mapping"].is_string())
	{
		std::cout << *this << dark_red(" Error:") << " no mapping specified, disabling tunnel decoder\n";
		_enabled = false;
		return;
	}

	mapping = encoding::utf8_to_utf16(config()["mapping"].get<std::string>());
	std::cout << *this << " Loaded " << mapping.length() << " mapping characters\n";

	hooks::hook_import(this, "TextOutA", TextOutAHook);
	hooks::hook_import(this, "MultiByteToWideChar", MultiByteToWideCharHook);
	hooks::hook_import(this, "GetTextExtentPoint32A", GetTextExtentPoint32AHook);
	hooks::hook_import(this, "IsDBCSLeadByteEx", IsDBCSLeadByteExHook);
	hooks::hook_import(this, "IsDBCSLeadByte", IsDBCSLeadByteHook);
}

void uif::features::tunnel_decoder::finalize()
{
	hooks::unhook_import(this, "TextOutA", TextOutAHook);
	hooks::unhook_import(this, "MultiByteToWideChar", MultiByteToWideCharHook);
	hooks::unhook_import(this, "GetTextExtentPoint32A", GetTextExtentPoint32AHook);
	hooks::unhook_import(this, "IsDBCSLeadByteEx", IsDBCSLeadByteExHook);
	hooks::unhook_import(this, "IsDBCSLeadByte", IsDBCSLeadByteHook);
}