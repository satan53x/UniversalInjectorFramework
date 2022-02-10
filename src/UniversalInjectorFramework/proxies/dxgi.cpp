#include "pch.h"

#ifdef UIF_LIB_DXGI

struct dxgi_dll {
	HMODULE dll;
	FARPROC ApplyCompatResolutionQuirking;
	FARPROC CompatString;
	FARPROC CompatValue;
	FARPROC CreateDXGIFactory;
	FARPROC CreateDXGIFactory1;
	FARPROC CreateDXGIFactory2;
	FARPROC DXGID3D10CreateDevice;
	FARPROC DXGID3D10CreateLayeredDevice;
	FARPROC DXGID3D10GetLayeredDeviceSize;
	FARPROC DXGID3D10RegisterLayers;
	FARPROC DXGIDeclareAdapterRemovalSupport;
	FARPROC DXGIDumpJournal;
	FARPROC DXGIGetDebugInterface1;
	FARPROC DXGIReportAdapterConfiguration;
	FARPROC PIXBeginCapture;
	FARPROC PIXEndCapture;
	FARPROC PIXGetCaptureState;
	FARPROC SetAppCompatStringPointer;
	FARPROC UpdateHMDEmulationStatus;
} dxgi;

void lib_dxgi_ApplyCompatResolutionQuirking() { dxgi.ApplyCompatResolutionQuirking(); }
void lib_dxgi_CompatString() { dxgi.CompatString(); }
void lib_dxgi_CompatValue() { dxgi.CompatValue(); }
void lib_dxgi_CreateDXGIFactory() { dxgi.CreateDXGIFactory(); }
void lib_dxgi_CreateDXGIFactory1() { dxgi.CreateDXGIFactory1(); }
void lib_dxgi_CreateDXGIFactory2() { dxgi.CreateDXGIFactory2(); }
void lib_dxgi_DXGID3D10CreateDevice() { dxgi.DXGID3D10CreateDevice(); }
void lib_dxgi_DXGID3D10CreateLayeredDevice() { dxgi.DXGID3D10CreateLayeredDevice(); }
void lib_dxgi_DXGID3D10GetLayeredDeviceSize() { dxgi.DXGID3D10GetLayeredDeviceSize(); }
void lib_dxgi_DXGID3D10RegisterLayers() { dxgi.DXGID3D10RegisterLayers(); }
void lib_dxgi_DXGIDeclareAdapterRemovalSupport() { dxgi.DXGIDeclareAdapterRemovalSupport(); }
void lib_dxgi_DXGIDumpJournal() { dxgi.DXGIDumpJournal(); }
void lib_dxgi_DXGIGetDebugInterface1() { dxgi.DXGIGetDebugInterface1(); }
void lib_dxgi_DXGIReportAdapterConfiguration() { dxgi.DXGIReportAdapterConfiguration(); }
void lib_dxgi_PIXBeginCapture() { dxgi.PIXBeginCapture(); }
void lib_dxgi_PIXEndCapture() { dxgi.PIXEndCapture(); }
void lib_dxgi_PIXGetCaptureState() { dxgi.PIXGetCaptureState(); }
void lib_dxgi_SetAppCompatStringPointer() { dxgi.SetAppCompatStringPointer(); }
void lib_dxgi_UpdateHMDEmulationStatus() { dxgi.UpdateHMDEmulationStatus(); }

bool load_library_dxgi() {
	wchar_t path[MAX_PATH];
	GetSystemDirectory(path, MAX_PATH);
	lstrcat(path, L"\\dxgi.dll");
	dxgi.dll = LoadLibrary(path);
	if (reinterpret_cast<size_t>(dxgi.dll) == false)
	{
		MessageBox(0, L"Cannot load original dxgi.dll library", L"Universal Injector", MB_ICONERROR);
		ExitProcess(0);
	}
	dxgi.ApplyCompatResolutionQuirking = GetProcAddress(dxgi.dll, "ApplyCompatResolutionQuirking");
	dxgi.CompatString = GetProcAddress(dxgi.dll, "CompatString");
	dxgi.CompatValue = GetProcAddress(dxgi.dll, "CompatValue");
	dxgi.CreateDXGIFactory = GetProcAddress(dxgi.dll, "CreateDXGIFactory");
	dxgi.CreateDXGIFactory1 = GetProcAddress(dxgi.dll, "CreateDXGIFactory1");
	dxgi.CreateDXGIFactory2 = GetProcAddress(dxgi.dll, "CreateDXGIFactory2");
	dxgi.DXGID3D10CreateDevice = GetProcAddress(dxgi.dll, "DXGID3D10CreateDevice");
	dxgi.DXGID3D10CreateLayeredDevice = GetProcAddress(dxgi.dll, "DXGID3D10CreateLayeredDevice");
	dxgi.DXGID3D10GetLayeredDeviceSize = GetProcAddress(dxgi.dll, "DXGID3D10GetLayeredDeviceSize");
	dxgi.DXGID3D10RegisterLayers = GetProcAddress(dxgi.dll, "DXGID3D10RegisterLayers");
	dxgi.DXGIDeclareAdapterRemovalSupport = GetProcAddress(dxgi.dll, "DXGIDeclareAdapterRemovalSupport");
	dxgi.DXGIDumpJournal = GetProcAddress(dxgi.dll, "DXGIDumpJournal");
	dxgi.DXGIGetDebugInterface1 = GetProcAddress(dxgi.dll, "DXGIGetDebugInterface1");
	dxgi.DXGIReportAdapterConfiguration = GetProcAddress(dxgi.dll, "DXGIReportAdapterConfiguration");
	dxgi.PIXBeginCapture = GetProcAddress(dxgi.dll, "PIXBeginCapture");
	dxgi.PIXEndCapture = GetProcAddress(dxgi.dll, "PIXEndCapture");
	dxgi.PIXGetCaptureState = GetProcAddress(dxgi.dll, "PIXGetCaptureState");
	dxgi.SetAppCompatStringPointer = GetProcAddress(dxgi.dll, "SetAppCompatStringPointer");
	dxgi.UpdateHMDEmulationStatus = GetProcAddress(dxgi.dll, "UpdateHMDEmulationStatus");
	return true;
}

bool unload_library_dxgi() {
	FreeLibrary(dxgi.dll);
	dxgi.dll = nullptr;
	return true;
}

#else

bool load_library_dxgi() {
	return false;
}

bool unload_library_dxgi() {
	return false;
}

#endif