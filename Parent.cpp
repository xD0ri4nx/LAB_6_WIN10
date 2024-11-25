#include <iostream>
#include <windows.h>
#include <sstream>

const int NUM_PROCESSES = 10;
const int RANGE = 100;

// Function to check if a number is prime
bool is_prime(int num) {
    if (num < 2) return false;
    for (int i = 2; i * i <= num; ++i) {
        if (num % i == 0) return false;
    }
    return true;
}

// Function to create child processes
void createChildProcess(int i, int start, int end, HANDLE pipeWriteEnd, PROCESS_INFORMATION &procInfo) {
    // Set up security attributes
    SECURITY_ATTRIBUTES saAttr = {sizeof(SECURITY_ATTRIBUTES), nullptr, TRUE};

    // Create a pipe for inter-process communication (IPC)
    HANDLE readPipe, writePipe;
    if (!CreatePipe(&readPipe, &writePipe, &saAttr, 0)) {
        std::cerr << "Failed to create pipe." << std::endl;
        exit(EXIT_FAILURE);
    }

    if (!SetHandleInformation(writePipe, HANDLE_FLAG_INHERIT, 0)) {
        std::cerr << "Failed to set pipe handle information." << std::endl;
        exit(EXIT_FAILURE);
    }

    // Prepare the command to run the child process
    std::ostringstream cmdArgs;
    cmdArgs << "C:\\Users\\Legion\\Documents\\ACE_3\\Sisteme de Operare Bun\\Lab_SO_6_Child\\cmake-build-debug\\lab_6_SO_WIN_Child.exe "
            << reinterpret_cast<intptr_t>(writePipe) << " "
            << start << " "
            << end;

    std::string cmdLine = cmdArgs.str();

    // Set up the startup info for the process
    STARTUPINFO startInfo;
    ZeroMemory(&startInfo, sizeof(STARTUPINFO));
    startInfo.cb = sizeof(STARTUPINFO);
    std::cout << "Attempting to launch child process with command: " << cmdLine << std::endl;
    // Create the child process
    if (!CreateProcess(
            nullptr,
            const_cast<char*>(cmdLine.c_str()),
            nullptr,
            nullptr,
            TRUE,
            0,
            nullptr,
            nullptr,
            &startInfo,
            &procInfo)) {
        std::cerr << "Failed to create child process." << std::endl;
        exit(EXIT_FAILURE);
    }
    std::cout << "Successfully launched child process " << i + 1 << std::endl;
    // Close the write end of the pipe in the parent process
    CloseHandle(writePipe);
}

// Function to read from the pipe and display the results
void readChildResults(HANDLE readPipe) {
    char buffer[1024];
    DWORD bytesRead;
    while (ReadFile(readPipe, buffer, sizeof(buffer) - 1, &bytesRead, nullptr) && bytesRead > 0) {
        buffer[bytesRead] = '\0';  // Null-terminate the string
        std::cout << buffer;
    }
}

int main() {
    HANDLE pipes[NUM_PROCESSES][2];  // Store pipes for each child process
    PROCESS_INFORMATION procInfo[NUM_PROCESSES];

    // Create child processes
    for (int i = 0; i < NUM_PROCESSES; ++i) {
        int start = i * RANGE + 1;
        int end = (i + 1) * RANGE;
        createChildProcess(i, start, end, pipes[i][1], procInfo[i]);
    }

    // Wait for each child process and read their results
    for (int i = 0; i < NUM_PROCESSES; ++i) {
        WaitForSingleObject(procInfo[i].hProcess, INFINITE);
        readChildResults(pipes[i][0]);
        CloseHandle(procInfo[i].hProcess);
    }

    std::cout << "Parent process finished." << std::endl;
    return 0;
}
