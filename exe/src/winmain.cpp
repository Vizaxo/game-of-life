#include <Windows.h>

#include "app.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    App app;

    if (FAILED(app.init(hInstance, 640, 480)))
        return 0;

    MSG msg;
    memset(&msg, 0, sizeof(MSG));

    int start_time = timeGetTime();
    while (msg.message != WM_QUIT) {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        } else {
            int t = timeGetTime();
            float dt = (t - start_time) * 0.001f;

            app.update(dt, (float)t * 0.001f);
            app.render();
            start_time = t;
        }
    }

    app.cleanup();

    return (int)msg.wParam;
}