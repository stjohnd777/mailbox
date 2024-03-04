//
// Created by e438262 on 3/4/2024.
//
#pragma once

#include <opencv2/core.hpp>

//////////////////////////////////
#include <nlohmann/json.hpp>
using namespace nlohmann;

class FileMedium {
public:
    std::string name;
    std::string inbox;
    std::string outbox;
    std::string errbox;
    std::string cmdbox;
    std::string logbox;
};
void to_json(json &j, const FileMedium &o){
    j["name"] = o.name;
    j["inbox"] = o.inbox;
    j["outbox"] = o.outbox;
    j["errbox"] = o.errbox;
    j["cmdbox"] = o.cmdbox;
    j["logbox"] = o.logbox;
}
void from_json(const json &j, FileMedium &o){
    o.name = j.at("name");
    o.inbox = j.at("inbox");
    o.outbox = j.at("outbox");
    o.errbox = j.at("errbox");
    o.cmdbox = j.at("cmdbox");
    o.logbox = j.at("logbox");
}
//////////////////////////////////

template<typename IN = cv::Mat, typename OUT = cv::Mat>
class MonitorBehavior {

    virtual void OnInit(const json &j) {

    }
    virtual void OnPreProcess(cv::Mat mat) {

    }

    virtual OUT Process(IN img){
    }

    virtual void OnPostProcess(cv::Mat mat) = 0;


    virtual void  OnDisable() {

    }
    virtual void  OnEnable() {
    }

    virtual void OnDestroy() {
    }

    std::function < OUT (const IN in )>  processImage;
};

class MonitorDir : public MonitorBehavior<cv::Mat,cv::Mat> {
public:
    void OnInit(const json &j) override {
    }

    void OnPreProcess(cv::Mat mat) override {
    }

    cv::Mat Process(cv::Mat mat) override{
    }

    void OnPostProcess(cv::Mat mat) override {
    }

    std::function < cv::Mat (const std::string& imgPath )>  processImage;
};


////////////////////////////////////////////////

#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <cstdlib>
#include <cstring>
const int IMAGE_WIDTH = 3840;
const int IMAGE_HEIGHT = 2160;
struct Image {
    unsigned char pixels[IMAGE_WIDTH][IMAGE_HEIGHT];
};
cv::Mat imageToMat(const Image& img) {
    // Create a CV_16U Mat for 16-bit grayscale image
    cv::Mat mat(IMAGE_HEIGHT, IMAGE_WIDTH, CV_16U);

    // Iterate through the pixels and set the values in the Mat
    for (int i = 0; i < IMAGE_WIDTH; ++i) {
        for (int j = 0; j < IMAGE_HEIGHT; ++j) {
            // Assuming 8-bit input, convert to 16-bit
            uint16_t pixelValue = static_cast<uint16_t>(img.pixels[i][j]) * 256;
            mat.at<uint16_t>(j, i) = pixelValue;
        }
    }

    return mat;
}

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

#include <iostream>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstdlib>
#include <cstring>

class MonitorSharedMem : public MonitorBehavior<cv::Mat,cv::Mat> {
private:
    key_t key;
    Image* sharedImage;
    int shmid;
public:

void OnInit(const json &j) override {

    // Create a key for shared memory
    // //(you can choose any unique integer)
    /*
     * ftok: This function is part of the POSIX standard for
     * System V IPC. It's used to generate a key for message
     * queues, semaphore sets, and shared memory segments.
     */
    key = ftok("/tmp", 'A');
    if (key == -1) {
        perror("ftok");
        exit(EXIT_FAILURE);
    }

    // Create or get the shared memory segment
    shmid = shmget(key, sizeof(Image), 0666 | IPC_CREAT);
    if (shmid == -1) {
        perror("shmget");
        exit(EXIT_FAILURE);
    }

    // Create a file to store the shmid
    int fd = open("/tmp/shmidfile", O_CREAT | O_RDWR, 0666);
    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    // Write the shmid to the file
    int shmid = shmget(key, sizeof(Image), 0666 | IPC_CREAT);
    if (shmid == -1) {
        perror("shmget");
        exit(EXIT_FAILURE);
    }


    // Attach to the shared memory
   sharedImage = (Image*)shmat(shmid, nullptr, 0);
    if (sharedImage == (Image*)-1) {
        perror("shmat");
        exit(EXIT_FAILURE);
    }

}

void OnPreProcess(cv::Mat mat) override {
    // Consumer Process: Read the image from shared memory
    sharedImage = (Image*)shmat(shmid, nullptr, 0);
    if (sharedImage == (Image*)-1) {
        perror("shmat");
        exit(EXIT_FAILURE);
    }

}

cv::Mat Process(cv::Mat mat) override{
}

void OnPostProcess(cv::Mat mat) override {
}

void OnDestroy() override {
    // Detach from the shared memory
    shmdt(sharedImage);
    // Destroy the shared memory segment (optional, comment out if not needed)
    shmctl(shmid, IPC_RMID, nullptr);
}
};