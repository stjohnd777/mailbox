//
// Created by overman on 3/1/2024.
//

#pragma once

#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include <iostream>
#include <string>
#include <filesystem>

namespace fs = std::filesystem;

#define HAS_FILESYSTEM 0

#include <cstdio>
#include <cstring>
#include <dirent.h>

namespace dsj {

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

    class FileUtils {
    public:

        static std::string GetFileNameFromPath(const std::string &path) {
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


        static bool FileExist(const std::string &path) {
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

        static void MoveFile(const std::string &src, const std::string &dst) {
#if HAS_FILESYSTEM
            fs::rename(src, dst);
#else
            if (std::rename(src.c_str(), dst.c_str()) != 0) {
                std::cerr << "Error moving file" << std::endl;
            } else {
                std::cout << "File moved successfully" << std::endl;
            }
#endif
        }

        static void RemoveFile(const std::string &src) {
#if HAS_FILESYSTEM
            if (fs::exists(src)) {
                fs::remove(src);
                std::cout << "File '" << src << "' successfully processed and removed." << std::endl;
            }
#else
            if (std::remove(src.c_str()) != 0) {
                perror("Error deleting file");
            } else {
                std::cout << "File deleted successfully" << std::endl;
            }
#endif
        }

    };

}

namespace utils {

    std::string GetCurrentTime() {
        auto currentTimePoint = std::chrono::system_clock::now();
        std::time_t currentTime = std::chrono::system_clock::to_time_t(currentTimePoint);
        char timeString[100];
        std::strftime(timeString, sizeof(timeString), "%Y-%m-%d.%H:%M:%S", std::localtime(&currentTime));
        return timeString;
    }


    void RemoveFile(const std::string &src) {
        if (fs::exists(src)) {
            fs::remove(src);
            std::cout << "File '" << src << "' successfully processed and removed." << std::endl;
        }
    }

    void MoveFile(const std::string &src, const std::string &dst) {
        fs::rename(src, dst);
    }


    /**
     * Shuts the system down by polling a cmd directoru
     * for a shutdown file, if the file 'shutdown.cmd is
     * present it stop the main thread
     * @return
     */
    bool isShutDown(const std::string cmdbox) {
        bool isShutdown = false;
        const std::string fileName = "shutdown.cmd";
        fs::path filePath = cmdbox + fs::path::preferred_separator + fileName;
        if (fs::exists(filePath)) {
            std::cout << "Shutdown command file found. Initiating shutdown..." << std::endl;
            isShutdown = true;
        }
        return isShutdown;
    }


    cv::Mat Edge(cv::Mat mat, int ksize = -1, int ddepth = CV_16S, int scale = 1, int delta = 0) {
        cv::Mat src = mat.clone();
        GaussianBlur(src, src, cv::Size(3, 3), 0, 0,
                     cv::BORDER_DEFAULT);// Remove noise by blurring with a Gaussian filter ( kernel size = 3 )
        cvtColor(src, src, cv::COLOR_BGR2GRAY);
        cv::Mat grad_x;
        Sobel(src, grad_x, ddepth, 1, 0, ksize, scale, delta, cv::BORDER_DEFAULT);
        cv::Mat grad_y;
        Sobel(src, grad_y, ddepth, 0, 1, ksize, scale, delta, cv::BORDER_DEFAULT);
        cv::Mat abs_grad_x, abs_grad_y;
        convertScaleAbs(grad_x, abs_grad_x);
        convertScaleAbs(grad_y, abs_grad_y);
        addWeighted(abs_grad_x, 0.5, abs_grad_y, 0.5, 0, src);
        return src;
    }

}