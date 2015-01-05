/*
*	This is the entry position to a windows application
By: Nicholas Stuter
*/

#include <Windows.h>
#include <stdint.h>

#define internal static 
#define local_persist static
#define global_variable static 


global_variable bool isRunning = true;

global_variable BITMAPINFO BitmapInfo;
global_variable void *BitmapMemory;

global_variable int BitmapWidth;
global_variable int BitmapHeight;

global_variable int BytesPerPixel = 4;

internal void RenderGradiant(int XOffset, int YOffset)
{
	int Width = BitmapWidth;
	int Pitch = Width*BytesPerPixel;
	uint8_t *Row = (uint8_t*)BitmapMemory;

	for (int Y = 0; Y < BitmapHeight; ++Y)
	{
		uint8_t *Pixel = (uint8_t *)Row;
		for (int X = 0; X < BitmapWidth; ++X)
		{


			// Pixel = 0x00RRGGBB
			*Pixel = (uint8_t)(X + XOffset);
			++Pixel;
			// Pixel = 0x00RRGGBB
			*Pixel = (uint8_t)(Y + YOffset);
			++Pixel;
			// Pixel = 0x00RRGGBB
			*Pixel = 0;
			++Pixel;
			// Pixel = 0x00RRGGBB
			*Pixel = 0;
			++Pixel;

		}

		Row += Pitch;
	}
}

// Testing
internal void Win32ResizeDIBSection(int Width, int Height)
{
	if (BitmapMemory)
	{
		VirtualFree(BitmapMemory, 0, MEM_RELEASE);
	}
	
	BitmapWidth = Width;
	BitmapHeight = Height;

	BitmapInfo.bmiHeader.biSize = sizeof(BitmapInfo.bmiHeader);
	BitmapInfo.bmiHeader.biWidth = BitmapWidth;
	BitmapInfo.bmiHeader.biHeight = -BitmapHeight;
	BitmapInfo.bmiHeader.biPlanes = 1;
	BitmapInfo.bmiHeader.biBitCount = 32;
	BitmapInfo.bmiHeader.biCompression = BI_RGB;

	int BitmapMemorySize = (BitmapWidth*BitmapHeight)*BytesPerPixel;
	BitmapMemory = VirtualAlloc(0, BitmapMemorySize, MEM_COMMIT, PAGE_READWRITE);

}

internal void Win32UpdateWindow(HDC DeviceContext, RECT * WindowRect, int  X, int Y, int Width, int Height)
{
	int WindowWidth = WindowRect->right - WindowRect->left;
	int WindowHeight = WindowRect->bottom - WindowRect->top;

	if (StretchDIBits(DeviceContext, 0, 0, BitmapWidth, BitmapHeight, 0, 0, WindowWidth, WindowHeight, BitmapMemory, &BitmapInfo, DIB_RGB_COLORS, SRCCOPY) == 0)
	{
		OutputDebugStringA("StretchDIBits Failed\n");
	}
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
			int X = Paint.rcPaint.left;
			int Y = Paint.rcPaint.top;
			int Width = Paint.rcPaint.right - Paint.rcPaint.left;
			int Height = Paint.rcPaint.bottom - Paint.rcPaint.top;
			RECT ClientRect;
			GetClientRect(Window, &ClientRect);
			Win32UpdateWindow(DeviceContext, &ClientRect, X, Y, Width, Height);
			EndPaint(Window, &Paint);

		} break;

		default:
		{
			//OutputDebugStringA("Default Used, Unknown message type, using standard Win function\n");
			result = DefWindowProc(Window, Message, WParam, LParam);
		} break;
	}

	return result;
}

int CALLBACK WinMain(HINSTANCE Instance, HINSTANCE prevInstance, LPSTR CommandLine, int ShowCode)
{
	WNDCLASS WindowClass = {};

	WindowClass.lpfnWndProc = Win32MainWindowCallback;
	WindowClass.hInstance = Instance;
	// TODO: Add icon
	// WindowClass.hIcon;
	WindowClass.lpszClassName = "WindowClass";

	if(RegisterClass(&WindowClass))
	{
		HWND Handle = CreateWindowExA(0, WindowClass.lpszClassName, "Nyx Wulf", WS_OVERLAPPEDWINDOW | WS_VISIBLE,
			CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, 0, 0, Instance, 0);

		if (Handle)
		{
			MSG Message;
			int XOffset = 0;
			int YOffset = 0;
			while (isRunning)
			{
				
				while (PeekMessage(&Message, Handle, 0, 0, PM_REMOVE))
				{
					if (Message.message == WM_QUIT)
					{
						isRunning = false;
					}


					TranslateMessage(&Message);
					DispatchMessage(&Message);
				}

				RenderGradiant(XOffset, YOffset);
				HDC DeviceContext = GetDC(Handle);
				RECT ClientRect;
				GetClientRect(Handle, &ClientRect);
				int Width = ClientRect.right - ClientRect.left;
				int Height = ClientRect.bottom - ClientRect.top;
				Win32UpdateWindow(DeviceContext, &ClientRect, 0, 0, Width, Height);
				++XOffset;
				if (XOffset > 1378)
				{
					XOffset = 0;
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