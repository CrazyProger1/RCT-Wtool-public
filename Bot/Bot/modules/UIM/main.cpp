#include <iostream>
#include <windows.h>
#include <string>
#include <cstdio>
#include <memory>
#include <stdexcept>
#include <array>
#include <direct.h>
#include <shlobj.h>
#include <fstream>
#include <winuser.h>

enum COMMANDS {
    SPEAK = 0,
    MSG_BOX = 1,
    SCREENSHOT = 2,
    PHOTO = 3
};


namespace utils {
    std::string exec(const char *cmd) {
        SetConsoleOutputCP(CP_UTF8);
        setvbuf(stdout, nullptr, _IOFBF, 1000);

        std::array<char, 128> buffer{};
        std::string result;
        std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
        if (!pipe) {
            throw std::runtime_error("popen() failed!");
        }
        while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
            result += buffer.data();
        }
        return result;
    }

    int showMessageBox(const std::string &title, const std::string &prompt, const std::string &type) {
        if (type == "info")
            MessageBox(nullptr, prompt.c_str(), title.c_str(), MB_ICONINFORMATION);
        else if (type == "critic")
            MessageBox(nullptr, prompt.c_str(), title.c_str(), MB_ICONERROR);
        else if (type == "excl")
            MessageBox(nullptr, prompt.c_str(), title.c_str(), MB_ICONEXCLAMATION);
        else if (type == "ques")
            MessageBox(nullptr, prompt.c_str(), title.c_str(), MB_ICONQUESTION);
    }

    void speak(const std::string &text) {
        TCHAR appDataPath[MAX_PATH];

        SHGetFolderPath(
                nullptr,
                CSIDL_COMMON_APPDATA | CSIDL_FLAG_CREATE,
                nullptr,
                SHGFP_TYPE_CURRENT,
                appDataPath
        );


        std::string stringAppDataPath = appDataPath;

        stringAppDataPath = R"(C:\Users\crazy\Desktop)";

        std::ofstream fout;
        fout.open(stringAppDataPath + "xxx.vbs");
        fout << "Set voice = CreateObject(\"SAPI.SpVoice\")\n"
                "voice.Rate = 1\n"
                "voice.Volume = 90\n"
                "Set voice.Voice = voice.GetVoices.Item(1)\n"
                "voice.Speak \"" + text + "\"";

        fout.close();

        std::string command = "start " + stringAppDataPath + "xxx.vbs";
        exec(command.c_str());

    }

    int takeScreenshot(LPCTSTR szFile, int resX = 0, int resY = 0) {
        HDC hdcScr, hdcMem;
        HBITMAP hbmScr;
        BITMAP bmp;
        int iXRes, iYRes;

        hdcScr = CreateDC("DISPLAY", nullptr, nullptr, nullptr);
        hdcMem = CreateCompatibleDC(hdcScr);

        iXRes = GetDeviceCaps(hdcScr, HORZRES) * 1.25;
        iYRes = GetDeviceCaps(hdcScr, VERTRES) * 1.25;

        if (resX != 0 && resY != 0) {
            iXRes = resX;
            iYRes = resY;
        }



        // Create a compatible bitmap for hdcScreen.
        hbmScr = CreateCompatibleBitmap(hdcScr, iXRes, iYRes);
        if (hbmScr == nullptr) return 0;

        // Select the bitmaps into the compatible DC.
        if (!SelectObject(hdcMem, hbmScr)) return 0;

        // Copy color data for the entire display into a
        // bitmap that is selected into a compatible DC.
        if (!StretchBlt(hdcMem,
                        0, 0, iXRes, iYRes,
                        hdcScr,
                        0, 0, iXRes, iYRes,
                        SRCCOPY))

            return 0;

        // Source[2]
        PBITMAPINFO pbmi;
        WORD cClrBits;

        // Retrieve the bitmap's color format, width, and height.
        if (!GetObject(hbmScr, sizeof(BITMAP), (LPSTR) &bmp)) return 0;

        // Convert the color format to a count of bits.
        cClrBits = (WORD) (bmp.bmPlanes * bmp.bmBitsPixel);
        if (cClrBits == 1)
            cClrBits = 1;
        else if (cClrBits <= 4)
            cClrBits = 4;
        else if (cClrBits <= 8)
            cClrBits = 8;
        else if (cClrBits <= 16)
            cClrBits = 16;
        else if (cClrBits <= 24)
            cClrBits = 24;
        else cClrBits = 32;

        // Allocate memory for the BITMAPINFO structure. (This structure
        // contains a BITMAPINFOHEADER structure and an array of RGBQUAD
        // data structures.)
        if (cClrBits != 24)
            pbmi = (PBITMAPINFO) LocalAlloc(LPTR,
                                            sizeof(BITMAPINFOHEADER) +
                                            sizeof(RGBQUAD) * (1 << cClrBits));

            // There is no RGBQUAD array for the 24-bit-per-pixel format.
        else
            pbmi = (PBITMAPINFO) LocalAlloc(LPTR,
                                            sizeof(BITMAPINFOHEADER));

        // Initialize the fields in the BITMAPINFO structure.
        pbmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        pbmi->bmiHeader.biWidth = bmp.bmWidth;
        pbmi->bmiHeader.biHeight = bmp.bmHeight;
        pbmi->bmiHeader.biPlanes = bmp.bmPlanes;
        pbmi->bmiHeader.biBitCount = bmp.bmBitsPixel;
        if (cClrBits < 24)
            pbmi->bmiHeader.biClrUsed = (1 << cClrBits);

        // If the bitmap is not compressed, set the BI_RGB flag.
        pbmi->bmiHeader.biCompression = BI_RGB;

        // Compute the number of bytes in the array of color
        // indices and store the result in biSizeImage.
        pbmi->bmiHeader.biSizeImage = (pbmi->bmiHeader.biWidth + 7) / 8
                                      * pbmi->bmiHeader.biHeight * cClrBits;

        // Set biClrImportant to 0, indicating that all of the
        // device colors are important.
        pbmi->bmiHeader.biClrImportant = 0;

        HANDLE hf;                  // file handle
        BITMAPFILEHEADER hdr;       // bitmap file-header
        PBITMAPINFOHEADER pbih;     // bitmap info-header
        LPBYTE lpBits;              // memory pointer
        DWORD dwTotal;              // total count of bytes
        DWORD cb;                   // incremental count of bytes
        BYTE *hp;                   // byte pointer
        DWORD dwTmp;

        pbih = (PBITMAPINFOHEADER) pbmi;
        lpBits = (LPBYTE) GlobalAlloc(GMEM_FIXED, pbih->biSizeImage);

        if (!lpBits) return 0;

        // Retrieve the color table (RGBQUAD array) and the bits
        // (array of palette indices) from the DIB.
        if (!GetDIBits(hdcMem, hbmScr, 0, (WORD) pbih->biHeight, lpBits, pbmi, DIB_RGB_COLORS)) return 0;

        // Create the .BMP file.
        hf = CreateFile(szFile,
                        GENERIC_READ | GENERIC_WRITE,
                        (DWORD) 0,
                        NULL,
                        CREATE_ALWAYS,
                        FILE_ATTRIBUTE_NORMAL,
                        (HANDLE) NULL);
        if (hf == INVALID_HANDLE_VALUE) return 0;

        hdr.bfType = 0x4d42;        // 0x42 = "B" 0x4d = "M"

        // Compute the size of the entire file.
        hdr.bfSize = (DWORD) (sizeof(BITMAPFILEHEADER) +
                              pbih->biSize + pbih->biClrUsed *
                                             sizeof(RGBQUAD) + pbih->biSizeImage);
        hdr.bfReserved1 = 0;
        hdr.bfReserved2 = 0;

        // Compute the offset to the array of color indices.
        hdr.bfOffBits = (DWORD) sizeof(BITMAPFILEHEADER) +
                        pbih->biSize + pbih->biClrUsed *
                                       sizeof(RGBQUAD);

        // Copy the BITMAPFILEHEADER into the .BMP file.
        if (!WriteFile(hf, (LPVOID) &hdr, sizeof(BITMAPFILEHEADER), (LPDWORD) &dwTmp, NULL)) return 0;

        // Copy the BITMAPINFOHEADER and RGBQUAD array into the file.
        if (!WriteFile(hf, (LPVOID) pbih, sizeof(BITMAPINFOHEADER)
                                          + pbih->biClrUsed * sizeof(RGBQUAD),
                       (LPDWORD) &dwTmp, NULL))
            return 0;

        // Copy the array of color indices into the .BMP file.
        dwTotal = cb = pbih->biSizeImage;
        hp = lpBits;
        if (!WriteFile(hf, (LPSTR) hp, (int) cb, (LPDWORD) &dwTmp, NULL)) return 0;

        // Close the .BMP file.
        if (!CloseHandle(hf)) return 0;

        // Free memory.
        GlobalFree((HGLOBAL) lpBits);
        ReleaseDC(0, hdcScr);
        ReleaseDC(0, hdcMem);

        return 1;
    }

}


int main(int argc, char *argv[]) {
    if (argc > 1) {
        try {
            int command = std::stoi(argv[1]);

            switch (command) {
                case SPEAK:
                    utils::speak(argv[2]);
                    std::cout << "Spoked" << std::endl;
                    break;
                case MSG_BOX:
                    utils::showMessageBox(argv[2], argv[3], argv[4]);
                    std::cout << "Message box showed" << std::endl;
                    break;
                case SCREENSHOT:
                    if (argc == 5) {
                        utils::takeScreenshot(argv[2], std::stoi(argv[3]), std::stoi(argv[4]));
                        std::cout << "Screenshot saved <" << argv[2] << ">" << std::endl;
                    } else {
                        utils::takeScreenshot("screenshot.bmp");
                        std::cout << "Screenshot saved <" << "screenshot.bmp" << ">" << std::endl;
                    }
                    break;
                case PHOTO:
                    break;
                default:
                    exit(-1);

            }
        } catch (std::invalid_argument &e) {
            exit(-1);
        } catch (std::logic_error &e) {
            exit(-1);
        }
    } else exit(-1);

    return EXIT_SUCCESS;

}