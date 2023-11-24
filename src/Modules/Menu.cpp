#include "Menu.h"
#include <Base/Base.h>
#include <Hooks/WindowHook.h>
#include "Draw.h"

#include <imgui/backends/imgui_impl_dx9.h>
#include <imgui/imgui_internal.h>
#include <Base/imgui_impl_win32.h>

CMenu::CMenu()
{
	RegisterEvent(EVENT_MENU);
	RegisterEvent(EVENT_POST_IMGUI_INPUT);
}

void CMenu::StartListening()
{
	Listen(EVENT_WINDOWPROC, [this] { return OnWindowProc(); });
	Listen(EVENT_IMGUI, [this]() { return OnImGui(); });
}

DWORD WINAPI UnhookThread(LPVOID)
{
	printf("Unhooking...\n");
	CBaseEvent::ShutdownAll();
	CBaseHook::UnHookAll();

	printf("Waiting for any hooked calls to end...\n");
	Sleep(0x1000);

	printf("FreeLibraryAndExitThread\n");
	FreeLibraryAndExitThread(Base::hInst, 0);

	return 0;
}

bool CMenu::AcceptMsg(HWND hWnd, UINT uMsg, LPARAM lParam, WPARAM wParam)
{
	switch (uMsg)
	{
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_INSERT:
		case VK_F11:
			SetOpen(!IsOpen());
			g_hk_window.SetInputEnabled(IsOpen());
			return true;
		}
	}

	ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam);
	bool hide_input = PushEvent(EVENT_POST_IMGUI_INPUT) & Return_NoOriginal;
	if (uMsg == WM_PAINT)
		return false; // Plz never eat this message. It will just infinitely loop.

	if (IsOpen())
	{
		switch (uMsg)
		{
		case WM_INPUT:
		case WM_MOUSEMOVE:
		case WM_MOUSEHOVER:
		case WM_NCMOUSEHOVER:
		case WM_NCMOUSEMOVE:
		case WM_MOUSELEAVE:
		case WM_NCMOUSELEAVE:
		case WM_LBUTTONDOWN: case WM_LBUTTONDBLCLK:
		case WM_RBUTTONDOWN: case WM_RBUTTONDBLCLK:
		case WM_MBUTTONDOWN: case WM_MBUTTONDBLCLK:
		case WM_XBUTTONDOWN: case WM_XBUTTONDBLCLK:
		case WM_MOUSEWHEEL:
		case WM_MOUSEHWHEEL:
		case WM_KEYDOWN:
		case WM_SYSKEYDOWN:
		case WM_CHAR:
			return true;
		case WM_KEYUP:
		case WM_SYSKEYUP:
		case WM_LBUTTONUP:
		case WM_RBUTTONUP:
		case WM_MBUTTONUP:
		case WM_XBUTTONUP:
			return false; // Prevent "stuck" input by allowing game to recieve key up
		}
	}
	return hide_input;
}

int CMenu::OnImGui()
{
	if (!IsOpen())
		return 0;

	ImGui::SetNextWindowSize(ImVec2(600, 400), ImGuiCond_Once);
	if (ImGui::Begin("Window"))
	{
		if (ImGui::Button("Eject"))
			CreateThread(0, 0, &UnhookThread, 0, 0, 0);

		PushEvent(EVENT_MENU);
	}
	ImGui::End();

	return 0;
}

int CMenu::OnWindowProc()
{
	WndProcArgs* ctx = g_hk_window.Context();
	if (AcceptMsg(ctx->hwnd, ctx->msg, ctx->lparam, ctx->wparam))
	{
		ctx->result = TRUE;
		return Return_NoOriginal | Return_Skip;
	}
	return 0;
}

bool CMenu::IsOpen() {
	return m_open;
}

void CMenu::SetOpen(bool Val) {
	m_open = Val;
}
