
#include <cassert>
#include <cmath>
#include <Windows.h>
#include "graphics_math.h"

typedef struct {
    HWND WindowHandle;
    BOOL IsRunning;
    HDC DeviceContext;
    UINT32 FrameBufferWidth;
    UINT32 FrameBufferHeight;
    UINT32 *FrameBufferPixels;
    float CurrAngle;
} global_state;

global_state GlobalState;

LRESULT WINAPI Win32WindowCallBack(
    HWND WindowHandle,
    UINT Message,
    WPARAM WParam,
    LPARAM LParam)
{
    LRESULT Result = {};

    switch (Message) {
        case WM_CLOSE:
        case WM_DESTROY:
            GlobalState.IsRunning = false;
            break;
        
        default:
            Result = DefWindowProc(WindowHandle, Message, WParam, LParam);
            break;
    }

    return Result;
}

int WINAPI WinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR pCmdLine,
    int nCmdShow)
{
    GlobalState.IsRunning = true;
    LARGE_INTEGER TimerFrequency = {};
    assert(QueryPerformanceFrequency(&TimerFrequency));

    {
        WNDCLASSA WindowClass = {};
        WindowClass.style = CS_VREDRAW | CS_HREDRAW;
        WindowClass.lpfnWndProc = Win32WindowCallBack;
        WindowClass.hInstance = hInstance;
        WindowClass.hCursor = LoadCursorA(NULL, IDC_ARROW);
        WindowClass.lpszClassName = "My Window";

        assert(RegisterClassA(&WindowClass));
        
        GlobalState.WindowHandle = CreateWindowExA(
            0,
            WindowClass.lpszClassName,
            "My Window",
            WS_OVERLAPPEDWINDOW | WS_VISIBLE,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            640,
            480,
            NULL,
            NULL,
            hInstance,
            NULL
        );

        if (!GlobalState.WindowHandle) {
            //InvalidCodePath;
        }

        GlobalState.DeviceContext = GetDC(GlobalState.WindowHandle);
    }

    {
        RECT ClientRect = {};

        assert(GetClientRect(GlobalState.WindowHandle, &ClientRect));

        GlobalState.FrameBufferWidth = ClientRect.right - ClientRect.left;
        GlobalState.FrameBufferHeight = ClientRect.bottom - ClientRect.top;
        UINT32 BuffSize = GlobalState.FrameBufferWidth * GlobalState.FrameBufferHeight;
        GlobalState.FrameBufferPixels = new UINT32[BuffSize];
    }

    LARGE_INTEGER BeginTime = {};
    LARGE_INTEGER EndTime = {};
    assert(QueryPerformanceCounter(&BeginTime));

    while (GlobalState.IsRunning) {
        assert(QueryPerformanceCounter(&EndTime));
        FLOAT FrameTime = (FLOAT)(EndTime.QuadPart - BeginTime.QuadPart) / (FLOAT)TimerFrequency.QuadPart;
        BeginTime = EndTime;

        MSG Message;

        while (PeekMessageA( &Message, GlobalState.WindowHandle, 0, 0, PM_REMOVE)) {
            switch (Message.message) {
            case WM_QUIT:
                GlobalState.IsRunning = false;
                break;
            
            default:
                TranslateMessage(&Message);
                DispatchMessageA(&Message);
                break;
            }
        }
        // fill all pixels with black color
        for (UINT32 Y = 0; Y < GlobalState.FrameBufferHeight; ++Y) {
            for (UINT32 X = 0; X < GlobalState.FrameBufferWidth; ++X) {
                UINT32 PixelId = Y * GlobalState.FrameBufferWidth + X;

                UINT8 Red = 0;
                UINT8 Green = 0;
                UINT8 Blue = 0;
                UINT8 Alpha = 255;

                GlobalState.FrameBufferPixels[PixelId] = ((UINT32)Alpha << 24) | ((UINT32)Red << 16) | 
                                                         ((UINT32)Green << 8) | (UINT32)Blue;
            }
        }

        Vec3 TrianglesColor[] = {
            Vec3::create(1.0f, 0.0f, 0.0f), // red
            Vec3::create(0.0f, 1.0f, 0.0f), // green
            Vec3::create(0.0f, 0.0f, 1.0f)  // blue
        };

        for (int32_t TriangleId = 9; TriangleId >= 0; --TriangleId) {
            float Depth = powf(2, TriangleId + 1);

            Vec3 Points[3] = {
                Vec3::create(-1.0f, -0.5f, Depth),
                Vec3::create(0.0f, 0.5f, Depth),
                Vec3::create(1.0f, -0.5f, Depth)
                
            };
            
            for (uint32_t PointId = 0; PointId  < sizeof(Points) / sizeof(Points[0]); ++PointId) {
                Vec3 ShiftedPoint = Points[PointId] + Vec3::create(cosf(GlobalState.CurrAngle), sinf(GlobalState.CurrAngle), 0.0f);
                Points[PointId] = ShiftedPoint;
            }

            DrawTriangle(
                Points,
                TrianglesColor,
                GlobalState.FrameBufferPixels,
                GlobalState.FrameBufferWidth,
                GlobalState.FrameBufferHeight
            );
        }

        GlobalState.CurrAngle += FrameTime;
        
        if (GlobalState.CurrAngle >= 2 * PI_32) {
            GlobalState.CurrAngle -= 2 * PI_32;
        }

        RECT ClientRect = {};

        assert(GetClientRect(GlobalState.WindowHandle, &ClientRect));

        UINT32 ClientWidth = ClientRect.right - ClientRect.left;
        UINT32 ClientHeight = ClientRect.bottom - ClientRect.top;

        BITMAPINFO BitmapInfo = {};
        
        BitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        BitmapInfo.bmiHeader.biWidth = GlobalState.FrameBufferWidth;
        BitmapInfo.bmiHeader.biHeight = GlobalState.FrameBufferHeight;
        BitmapInfo.bmiHeader.biPlanes = 1;
        BitmapInfo.bmiHeader.biBitCount = 32;
        BitmapInfo.bmiHeader.biCompression = BI_RGB;
        BitmapInfo.bmiHeader.biSizeImage = 0;

        assert(StretchDIBits(
            GlobalState.DeviceContext,
            0,
            0,
            ClientWidth,
            ClientHeight,
            0,
            0,
            GlobalState.FrameBufferWidth,
            GlobalState.FrameBufferHeight,
            GlobalState.FrameBufferPixels,
            &BitmapInfo,
            DIB_RGB_COLORS,
            SRCCOPY
        ));
    }
    return 0;
}