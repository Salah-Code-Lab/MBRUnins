#include <windows.h>
#include <setupapi.h>
#include <cfgmgr32.h> 
#include <devguid.h>
#include <vector>
#include <string>
#include <iostream>

#pragma comment(lib, "setupapi.lib")
#pragma comment(lib, "cfgmgr32.lib") 

const char* DISK_CLASS_GUID_STR = "System\\CurrentControlSet\\Control\\Class\\{4d36e967-e325-11ce-bfc1-08002be10318}";
const wchar_t* DRIVER_NAME = L"MBRFilterPP";

bool RemoveFilterFromRegistry() {
    HKEY hKey;
    if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"SYSTEM\\CurrentControlSet\\Control\\Class\\{4d36e967-e325-11ce-bfc1-08002be10318}", 0, KEY_ALL_ACCESS, &hKey) != ERROR_SUCCESS) {
        return false;
    }

    DWORD dwType, dwSize;
    // 1. Get size of current UpperFilters
    if (RegQueryValueExW(hKey, L"UpperFilters", NULL, &dwType, NULL, &dwSize) != ERROR_SUCCESS) {
        RegCloseKey(hKey);
        return false;
    }

    std::vector<wchar_t> buffer(dwSize / sizeof(wchar_t));
    RegQueryValueExW(hKey, L"UpperFilters", NULL, &dwType, (LPBYTE)buffer.data(), &dwSize);

    // 2. Build new list without our driver
    std::vector<wchar_t> newBuffer;
    wchar_t* p = buffer.data();
    while (*p) {
        if (_wcsicmp(p, DRIVER_NAME) != 0) {
            
            while (*p) {
                newBuffer.push_back(*p++);
            }
            newBuffer.push_back(L'\0'); 
        }
        else {
            // Skip our driver
            while (*p) p++;
        }
        p++;
    }
    newBuffer.push_back(L'\0'); 

    LSTATUS status = RegSetValueExW(hKey, L"UpperFilters", 0, REG_MULTI_SZ, (BYTE*)newBuffer.data(), (DWORD)(newBuffer.size() * sizeof(wchar_t)));

    RegCloseKey(hKey);
    return (status == ERROR_SUCCESS);
}

void RefreshDiskDevices() {
    CONFIGRET cr = CM_Reenumerate_DevNode(0, CM_REENUMERATE_NORMAL);

    if (cr == CR_SUCCESS) {
        std::cout << "PnP Re-enumeration triggered successfully." << std::endl;
    }
    else {
        std::cout << "Re-enumeration failed. Error code: " << cr << std::endl;
    }
}

bool CleanupService(const wchar_t* serviceName) {
    SC_HANDLE hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
    if (!hSCM) {
        std::wcerr << L"Failed to open Service Control Manager." << std::endl;
        return false;
    }

    SC_HANDLE hService = OpenServiceW(hSCM, serviceName, SERVICE_ALL_ACCESS);
    if (!hService) {
       
        if (GetLastError() == ERROR_SERVICE_DOES_NOT_EXIST) {
            CloseServiceHandle(hSCM);
            return true;
        }
        CloseServiceHandle(hSCM);
        return false;
    }

    SERVICE_STATUS status;
    if (ControlService(hService, SERVICE_CONTROL_STOP, &status)) {
        std::wcout << L"Stop signal sent to driver..." << std::endl;
        Sleep(1000);
    }


    if (DeleteService(hService)) {
        std::wcout << L"Service marked for deletion." << std::endl;
    }
    else {
        std::wcerr << L"DeleteService failed. Error: " << GetLastError() << std::endl;
    }

    CloseServiceHandle(hService);
    CloseServiceHandle(hSCM);
    return true;
}

int main() {
    int bootMode = GetSystemMetrics(SM_CLEANBOOT);

    if (bootMode == 0) {
        std::cerr << "####################################################" << std::endl;
        std::cerr << "FATAL ERROR: UNAUTHORIZED ENVIRONMENT" << std::endl;
        std::cerr << "This emergency tool can ONLY run in Safe Mode." << std::endl;
        std::cerr << "Running this in Normal Mode could brick the OS." << std::endl;
        std::cerr << "####################################################" << std::endl;

        std::cout << "\nPlease reboot into Safe Mode and try again." << std::endl;
        system("pause");
        return 1;
    }


    std::cout << "Safe Mode detected (Mode: " << bootMode << ")." << std::endl;
    std::cout << "Proceeding with MBRFilter++ removal..." << std::endl;

    if (RemoveFilterFromRegistry()) {
        std::cout << "1. Registry cleaned." << std::endl;
    }


    RefreshDiskDevices();
    std::cout << "2. PnP Stack refreshed. Driver should have detached." << std::endl;


    if (CleanupService(L"MBRFilterPP")) {
        std::cout << "3. Service cleanup complete." << std::endl;
    }

    std::cout << "\nUninstallation finished. The driver is no longer active." << std::endl;
    return 0;
}
