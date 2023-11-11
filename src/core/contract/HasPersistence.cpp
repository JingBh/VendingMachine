#include "HasPersistence.h"

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>

#ifdef _WIN32
#include <windows.h>
#include <shlobj.h>
#endif

void HasPersistence::saveState() const {
    try {
        std::ofstream os(getFilePath());
        if (!os.is_open()) {
            throw std::runtime_error("Cannot open persistence file");
        }

        saveState(os);

        os.close();
    } catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
    }
}

void HasPersistence::loadState() {
    try {
        std::ifstream is(getFilePath());
        if (!is.is_open()) {
            throw std::runtime_error("Cannot open persistence file");
        }

        loadState(is);

        is.close();
    } catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
    }
}

std::string HasPersistence::getFilePath() {
    if (!filePath.empty()) {
        return filePath;
    }

    filePath = const_cast<const HasPersistence *>(this)->getFilePath();
    return filePath;
}

std::string HasPersistence::getFilePath() const {
    if (!filePath.empty()) {
        return filePath;
    }

    std::string dir;

#ifdef _WIN32
    char localAppData[MAX_PATH];
    if (SUCCEEDED(SHGetFolderPath(nullptr, CSIDL_LOCAL_APPDATA, nullptr, 0, localAppData))) {
        dir = std::string(localAppData) + R"(\JingBh\VendingMachine\)";
    } else {
        throw std::runtime_error("Cannot get persistence directory");
    }
#else
    // POSIX compliant systems (Linux, Unix, Mac OS X, etc.)
    char const *homeDir = std::getenv("HOME");
    if (homeDir) {
        dir = std::string(homeDir) + "/.local/share/jingbh/vending-machine/";
    } else {
        throw std::runtime_error("Cannot get persistence directory");
    }
#endif

    std::filesystem::create_directories(dir);

    return dir + name + ".dat";
}
