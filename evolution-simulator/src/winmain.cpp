#include <Windows.h>

#include "app.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    App app;

    if (FAILED(app.init(hInstance, 1920, 1080)))
        return 0;

    MSG msg;
    memset(&msg, 0, sizeof(MSG));

    LARGE_INTEGER qpc;
    QueryPerformanceFrequency(&qpc);

    float frequency = (float)qpc.QuadPart;

    QueryPerformanceCounter(&qpc);
    u64 start_time = qpc.QuadPart;
    u64 last_frame_time = start_time;
    while (msg.message != WM_QUIT) {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        } else {
            QueryPerformanceCounter(&qpc);
            u64 t = qpc.QuadPart;
            float dt = (float)(t - last_frame_time) / frequency;

            app.update(dt, (float)(t - start_time) / frequency);
            app.render();
            last_frame_time = t;
        }
    }

    app.cleanup();

    return (int)msg.wParam;
}