#include "Process.h"

std::wstring getStateString(const DWORD state)
{
    switch (state)
    {
        case SERVICE_STOPPED:
            return L"Stopped";
        case SERVICE_START_PENDING:
            return L"Start Pending";
        case SERVICE_STOP_PENDING:
            return L"Stop Pending";
        case SERVICE_RUNNING:
            return L"Running";
        case SERVICE_CONTINUE_PENDING:
            return L"Continue Pending";
        case SERVICE_PAUSE_PENDING:
            return L"Pause Pending";
        case SERVICE_PAUSED:
            return L"Paused";
        default:
            return L"Unknown";
    }
}

bool Process::endProcess(const int process_id)
{
    // Get a handle to the process
    const auto hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, process_id);

    if (hProcess == nullptr)
    {
        std::cerr << "Failed to open process with ID " << process_id << ". Error: " << GetLastError() << std::endl;
        return false;
    }

    // Terminate the process
    if (TerminateProcess(hProcess, 0))
    {
        std::cout << "Process with ID " << process_id << " terminated successfully." << std::endl;
    }
    else
    {
        std::cerr << "Failed to terminate process with ID " << process_id << ". Error: " << GetLastError() << std::endl;
    }
    // Close the process handle
    CloseHandle(hProcess);
    return true;
}

std::vector<ProcessInfo> Process::listProcesses()
{
    std::vector<ProcessInfo> processes;
    const auto hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE)
    {
        throw std::runtime_error("CreateToolhelp32Snapshot failed");
    }

    PROCESSENTRY32W pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32W);

    if (!Process32FirstW(hSnapshot, &pe32))
    {
        CloseHandle(hSnapshot);
        throw std::runtime_error("Process32First failed");
    }

    do
    {
        const auto hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pe32.th32ProcessID);
        if (hProcess)
        {
            wchar_t szProcessPath[MAX_PATH];
            if (GetModuleFileNameExW(hProcess, NULL, szProcessPath, MAX_PATH))
            {
                ProcessType type;
                if (wcsstr(szProcessPath, L"\\Windows\\") != nullptr)
                {
                    type = ProcessType::WindowsProcess;
                }
                else if (pe32.cntThreads > 1)
                {
                    type = ProcessType::App;
                }
                else
                {
                    type = ProcessType::BackgroundProcess;
                }
                processes.push_back({pe32.th32ProcessID, pe32.szExeFile, type});
            }
            CloseHandle(hProcess);
        }
    } while (Process32NextW(hSnapshot, &pe32));

    CloseHandle(hSnapshot);
    return processes;
}