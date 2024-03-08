//
// Created by overman on 3/4/2024.
//
#pragma once

#include "MonitorBehavior.h"
#include "image.h"
#include "utils.h"

#include <sys/ipc.h>
#include <sys/shm.h>
#include <cstdlib>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstdlib>
#include <cstring>
#include "macros.h"

#include <opencv2/core.hpp>
#include "opencv2/highgui.hpp"

#include <nlohmann/json.hpp>
#include <utility>
using namespace nlohmann;

class SharedMemMedium {
public:
    std::string name;
};

void to_json(json &j, const SharedMemMedium &o){
    j["name"] = o.name;
}

void from_json(const json &j, SharedMemMedium &o){
    o.name = j.at("name");
}

template <typename I >
class MonitorSharedMem : public MonitorBehavior<cv::Mat,cv::Mat> {

    GETTERSETTER(key_t, key, Key)
    GETTERSETTER(I*, sharedImage, Image)
    GETTERSETTER(int, shmid, ShMemId)

public:

    cv::Mat mat;
    cv::Mat preMat;
    cv::Mat processedMat;
    cv::Mat postMat;

    std::function<cv::Mat(cv::Mat)> preProcessing;
    std::function<cv::Mat(cv::Mat)> process ;
    std::function<cv::Mat(cv::Mat)> postProcessing;

    void OnInit(const json &j) override {

        key = ftok("/tmp", 'A');
        if (key == -1) {
            perror("ftok");
            exit(EXIT_FAILURE);
        }

        // Create or get the shared memory segment
        shmid = shmget(key, sizeof(Image16U), 0666 | IPC_CREAT);
        if (shmid == -1) {
            perror("shmget");
            exit(EXIT_FAILURE);
        }

        // Create a file to store the shmid
        int fd = open("/tmp/shmid_sobel", O_CREAT | O_RDWR, 0666);
        if (fd == -1) {
            perror("open");
            exit(EXIT_FAILURE);
        }

        // Write the shmid to the file
        int shmid = shmget(key, sizeof(Image16U), 0666 | IPC_CREAT);
        if (shmid == -1) {
            perror("shmget");
            exit(EXIT_FAILURE);
        }

        // Attach to the shared memory
        sharedImage = (I*)shmat(shmid, nullptr, 0);
        if (sharedImage == (I*)-1) {
            perror("shmat");
            exit(EXIT_FAILURE);
        }

    }

    void invoke(){
        cv::Mat mat = readFromSharedMemory();
        OnPreProcess(mat);
    }

    cv::Mat readFromSharedMemory(){
        sharedImage = (I*) shmat(shmid, nullptr, 0);
        if (sharedImage == (I*)-1) {
            perror("shmat");
            exit(EXIT_FAILURE);
        }
        mat = ImageToMat( *sharedImage );
        return mat;
    }

    cv::Mat OnPreProcess(cv::Mat mat) override {
        preMat = process != nullptr ? process(mat):mat;
        Process(preMat.clone());
        return preMat;
    }

    cv::Mat Process(cv::Mat mat) override{
        processedMat = process(mat);
        OnPostProcess(processedMat.clone());
        return processedMat;
    }

    cv::Mat OnPostProcess(cv::Mat mat) override {
        postMat = postProcessing(mat.clone());
        return postMat;
    }

    void writeToSharedMemory(cv::Mat mat){
        auto image = MatToImage<I>( mat);
        std::memcpy(sharedImage, image.get(), sizeof(Image16U));
    }


    void OnDestroy() override {
        shmdt(sharedImage);
        shmctl(shmid, IPC_RMID, nullptr);
    }
};