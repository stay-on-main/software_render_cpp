
#include <cassert>
#include <cmath>
#include <cfloat>
#include <iostream>
#include <Windows.h>
#include "graphics_math.h"

typedef struct {
    Vec3 Pos;
    Vec2 PrevMousePos;
    bool PrevMouseDown;
    float Yaw;
    float Picth;
} Camera;

typedef struct {
    HWND WindowHandle;
    BOOL IsRunning;
    HDC DeviceContext;
    UINT32 FrameBufferWidth;
    UINT32 FrameBufferHeight;
    UINT32 *FrameBufferPixels;
    float *DepthBuffer;
    float CurrAngle;
    bool KeyUp;
    bool KeyDown;
    bool KeyLeft;
    bool KeyRight;
    Camera Cam;
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
        GlobalState.DepthBuffer = new float[BuffSize];
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

            case WM_KEYUP:
            case WM_KEYDOWN: {
                uint32_t VkCode = Message.wParam;
                bool Pressed = (Message.lParam & (1 << 31)) == 0;

                switch (VkCode) {
                case 'W':
                    GlobalState.KeyUp = Pressed;
                    break;

                case 'S':
                    GlobalState.KeyDown = Pressed;
                    break;

                case 'A':
                    GlobalState.KeyLeft = Pressed;
                    break;

                case 'D':
                    GlobalState.KeyRight = Pressed;
                    break;
                }
                break;
            }
            
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

                GlobalState.DepthBuffer[PixelId] = FLT_MAX;
            }
        }
        
                RECT ClientRect = {};

        assert(GetClientRect(GlobalState.WindowHandle, &ClientRect));

        UINT32 ClientWidth = ClientRect.right - ClientRect.left;
        UINT32 ClientHeight = ClientRect.bottom - ClientRect.top;
        float AspectRatio = (float)ClientWidth / (float)ClientHeight;

        Mat4 CameraTransform = Mat4::Identity();
        {
            Vec2 CurrMousePos = Vec2::Create(0, 0);
            bool MouseDown = false;
            if (GetActiveWindow() == GlobalState.WindowHandle) {
                POINT Win32MousePos;
                assert(GetCursorPos(&Win32MousePos));
                assert(ScreenToClient(GlobalState.WindowHandle, &Win32MousePos));

                Win32MousePos.y = ClientRect.bottom - Win32MousePos.y;

                CurrMousePos.x = (float)Win32MousePos.x / (float)ClientWidth;
                CurrMousePos.y = (float)Win32MousePos.y / (float)ClientHeight;

                MouseDown = (GetKeyState(VK_LBUTTON) & 0x8000) != 0;
            }

            if (MouseDown) {
                
                if (!GlobalState.Cam.PrevMouseDown) {
                    GlobalState.Cam.PrevMousePos = CurrMousePos;
                }

                Vec2 MouseDelta = CurrMousePos - GlobalState.Cam.PrevMousePos;
                std::cout << "mouse delta (" << CurrMousePos.x << " " << CurrMousePos.y << ")" << std::endl;
                GlobalState.Cam.Picth += MouseDelta.y;
                GlobalState.Cam.Yaw += MouseDelta.x;

                GlobalState.Cam.PrevMousePos = CurrMousePos;
            }

            GlobalState.Cam.PrevMouseDown = MouseDown;

            Mat4 YawTransfrom = Mat4::RotationMatrix(0, GlobalState.Cam.Yaw, 0);
            Mat4 PitchTransform = Mat4::RotationMatrix(GlobalState.Cam.Picth, 0, 0);
            Mat4 CameraAxisTransfrom = PitchTransform * YawTransfrom;

            Vec3 Right = (CameraAxisTransfrom * Vec4::Create(1, 0, 0, 0)).ToVec3().Normalize();
            Vec3 Up = (CameraAxisTransfrom * Vec4::Create(0, 1, 0, 0)).ToVec3().Normalize();
            Vec3 LookAt = (CameraAxisTransfrom * Vec4::Create(0, 0, 1, 0)).ToVec3().Normalize();

            Mat4 CameraViewTransfrom = Mat4::Identity();

            CameraViewTransfrom.v[0].x = Right.x;
            CameraViewTransfrom.v[1].x = Right.y;
            CameraViewTransfrom.v[2].x = Right.z;

            CameraViewTransfrom.v[0].y = Up.x;
            CameraViewTransfrom.v[1].y = Up.y;
            CameraViewTransfrom.v[2].y = Up.z;

            CameraViewTransfrom.v[0].z = LookAt.x;
            CameraViewTransfrom.v[1].z = LookAt.y;
            CameraViewTransfrom.v[2].z = LookAt.z;

            if (GlobalState.KeyDown) {
                GlobalState.Cam.Pos -= LookAt * FrameTime;
            }

            if (GlobalState.KeyUp) {
                GlobalState.Cam.Pos += LookAt * FrameTime;
            }

            if (GlobalState.KeyLeft) {
                GlobalState.Cam.Pos -= Right * FrameTime;
            }

            if (GlobalState.KeyRight) {
                GlobalState.Cam.Pos += Right * FrameTime;
            }
        
            CameraTransform = CameraViewTransfrom * Mat4::TranslationMatrixFromVec3(-GlobalState.Cam.Pos);
        }

        GlobalState.CurrAngle += FrameTime;
        
        if (GlobalState.CurrAngle >= 2 * PI_32) {
            GlobalState.CurrAngle -= 2 * PI_32;
        }

        Vec3 ModelVertices[] = {
            // Front face
            Vec3::Create(-0.5f, -0.5f, -0.5f),
            Vec3::Create(-0.5f, 0.5f, -0.5f),
            Vec3::Create(0.5f, 0.5f, -0.5f),
            Vec3::Create(0.5f, -0.5f, -0.5f),
            // Back face
            Vec3::Create(-0.5f, -0.5f, 0.5f),
            Vec3::Create(-0.5f, 0.5f, 0.5f),
            Vec3::Create(0.5f, 0.5f, 0.5f),
            Vec3::Create(0.5f, -0.5f, 0.5f),
        };

        Vec3 ModelColors[] = {
            Vec3::Create(1, 0, 0),
            Vec3::Create(0, 1, 0),
            Vec3::Create(0, 0, 1),
            Vec3::Create(1, 0, 1),

            Vec3::Create(1, 1, 0),
            Vec3::Create(0, 1, 1),
            Vec3::Create(1, 0, 0),
            Vec3::Create(1, 1, 1),
        };

        uint32_t ModelIndexes[] = {
            // Front face
            0, 1, 2,
            2, 3, 0,
            // Back face
            6, 5, 4,
            4, 7, 6,
            // Left face
            4, 5, 1,
            1, 0, 4,
            // Right face
            3, 2, 6,
            6, 7, 3,
            // Top face
            1, 5, 6,
            6, 2, 1,
            // Bottom face
            4, 0, 3,
            3, 7, 4
        };

        float Offset = GlobalState.CurrAngle;

        Mat4 Transform = Mat4::PrespectiveMatrix(60.0f, AspectRatio, 1.0f, 6.f) *
                         CameraTransform *
                         Mat4::TranslationMatrix(0.0f, 0.0f, 4.0f) *
                         Mat4::RotationMatrix(Offset, Offset, Offset) *
                         Mat4::ScaleMatrix(1.0f, 1.0f, 1.0f);

        for (uint32_t IndexId = 0; IndexId < sizeof(ModelIndexes) / sizeof(ModelIndexes[0]); IndexId += 3) {
            uint32_t Index0 = ModelIndexes[IndexId + 0];
            uint32_t Index1 = ModelIndexes[IndexId + 1];
            uint32_t Index2 = ModelIndexes[IndexId + 2];

            DrawTriangle(
                ModelVertices[Index0], ModelVertices[Index1], ModelVertices[Index2],
                ModelColors[Index0], ModelColors[Index1], ModelColors[Index2],
                GlobalState.FrameBufferPixels,
                GlobalState.FrameBufferWidth,
                GlobalState.FrameBufferHeight,
                GlobalState.DepthBuffer,
                Transform
            );
        }

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