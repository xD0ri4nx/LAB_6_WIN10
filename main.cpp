#include <iostream>
#include <windows.h>

bool is_prime(int num) {
    if (num < 2) return false;
    for (int i = 2; i * i <= num; ++i) {
        if (num % i == 0) return false;
    }
    return true;
}

void performChildTask(HANDLE writePipe, int start, int end) {
    for (int num = start; num <= end; ++num) {
        if (is_prime(num)) {
            DWORD written;
            std::string primeStr = std::to_string(num) + "\n";
            WriteFile(writePipe, primeStr.c_str(), primeStr.size(), &written, nullptr);
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc < 4) {
        std::cerr << "Insufficient arguments!" << std::endl;
        return 1;
    }

    // Get the arguments passed from the parent process
    HANDLE writePipe = reinterpret_cast<HANDLE>(std::stoi(argv[1]));
    int start = std::stoi(argv[2]);
    int end = std::stoi(argv[3]);

    // Perform the task
    performChildTask(writePipe, start, end);

    return 0;
}
