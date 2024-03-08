//
// Created by overman on 3/6/2024.
//
#include <string>
#include <sstream>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

#include <nlohmann/json.hpp>
using namespace nlohmann;

#include "MonitorSharedMem.h"
#include "utils.h"
#include "macros.h"


int main(int argc, char** argv){

    std::string path = "/data/sun.jpg";
    if ( ! utils::fs::FileExist(path)){
        throw std::runtime_error("file dne " + path);
    }
    cv::Mat raw = cv::imread(path,cv::IMREAD_GRAYSCALE);
    if ( raw.empty()){
        throw std::runtime_error("image empty");
    }

    MonitorSharedMem<ImageGray8> shmemMonitor("sobel");

    // Set Handler
    shmemMonitor.process = [&](cv::Mat mat){
        return utils::vision::SobelEdge(mat);
    };

    // Get Gray
    cv::Mat mat = utils::vision::GetCvMAtAsGray8(path, 800, 600);

    // producer
    shmemMonitor.WriteInBox(mat);

    // invoke processing chain
    shmemMonitor.Invoke();

    // read out results
    cv::Mat after = shmemMonitor.ReadOutBox();

#if DEBUG
    cv::imshow("after", after);
    cv::waitKey(0);
#endif

    return  0;
}