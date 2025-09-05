#include <windows.h>
#include <mmsystem.h>
#include <math.h>
#include <functional>

// --< Sound >--
using SndCalc = std::function<char(DWORD)>;

void play(SndCalc calc) {
    HWAVEOUT Wave = 0;
    WAVEFORMATEX Format = {WAVE_FORMAT_PCM, 1, 8000, 8000, 1, 8, 0};
    waveOutOpen(&Wave, WAVE_MAPPER, &Format, 0, 0, CALLBACK_NULL);
    char Sound[8000 * 30];

    for (DWORD t = 0; t < sizeof(Sound); ++t) {
		Sound[t] = calc(t);
    }

    WAVEHDR Head = { Sound, sizeof(Sound), 0, 0, 0, 0, 0, 0 };
    waveOutPrepareHeader(Wave, &Head, sizeof(WAVEHDR));
    waveOutWrite(Wave, &Head, sizeof(WAVEHDR));
    waveOutUnprepareHeader(Wave, &Head, sizeof(WAVEHDR));
    waveOutClose(Wave);
}

VOID WINAPI sound1() {
    play([](DWORD t) -> char {
        // CA98
        return static_cast<char>(t*(0xCA98CA98>>(t>>9&30)&15)|t>>8);
    });
}

VOID WINAPI sound2() {
    // Bear @ Celaphis
    play([](DWORD t) -> char {
        return static_cast<char>(t+(t&t^t>>6)-t*(t>>9&(t%16?2:6)&t>>9));
    });
}

VOID WINAPI sound3() {
    play([](DWORD t) -> char {
        return static_cast<char>(t*(1+((t>>10)*(43+(2*(t>>(15-((t>>16)%13)))%8)))%8)*(1+(t>>14)%4));
    });
}
VOID WINAPI sound4() {
    // lol i made this
    play([](DWORD t) -> char {
        return static_cast<char>(t * ((t >> 12) + 0xDEADC0DE | 4));
    });
}

VOID WINAPI sound5() {
    play([](DWORD t) -> char {
        // Mad neurofunk
        return static_cast<char>(t*((t&4096?t%65536<59392?7:5:10)^(1&t>>14))>>(3&-t>>(t&2048?2:10)));
    });
}

VOID WINAPI sound6() {
    play([](DWORD t) -> char {
        // credits: tejeez
        return static_cast<char>((~t>>2)*((127&t*(7&t>>10))<(245&t*(2+(5&t>>14)))));
    });
}

VOID WINAPI sound7() {
    play([](DWORD t) -> char {
        // lhphr (again lol i like his songs)
        return static_cast<char>((t>>10^t>>11)%5*((t>>14&3^t>>15&1)+1)*t%99+((3+(t>>14&3)-(t>>16&1))/3*t%99&64));
    });
}
// --< Sound >--

// --< Visual >--

DWORD WINAPI Furnace(LPVOID lpParam) {
    int width = GetSystemMetrics(0);
    int height = GetSystemMetrics(1);
	while (true) {
    	HDC Screen = GetDC(0);
   		BitBlt(Screen, rand() % width, 10, 100, height, Screen, rand() % width, 0, SRCCOPY);
    	ReleaseDC(NULL, Screen);
	}
}

DWORD WINAPI shader1(LPVOID lpParam) {
    HDC Screen = GetDC(0);
    int width = GetSystemMetrics(0);
    int height = GetSystemMetrics(1);
    
    BITMAPINFO bmi = { 0 };
    void* pBits = nullptr;
    
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = width;
    bmi.bmiHeader.biHeight = -height;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;
    
    HBITMAP hDIB = CreateDIBSection(Screen, &bmi, DIB_RGB_COLORS, &pBits, NULL, 0);
    HDC Memory = CreateCompatibleDC(Screen);
    SelectObject(Memory, hDIB);

    double mom = 0.0;

    while (true) {
        Screen = GetDC(0);
        BitBlt(Memory, 0, 0, width, height, Screen, 0, 0, SRCCOPY);
        DWORD* pixels = (DWORD*)pBits;

        int cx = width / 2;
        int cy = height / 2;
        double maxDist = sqrt(cx*cx + cy*cy);

        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                int dx = x - cx;
                int dy = y - cy;
                double dist = (dx*dx + dy*dy) / 2;

                double offsetX = x + (dx / dist) * sin(dist / 20.0 - mom * 2.0) * 10;
                double offsetY = y + (dy / dist) * sin(dist / 20.0 - mom * 2.0) * 10;

                int srcX = (int)offsetX;
                int srcY = (int)offsetY;

                if (srcX >= 0 && srcX < width && srcY >= 0 && srcY < height) {
                    int srcIdx = srcY * width + srcX;
                    int dstIdx = y * width + x;
                    pixels[dstIdx] = pixels[srcIdx];
                }
            }
        }

        for (int i = 0; i < width * height; i++) {
            DWORD pixel = pixels[i];

            BYTE a = (pixel >> 24);
            BYTE r = ((pixel >> 16) + 1)  % 256;
            BYTE g = ((pixel >> 8) + 2)  % 256;
            BYTE b = (pixel + 3) % 256;

            pixels[i] = (a << 24) | (r << 16) | (g << 8) | b;
        }

        HDC overlayDC = CreateCompatibleDC(Screen);
        HBITMAP overlayBitmap = CreateCompatibleBitmap(Screen, width, height);
        SelectObject(overlayDC, overlayBitmap);
        BITMAPINFO bmi2 = { 0 };
        bmi2.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bmi2.bmiHeader.biWidth = width;
        bmi2.bmiHeader.biHeight = -height;
        bmi2.bmiHeader.biPlanes = 1;
        bmi2.bmiHeader.biBitCount = 32;
        bmi2.bmiHeader.biCompression = BI_RGB;
        void* overlayBits = nullptr;
        HBITMAP tempBmp = CreateDIBSection(overlayDC, &bmi2, DIB_RGB_COLORS, &overlayBits, NULL, 0);
        memcpy(overlayBits, pixels, width * height * 4);
        SelectObject(overlayDC, tempBmp);

        BLENDFUNCTION blendFunc = { AC_SRC_OVER, 0, 180, 0 };
        AlphaBlend(Screen, 0, 0, width, height, overlayDC, 0, 0, width, height, blendFunc);

        DeleteObject(overlayBitmap);
        DeleteDC(overlayDC);

        mom += 1.2;
    }
}

POINT Pos = { 0, 0 }; // circle pos

DWORD WINAPI shader2(LPVOID lpParam) {
    HDC Screen = GetDC(0);
    int width = GetSystemMetrics(0);
    int height = GetSystemMetrics(1);

    BITMAPINFO bmi = { 0 };
    void* pBits = nullptr;

    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = width;
    bmi.bmiHeader.biHeight = -height;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    HBITMAP hDIB = CreateDIBSection(Screen, &bmi, DIB_RGB_COLORS, &pBits, NULL, 0);
    HDC Memory = CreateCompatibleDC(Screen);
    SelectObject(Memory, hDIB);

    double mom = 0.0;

    while (true) {
        Screen = GetDC(0);
        BitBlt(Memory, 0, 0, width, height, Screen, 0, 0, SRCCOPY);
        DWORD* pixels = (DWORD*)pBits;

        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                int dx = x - Pos.x;
                int dy = y - Pos.y;
                double dist = sqrt(dx*dx + dy*dy);

                double ripple = sin(dist / 20.0 - mom * 3) * 15 * exp(-dist / 300);

                double offsetX = x + (dx / dist) * ripple;
                double offsetY = y + (dy / dist) * ripple;

                int srcX = (int)offsetX;
                int srcY = (int)offsetY;

                if (srcX >= 0 && srcX < width && srcY >= 0 && srcY < height) {
                    int srcIdx = srcY * width + srcX;
                    int dstIdx = y * width + x;
                    pixels[dstIdx] = pixels[srcIdx];
                }
            }
        }

        for (int i = 0; i < width * height; i++) {
            DWORD pixel = pixels[i];

            BYTE a = (pixel >> 24);
            BYTE r = (pixel >> 16);
            BYTE g = (pixel >> 8);
            BYTE b = pixel;

            r = (r + (int)(sin(mom) * 50)) % 256;
            g = (g + (int)(cos(mom) * 50)) % 256;
            b = (b + (int)(sin(mom + 2.5) * 50)) % 256;

            pixels[i] = (a << 24) | (r << 16) | (g << 8) | b;
        }

        HDC overlayDC = CreateCompatibleDC(Screen);
        HBITMAP overlayBitmap = CreateCompatibleBitmap(Screen, width, height);
        SelectObject(overlayDC, overlayBitmap);
        BITMAPINFO bmi2 = { 0 };
        bmi2.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bmi2.bmiHeader.biWidth = width;
        bmi2.bmiHeader.biHeight = -height;
        bmi2.bmiHeader.biPlanes = 1;
        bmi2.bmiHeader.biBitCount = 32;
        bmi2.bmiHeader.biCompression = BI_RGB;
        void* overlayBits = nullptr;
        HBITMAP tempBmp = CreateDIBSection(overlayDC, &bmi2, DIB_RGB_COLORS, &overlayBits, NULL, 0);
        memcpy(overlayBits, pixels, width * height * 4);
        SelectObject(overlayDC, tempBmp);

        BLENDFUNCTION blendFunc = { AC_SRC_OVER, 0, 180, 0 };
        AlphaBlend(Screen, 0, 0, width, height, overlayDC, 0, 0, width, height, blendFunc);

        DeleteObject(overlayBitmap);
        DeleteDC(overlayDC);

        mom += 0.03;
        Pos.x = (Pos.x + 150) % width;
        Pos.y = (Pos.y + 150) % height;
    }
}

DWORD WINAPI shader3(LPVOID lpParam) {
    HDC Screen = GetDC(0);
    int width = GetSystemMetrics(0);
    int height = GetSystemMetrics(1);

    BITMAPINFO bmi = { 0 };
    void* pBits = nullptr;

    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = width;
    bmi.bmiHeader.biHeight = -height;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    HBITMAP hDIB = CreateDIBSection(Screen, &bmi, DIB_RGB_COLORS, &pBits, NULL, 0);
    HDC Memory = CreateCompatibleDC(Screen);
    SelectObject(Memory, hDIB);

    while (true) {
        Screen = GetDC(0);
        BitBlt(Memory, 0, 0, width, height, Screen, 0, 0, SRCCOPY);

        DWORD* pixels = (DWORD*)pBits;

        DWORD* temp = new DWORD[width * height];
        memcpy(temp, pixels, width * height * 4);

        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width - 1; x++) {
                int idx = y * width + x;
                int neighborIdx = y * width + (x + 1);

                DWORD p1 = temp[idx];
                DWORD p2 = temp[neighborIdx];

                BYTE a1 = (p1 >> 24) & 0xFF;
                BYTE r1 = (p1 >> 16) & 0xFF;
                BYTE g1 = (p1 >> 8) & 0xFF;
                BYTE b1 = p1 & 0xFF;

                BYTE a2 = (p2 >> 24) & 0xFF;
                BYTE r2 = (p2 >> 16) & 0xFF;
                BYTE g2 = (p2 >> 8) & 0xFF;
                BYTE b2 = p2 & 0xFF;

                BYTE r = (BYTE)((r1 * (1 - 0.1f)) + (r2 * 0.1f));
                BYTE g = (BYTE)((g1 * (1 - 0.1f)) + (g2 * 0.1f));
                BYTE b = (BYTE)((b1 * (1 - 0.1f)) + (b2 * 0.1f));
                BYTE a = (BYTE)((a1 * (1 - 0.1f)) + (a2 * 0.1f));

                pixels[idx] = (a << 24) | (r << 16) | (g << 8) | b;
            }
        }

        delete[] temp;
        BitBlt(Screen, 0, 0, width, height, Memory, 0, 0, SRCCOPY);
    }
}

DWORD WINAPI shader4(LPVOID lpParam) {
    HDC Screen = GetDC(0);
    int width = GetSystemMetrics(0);
    int height = GetSystemMetrics(1);

    BITMAPINFO bmi = { 0 };
    void* pBits = nullptr;

    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = width;
    bmi.bmiHeader.biHeight = -height;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    HBITMAP hDIB = CreateDIBSection(Screen, &bmi, DIB_RGB_COLORS, &pBits, NULL, 0);
    HDC Memory = CreateCompatibleDC(Screen);
    SelectObject(Memory, hDIB);

    double mom = 0.0;

    while (true) {
        Screen = GetDC(0);
        BitBlt(Memory, 0, 0, width, height, Screen, 0, 0, SRCCOPY);

        DWORD* pixels = (DWORD*)pBits;
        DWORD* temp = new DWORD[width * height];
        memcpy(temp, pixels, width * height * 4);

        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                double dx = (x - width / 2);
                double dy = (y - height / 2);
                double dist = sqrt(dx * dx + dy * dy);
                double ripple = sin(dist / 20.0 - mom * 2.0) * 10.0;
                int srcX = (int)(x + (dx / dist) * ripple);
                int srcY = (int)(y + (dy / dist) * ripple);

                srcX = std::max(0, std::min(width - 1, srcX));
                srcY = std::max(0, std::min(height - 1, srcY));

                int srcIdx = srcY * width + srcX;
                int dstIdx = y * width + x;

                pixels[dstIdx] = temp[srcIdx];
            }
        }

        delete[] temp;
        BitBlt(Screen, 0, 0, width, height, Memory, 0, 0, SRCCOPY);
        mom += 0.05;
        Sleep(30);
    }
}

DWORD WINAPI shader5(LPVOID lpParam) {
    HDC Screen = GetDC(0);
    int width = GetSystemMetrics(0);
    int height = GetSystemMetrics(1);

    BITMAPINFO bmi = { 0 };
    void* pBits = nullptr;

    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = width;
    bmi.bmiHeader.biHeight = -height;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    HBITMAP hDIB = CreateDIBSection(Screen, &bmi, DIB_RGB_COLORS, &pBits, NULL, 0);
    HDC Memory = CreateCompatibleDC(Screen);
    SelectObject(Memory, hDIB);

    double time = 0.0;

    while (true) {
        Screen = GetDC(0);
        BitBlt(Memory, 0, 0, width, height, Screen, 0, 0, SRCCOPY);
        DWORD* pixels = (DWORD*)pBits;

        DWORD* tempPixels = new DWORD[width * height];
        memcpy(tempPixels, pixels, width * height * 4);

        for (int y = 0; y < height; y++) {
            int ofx = (int)(10 * sin(2 * 3.14 * (y / 50.0) + time));
            for (int x = 0; x < width; x++) {
                int srcX = x + ofx;

                if (srcX < 0) srcX = 0;
                if (srcX >= width) srcX = width - 1;

                int dstIdx = y * width + x;
                int srcIdx = y * width + srcX;

                pixels[dstIdx] = tempPixels[srcIdx];
            }
        }

        delete[] tempPixels;

        BitBlt(Screen, 0, 0, width, height, Memory, 0, 0, SRCCOPY);

        time += 0.1;
        Sleep(30);
    }
}

DWORD WINAPI Shake(LPVOID lpParam) {
    int width = GetSystemMetrics(0);
    int height = GetSystemMetrics(1);

    while (true) {
        HDC Screen = GetDC(0);
        BitBlt(Screen, 0, 0, width, height, Screen, -30, 0, SRCCOPY);
        BitBlt(Screen, 0, 0, width, height, Screen, width - 30, 0, SRCCOPY);
        BitBlt(Screen, 0, 0, width, height, Screen, 0, -30, SRCCOPY);
        BitBlt(Screen, 0, 0, width, height, Screen, 0, height - 30, SRCCOPY);
        ReleaseDC(0, Screen);
    }
}

DWORD WINAPI shader6(LPVOID lpParam) {
    HDC Screen = GetDC(0);
    int width = GetSystemMetrics(0);
    int height = GetSystemMetrics(1);

    BITMAPINFO bmi = { 0 };
    void* pBits = nullptr;

    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = width;
    bmi.bmiHeader.biHeight = -height;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    HBITMAP hDIB = CreateDIBSection(Screen, &bmi, DIB_RGB_COLORS, &pBits, NULL, 0);
    HDC Memory = CreateCompatibleDC(Screen);
    SelectObject(Memory, hDIB);

    double time = 0.0;

    while (true) {
        BitBlt(Memory, 0, 0, width, height, Screen, 0, 0, SRCCOPY);
        DWORD* pixels = (DWORD*)pBits;

        DWORD* temp = new DWORD[width * height];

        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                float dx = x - width / 2.0f;
                float dy = y - height / 2.0f;
                float dist = sqrt(dx * dx + dy * dy);

                float rep = sin(4.0f * dist - time) * 10.0f;

                int srcX = (int)(x + (dx / dist) * rep);
                int srcY = (int)(y + (dy / dist) * rep);

                if (srcX < 0) srcX = 0;
                if (srcX >= width) srcX = width - 1;
                if (srcY < 0) srcY = 0;
                if (srcY >= height) srcY = height - 1;

                int srcIdx = srcY * width + srcX;
                int dstIdx = y * width + x;

                temp[dstIdx] = pixels[srcIdx];
            }
        }

        memcpy(pixels, temp, width * height * 4);
        delete[] temp;

        BitBlt(Screen, 0, 0, width, height, Memory, 0, 0, SRCCOPY);

        time += 1;
        Sleep(10);
    }
}

DWORD WINAPI shader7(LPVOID lpParam) {
    HDC Screen = GetDC(0);
    int width = GetSystemMetrics(0);
    int height = GetSystemMetrics(1);

    BITMAPINFO bmi = { 0 };
    void* pBits = nullptr;

    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = width;
    bmi.bmiHeader.biHeight = -height;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    HBITMAP hDIB = CreateDIBSection(Screen, &bmi, DIB_RGB_COLORS, &pBits, NULL, 0);
    HDC Memory = CreateCompatibleDC(Screen);
    SelectObject(Memory, hDIB);

    DWORD tm = GetTickCount();

    while (true) {
        BitBlt(Memory, 0, 0, width, height, Screen, 0, 0, SRCCOPY);

        DWORD* pixels = (DWORD*)pBits;
        double t = (GetTickCount() - tm) / 1000.0;

        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                int idx = y * width + x;
                double nx = (double)x / width - 0.5;
                double ny = (double)y / height - 0.5;

                double ang = 10 * (nx * nx + ny * ny) + t;
                double rad = sqrt(nx * nx + ny * ny);

                double disx = nx + 0.5 * sin(5 * rad + ang);
                double disy = ny + 0.5 * cos(5 * rad + ang);

                int srcX = (int)((disx + 0.25) * width);
                int srcY = (int)((disy + 0.25) * height);

                if (srcX >= 0 && srcX < width && srcY >= 0 && srcY < height) {
                    int srcIdx = srcY * width + srcX;
                    pixels[idx] = ((pixels[srcIdx] & 16777215) | (0x80 << 24));
                }
            }
        }

        BLENDFUNCTION blendFunc = { AC_SRC_OVER, 0, 80, 0 };
        HDC overlayDC = CreateCompatibleDC(Screen);
        HBITMAP overlayBitmap = CreateCompatibleBitmap(Screen, width, height);
        SelectObject(overlayDC, overlayBitmap);

        RECT rect = { 0, 0, width, height };
        HBRUSH brush = CreateSolidBrush(RGB(255, 100, 200));
        FillRect(overlayDC, &rect, brush);
        DeleteObject(brush);
        AlphaBlend(Screen, 0, 0, width, height, overlayDC, 0, 0, width, height, blendFunc);
        DeleteObject(overlayBitmap);
        DeleteDC(overlayDC);

        BitBlt(Screen, 0, 0, width, height, Memory, 0, 0, SRCCOPY);

        Sleep(20);
    }
}
// --< Visual >--

int main() {
    if (MessageBoxW(NULL, 
        L"The program also has flashing Lights\nIf you know to have photosensitive epilepsy then click off\nContinue?", 
        L"counterimmunoelectrophoresis.exe By chinawillspyonusa", 
        MB_YESNO | MB_ICONEXCLAMATION) == IDNO) {
        ExitProcess(0);
    }
    Sleep(5000)
    HANDLE shader_1 = CreateThread(0, 0, shader1, 0, 0, 0);
    HANDLE frn = CreateThread(0, 0, Furnace, 0, 0, 0);
    sound1();
    Sleep(30000);
    TerminateThread(shader_1, 0);
    TerminateThread(frn, 0);
    HANDLE shader_2 = CreateThread(0, 0, shader2, 0, 0, 0);
    sound2();
    Sleep(30000);
    TerminateThread(shader_2, 0);
    HANDLE shader_3 = CreateThread(0, 0, shader3, 0, 0, 0);
    sound3();
    Sleep(30000);
    TerminateThread(shader_3, 0);
    HANDLE shader_4 = CreateThread(0, 0, shader4, 0, 0, 0);
    sound4();
    Sleep(30000);
    TerminateThread(shader_4, 0);
    HANDLE shader_5 = CreateThread(0, 0, shader5, 0, 0, 0);
    HANDLE shk = CreateThread(0, 0, Shake, 0, 0, 0);
    sound5();
    Sleep(30000);
    TerminateThread(shader_5, 0);
    TerminateThread(shk, 0);
    HANDLE shader_6 = CreateThread(0, 0, shader6, 0, 0, 0);
    sound6();
    Sleep(30000);
    TerminateThread(shader_6, 0);
    HANDLE shader_7 = CreateThread(0, 0, shader7, 0, 0, 0);
    sound7();
    Sleep(30000);
    TerminateThread(shader_7, 0);
    return 1;
}