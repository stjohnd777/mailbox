//
// Created by overman on 3/6/2024.
//
#include <string>

#include <opencv4/opencv2/core.hpp>
#include <opencv4/opencv2/highgui.hpp>

#include <nlohmann/json.hpp>
using namespace nlohmann;

#include "MonitorSharedMem.h"
#include "utils.h"
#include "macros.h"

int main(int argc, char** argv){

    auto process = [&](cv::Mat mat){
        return utils::vision::Edge(mat);
    };

    MonitorSharedMem<Image8U> shmemMonitor;
    json j;
    shmemMonitor.OnInit(j);

    shmemMonitor.process = process;

    std::string path = "/data/pia23810.jpg";
    cv::Mat mat = utils::vision::GetImageAsGray8(path,2592,1944);

#if DEBUG
    cv::imshow("before",mat);
    cv::waitKey(0);
#endif

    shmemMonitor.writeToSharedMemory(mat);
    shmemMonitor.invoke();
    cv::Mat after = shmemMonitor.readFromSharedMemory();

#if DEBUG
    cv::imshow("after", after);
    cv::waitKey(0);
#endif


    return  0;
}