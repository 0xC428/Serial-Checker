#include <windows.h>
#include <iostream>

using namespace std;

BOOL IsRunningAsAdmin() {
    HANDLE hToken = NULL;
    TOKEN_ELEVATION elevation;
    DWORD dwSize;

    if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken)) {
        if (GetTokenInformation(hToken, TokenElevation, &elevation, sizeof(elevation), &dwSize)) {
            CloseHandle(hToken);
            return elevation.TokenIsElevated;
        }
        CloseHandle(hToken);
    }
    return FALSE;
}

void AdjustWindow(HWND hWnd) {
    SetConsoleTitleA("Serical Checker");

    RECT rect;

    GetWindowRect(hWnd, &rect);

    int x = rect.left;
    int y = rect.top;
    int nWidth = 750;
    int nHeight = 850;
    BOOL bRepaint = TRUE;

    MoveWindow(hWnd, x, y, nWidth, nHeight, bRepaint);
}

void PrintC(const char* text, WORD color) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color);
    cout << text;
    SetConsoleTextAttribute(hConsole, 6);
}


int main() {
    if (IsRunningAsAdmin()) {
        AdjustWindow(GetConsoleWindow());

        PrintC("[ Disks ]\n", 0xB);
        system("wmic diskdrive get model, serialnumber, size, mediaType");

        DWORD drives = GetLogicalDrives();

        for (char drive = 'A'; drive <= 'Z'; drive++) {
            if (drives & (1 << (drive - 'A'))) {
                string cmd = "vol ";
                cmd += drive;
                cmd += ":";

                system(cmd.c_str());
            }
        }
        cout << endl;

        PrintC("[ Motherboard ]\n", 0xB);
        system("wmic baseboard get serialnumber");

        PrintC("[ SMBIOS ]\n", 0xB);
        system("wmic path win32_computersystemproduct get uuid");

        PrintC("[ BIOS ]\n", 0xB);
        system("wmic csproduct get uuid, identifyingnumber");

        PrintC("[ CPU ]\n", 0xB);
        system("wmic cpu get serialnumber, processorid");

        PrintC("[ MAC ]", 0xB);
        system("powershell \"Get-NetAdapter | Select-Object InterfaceDescription, Status, MacAddress\"");
        system("wmic path Win32_NetworkAdapter where \"PNPDeviceID like '%%PCI%%' AND NetConnectionStatus=2 AND AdapterTypeID='0'\" get MacAddress");

        PrintC("[ TPM ]", 0xB);
        system("powershell \"Get-Tpm | Select-Object ManufacturerIdTxt, TpmPresent, TpmReady, TpmActivated, OwnerAuth\"");

        PrintC("[ Secure Boot ]\n", 0xB);
        system("powershell \"Confirm-SecureBootUEFI\"");

        PrintC("\n> Press Enter to exit <\n", 0xB);

        cin.get();
    }
    else {
        MessageBoxA(NULL,
            "This program must be run as administrator.",
            "Insufficient Privileges",
            MB_OK | MB_ICONERROR);
    }

	return 0;
}