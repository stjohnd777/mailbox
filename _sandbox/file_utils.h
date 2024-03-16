#pragma once

#define HAS_FILESYSTEM 0

#if HAS_FILESYSTEM
#include <filesystem>
namespace fs = std::filesystem;
#endif

#include <iostream>
#include <string>
#include <functional>

#include <cstdio>
#include <cstring>
#include <dirent.h>

namespace utils2 {
        void RemoveFile(const std::string &src) {
#if HAS_FILESYSTEM
            try{
                if (fs::exists(src)) {
                    fs::remove(src);
                    std::cout << "File '" << src << "' successfully processed and removed." << std::endl;
                }
             }catch(...) {
                // swallow
            }
#else
            try {
                if (std::remove(src.c_str()) != 0) {
                    perror("Error deleting file");
                } else {
                    std::cout << "File deleted successfully" << std::endl;
                }
            }catch(...) {
                // swallow
            }
#endif
        }

        void MoveFile(const std::string &src, const std::string &dst) {
#if HAS_FILESYSTEM
            fs::rename(src, dst);
#else
            if (std::remove(src.c_str()) != 0) {
                perror("Error deleting file");
            } else {
                std::cout << "File deleted successfully" << std::endl;
            }
#endif

        }

        bool FileExist(const std::string &path) {
#if HAS_FILESYSTEM
            return (fs::exists(path));
#else
            FILE *file = std::fopen(path.c_str(), "r");
            // Check if the file was successfully opened
            if (file) {
                // File exists, close it and return true
                std::fclose(file);
                return true;
            } else {
                // File doesn't exist or couldn't be opened, return false
                return false;
            }
#endif
        }

        std::string GetFileNameFromPath(const std::string &path) {
#if HAS_FILESYSTEM
            fs::path filePath = path;
                fs::path fileName = filePath.filename();
#else
            size_t lastSlash = path.find_last_of("/\\");
            if (lastSlash != std::string::npos) {
                // Found a slash, extract the substring after the last slash
                return path.substr(lastSlash + 1);
            }
            // No slash found, return the entire path as the filename
            return path;
#endif
        }

        template<typename R>
        void ProcessFilesInDir(std::string &inbox, std::string &outbox, std::function<R(std::string)> processor) {
#if HAS_FILESYSTEM
            for (const auto& entry : fs::directory_iterator(inbox)) {
                std::cout << entry.path() << std::endl;
                R aR = processImage(entry.path(),outbox);
                }
#else
            DIR *dir;
            struct dirent *entry;
            if ((dir = opendir(inbox.c_str())) != nullptr) {
                while ((entry = readdir(dir)) != nullptr) {
                    if (entry->d_type == DT_REG) { // Check if it's a regular file
                        auto aFileName = entry->d_name;
                        R aR = processor(aFileName);
                    }
                }
                closedir(dir);
            }
#endif
        }
}