#include "includes.h"

LPDIRECT3D9 d3d;
LPDIRECT3DDEVICE9 d3d_devise;

INT SCREEN_WIDTH;
INT SCREEN_HEIGHT;

RECT DESCTOP_RECT;

void Render() {
	d3d_devise->Clear(0, nullptr, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0, 0, 0, 0), 1, 0);
	d3d_devise->BeginScene();
	ESP(DESCTOP_RECT, d3d_devise);
	d3d_devise->EndScene();
	d3d_devise->Present(0, 0, 0, 0);
}

void CleanupDevice() {
	d3d_devise->Release();
	d3d->Release();
}

void InitD3D(HWND hWnd) {
	d3d = Direct3DCreate9(D3D_SDK_VERSION);
	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.Windowed = 1;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3d->CreateDevice(0,D3DDEVTYPE_HAL,hWnd,D3DCREATE_SOFTWARE_VERTEXPROCESSING,&d3dpp,&d3d_devise);
}

LRESULT CALLBACK WndProc(_In_ HWND hWnd, _In_ UINT message, _In_ WPARAM wParam, _In_ LPARAM lParam) {
	switch (message)
	{
		case WM_DESTROY: {
			PostQuitMessage(0);
			break;
		}
		default: {
			return DefWindowProc(hWnd, message, wParam, lParam);
			break;
		}
	}
	return 0;
}

int CALLBACK WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow) {
	WNDCLASSEX wc;
	const HWND desctop = GetDesktopWindow();
	if (desctop != nullptr) {
		GetWindowRect(desctop, &DESCTOP_RECT);
		SCREEN_WIDTH = DESCTOP_RECT.right - DESCTOP_RECT.left;
		SCREEN_HEIGHT = DESCTOP_RECT.bottom - DESCTOP_RECT.top;
	}
	else {
		ExitProcess(0);
	}
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = HBRUSH(RGB(0, 0, 0));
	wc.lpszMenuName = NULL;
	wc.lpszClassName = L"MyAppClass";
	wc.hIconSm = LoadIcon(wc.hInstance, IDI_APPLICATION);
	if (!RegisterClassEx(&wc)) {
		return EXIT_FAILURE;
	}
	else {
		HWND hWnd = CreateWindowEx(0, wc.lpszClassName, L"Byte++", WS_EX_TOPMOST | WS_POPUP, DESCTOP_RECT.left, DESCTOP_RECT.top, SCREEN_WIDTH, SCREEN_HEIGHT, NULL, NULL, wc.hInstance, NULL);
		SetWindowLong(hWnd, GWL_EXSTYLE, GetWindowLong(hWnd, GWL_EXSTYLE) | WS_EX_LAYERED | WS_EX_TRANSPARENT);
		SetLayeredWindowAttributes(hWnd,NULL,0,LWA_ALPHA);
		SetLayeredWindowAttributes(hWnd, RGB(0, 0, 0), 0, ULW_COLORKEY);
		if (!hWnd) {
			return EXIT_FAILURE;
		}
		else {
			ShowWindow(hWnd, nCmdShow);
			StartCsGo();
			InitD3D(hWnd);
			UpdateWindow(hWnd);
			MSG msg = {0};
			while (WM_QUIT != msg.message) {
				SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
				if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
				else
				{
					Render();
				}
			}
			CloseCs();
			CleanupDevice();
			return (int)msg.wParam;
		}
	}
}