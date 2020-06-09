#pragma once
#include "Hooks.h"
#include <d3d9.h>

typedef HRESULT(WINAPI* D3D9Present_t)(IDirect3DDevice9*, const RECT*, const RECT*, HWND, const RGNDATA*);
typedef HRESULT(WINAPI* D3D9Reset_t)(IDirect3DDevice9*, D3DPRESENT_PARAMETERS*);

class COverlayHook : public CBaseHook
{
public:
	COverlayHook();

	inline D3D9Present_t Present() const { return (D3D9Present_t)m_oldpresent; }
	inline D3D9Reset_t Reset() const { return (D3D9Reset_t)m_oldreset; }

private:
	void** m_pPresent, ** m_pReset;
	void* m_oldpresent, * m_oldreset;

	static HRESULT WINAPI Hooked_Reset(IDirect3DDevice9* thisptr, D3DPRESENT_PARAMETERS* Params);
	static HRESULT WINAPI Hooked_Present(
		IDirect3DDevice9* thisptr,
		const RECT* Src,
		const RECT* Dest,
		HWND Window,
		const RGNDATA* DirtyRegion);
};