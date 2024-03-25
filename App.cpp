#include "App.h"
#include "Scene/Sequence.h"
#include "Rendering/Renderer.h"
#include "Common/Random.h"
#include "Model.h"
#include <memory>
#include <string>
#include <cassert>
#include <iostream>
#include <vector>

#define TextureDimension 2048

using namespace DirectX;


Main* mMain;

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);


#if defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#if defined(_DEBUG)
int main()

#else
INT WINAPI WinMain(HINSTANCE , HINSTANCE, LPSTR, INT)
#endif

{
#if defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	
#endif

	mMain = new Main();
	
	if (mMain != NULL)
	{
		//ウィンドウの作成
		WNDCLASSEX  wc;
		ZeroMemory(&wc, sizeof(wc));
		wc.cbSize = sizeof(wc);
		wc.style = CS_HREDRAW | CS_VREDRAW;
		wc.lpfnWndProc = WndProc;
		wc.hInstance = GetModuleHandle(nullptr);
		wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground = (HBRUSH)GetStockObject(LTGRAY_BRUSH);
		wc.lpszClassName = (LPSTR)AppName;
		wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
		RegisterClassEx(&wc);

		mMain->mHWnd = CreateWindow((LPSTR)AppName, (LPSTR)AppName, WS_OVERLAPPEDWINDOW,
			0, 0, WindowWidth, WindowHeight, 0, 0,wc.hInstance, 0);
		if (!mMain->mHWnd)
		{
			return E_FAIL;
		}

		ShowWindow(mMain->mHWnd, SW_SHOW);
		UpdateWindow(mMain->mHWnd);
		if (FAILED(Renderer::CreateRenderObject(&mMain->mHWnd))) {
			assert(!"描画オブジェクト初期化失敗");
		}
		Sequence* sequence = new Sequence();
		// メッセージループ
		MSG msg = { 0 };
		ZeroMemory(&msg, sizeof(msg));
		while (msg.message != WM_QUIT)
		{
			
			if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			
			else {
				//FPS計算表示
				static DWORD time = 0;
				static int frame = 0;
				frame++;
				char str[50];
				sprintf(str, "fps=%d", frame);
				if (timeGetTime() - time > 1000)
				{
					time = timeGetTime();
					frame = 0;
					SetWindowTextA(mMain->mHWnd, str);
				}
				sequence->RunScene();
			}
			
		}
		delete sequence;
		

	}
	delete mMain;
	
	return 0;
}


//------------------------------
//  ウィンドウプロシージャー
//------------------------------
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return mMain->MsgProc(hWnd, uMsg, wParam, lParam);
}
LRESULT Main::MsgProc(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	switch (iMsg)
	{
	case WM_KEYDOWN:
		switch ((char)wParam)
		{
		case VK_ESCAPE:
			PostQuitMessage(0);
			break;
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}
	return DefWindowProc(hWnd, iMsg, wParam, lParam);
}