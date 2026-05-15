#pragma once
#include <fstream>
#include <string>

class Logger {
public:
    void Initialize();
    void Finalize();

    void Log(const std::string& message);

private:
    std::ofstream stream_;
};