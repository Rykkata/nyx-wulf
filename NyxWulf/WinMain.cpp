/*
*	This is the entry position to a windows application
By: Nicholas Stuter
*/

#include <Windows.h>

#define internal static 
#define local_persist static
#define global_variable static 

global_variable bool isRunning = true;

global_variable BITMAPINFO BitmapInfo;
global_variable void *BitmapMemory;
global_variable HBITMAP BitmapHandle;
global_variable HDC BitmapDeviceContext;
// Testing
internal void Win32ResizeDIBSection(int Width, int Height)
{
	if (BitmapHandle)
	{
		DeleteObject(BitmapHandle);
	}
	if (!BitmapDeviceContext)
	{
		BitmapDeviceContext = CreateCompatibleDC(0);
	}
	
	BitmapInfo.bmiHeader.biSize = sizeof(BitmapInfo.bmiHeader);
	BitmapInfo.bmiHeader.biWidth = Width;
	BitmapInfo.bmiHeader.biHeight = Height;
	BitmapInfo.bmiHeader.biPlanes = 1;
	BitmapInfo.bmiHeader.biBitCount = 32;
	BitmapInfo.bmiHeader.biCompression = BI_RGB;

	BitmapHandle = CreateDIBSection(BitmapDeviceContext, &BitmapInfo, DIB_RGB_COLORS, &BitmapMemory, 0, 0);

}

internal void Win32UpdateWindow(HDC DeviceContext,int  X, int Y, int Width, int Height)
{
	StretchDIBits(DeviceContext, X, Y, Width, Height, X, Y, Width, Height, NULL, NULL, DIB_RGB_COLORS, SRCCOPY);
}

LRESULT CALLBACK Win32MainWindowCallback(HWND Window, UINT Message, WPARAM WParam, LPARAM LParam)
{
	LRESULT result = 0;
	switch (Message)
	{
	case WM_SIZE:
	{
		RECT ClientRect;
		GetClientRect(Window, &ClientRect);
		int Width = ClientRect.right - ClientRect.left;
		int Height = ClientRect.bottom - ClientRect.top;
		Win32ResizeDIBSection(Width, Height);
	} break;

	case WM_DESTROY:
	{
		isRunning = false;
	} break;

	case WM_QUIT:
	{
		isRunning = false;
	} break;

	case WM_ACTIVATEAPP:
	{
		OutputDebugStringA("Activate Message\n");
	} break;

	case WM_PAINT:
	{
		PAINTSTRUCT Paint;
		HDC DeviceContext = BeginPaint(Window, &Paint);	
		LONG X = Paint.rcPaint.left;
		LONG Y = Paint.rcPaint.top;
		LONG Width = Paint.rcPaint.right - Paint.rcPaint.left;
		LONG Height = Paint.rcPaint.bottom - Paint.rcPaint.top;
		Win32UpdateWindow(DeviceContext, X, Y, Width, Height);

		EndPaint(Window, &Paint);
	} break;

	default:
	{
		OutputDebugStringA("Default Used, Unknown message type, using standard Win function\n");
		result = DefWindowProc(Window, Message, WParam, LParam);
	} break;
	}

	return result;
}

int CALLBACK WinMain(HINSTANCE Instance, HINSTANCE prevInstance, LPSTR CommandLine, int ShowCode)
{
	WNDCLASS WindowClass = {};

	WindowClass.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
	WindowClass.lpfnWndProc = Win32MainWindowCallback;
	WindowClass.hInstance = Instance;
	// TODO: Add icon
	// WindowClass.hIcon;
	WindowClass.lpszClassName = "WindowClass";

	if(RegisterClass(&WindowClass))
	{
		HWND Handle = CreateWindowEx(0, WindowClass.lpszClassName, "Nyx Wulf", WS_OVERLAPPEDWINDOW | WS_VISIBLE,
			CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, 0, 0, Instance, 0);

		if (Handle)
		{
			MSG message;
			while (isRunning)
			{
				while (PeekMessage(&message, Handle, 0, 0, PM_REMOVE))
				{

					TranslateMessage(&message);
					DispatchMessage(&message);
				}
			}
		}
		else
		{
			OutputDebugStringA("Create Window Failed\n");
		}
	}
	else
	{
		OutputDebugStringA("Register Window Failed\n");
	}

	return 0;
}