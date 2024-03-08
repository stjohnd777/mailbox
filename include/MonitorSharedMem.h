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


template <typename I >
class MonitorSharedMem : public MonitorBehavior<cv::Mat,cv::Mat> {

public:

    MonitorSharedMem( std:: string name){
        m_TmpFileInbox = std::string("/tmp") + std::string("/") + name + "IN";
        tmpFileOutSharedMemId = std::string("/tmp") + std::string("/") + name + "OUT";
        OnInit();
    }

    cv::Mat srcMat;
    cv::Mat preProcessedMat;
    cv::Mat processedMat;
    cv::Mat postProcessedMat;

    std::function<cv::Mat(cv::Mat)> preProcessing;
    std::function<cv::Mat(cv::Mat)> process ;
    std::function<cv::Mat(cv::Mat)> postProcessing;

    void OnInit() override {

        auto tupIn = GetSharedMemorySegment(m_TmpFileInbox);
        keyIn = get<0>(tupIn);
        m_SharedMemId_In = get<1>(tupIn);
        m_SharedMemoryIn = get<2>(tupIn);

        auto tupOut = GetSharedMemorySegment(m_TmpFileInbox);
        keyOut = get<0>(tupOut);
        m_SharedMemId_Out = get<1>(tupOut);
        m_SharedMemoryOut = get<2>(tupOut);

    }


    void Invoke() override {
        srcMat = ReadInbox();
        preProcessedMat = OnPreProcess(srcMat);
        processedMat = Process(preProcessedMat);
        postProcessedMat = OnPostProcess(processedMat);
        WriteOutBox(postProcessedMat);
    }


    void WriteInBox(cv::Mat mat){
        auto image = MatToImage<I>( mat);
        std::memcpy(m_SharedMemoryIn, image.get(), sizeof(I));
    }

    void WriteOutBox(cv::Mat mat){
        auto image = MatToImage<I>( mat);
        std::memcpy(m_SharedMemoryOut, image.get(), sizeof(I));
    }

    cv::Mat ReadInbox(){
        return ImageToMat(*m_SharedMemoryIn);
    }
    cv::Mat ReadOutBox(){
        return ImageToMat(*m_SharedMemoryOut);
    }

//    cv::Mat readFromSharedMemory(){
//        m_SharedMemoryIn = (I*) shmat(m_SharedMemId_In, nullptr, 0);
//        if (m_SharedMemoryIn == (I*)-1) {
//            perror("shmat");
//            exit(EXIT_FAILURE);
//        }
//        return ImageToMat(*m_SharedMemoryIn);
//    }


    cv::Mat OnPreProcess(cv::Mat mat) override {
        return  preProcessing != nullptr ? preProcessing(mat) : mat;
    }

    cv::Mat Process(cv::Mat mat) override{
        return process(mat);
    }

    cv::Mat OnPostProcess(cv::Mat mat) override {
        return postProcessing != nullptr ? postProcessing(mat.clone()) : mat;
    }

    void OnDisable() { }

    void OnEnable()  { }

    void OnDestroy() override {
        shmdt(m_SharedMemoryIn);
        shmctl(m_SharedMemId_In, IPC_RMID, nullptr);
        utils::fs::RemoveFile(m_TmpFileInbox);

        shmdt(m_SharedMemoryOut);
        shmctl(m_SharedMemId_Out, IPC_RMID, nullptr);
        utils::fs::RemoveFile(m_TmpFileOutbox);
    }

    ~MonitorSharedMem(){
        OnDestroy();
    }


private:

    key_t keyIn;
    I* m_SharedMemoryIn;
    int m_SharedMemId_In;
    std::string m_TmpFileInbox ;

    key_t keyOut;
    I* m_SharedMemoryOut;
    int m_SharedMemId_Out;
    std::string m_TmpFileOutbox;

    void writeLookUpFile(std::string lookUpFile, int shmemId){
        // write shared memory id to file
        int fd = open(lookUpFile.c_str(), O_CREAT | O_RDWR, 0666);
        if (fd == -1) {
            OnDestroy();
            throw std::runtime_error("could ot open temporary file to save shmemid " + m_TmpFileInbox );
        }
        write(fd, &shmemId, sizeof(shmemId));
        close(fd);
    }

    std::tuple<key_t,int,I*> GetSharedMemorySegment(std::string lookUpFile){

        key_t key = ftok("/tmp", 'A');
        if (key == -1) {
            throw std::runtime_error("ftok failed");
        }

        // Create or Get Shared Memory Segment
        int shmemId = shmget(key, sizeof(I), 0666 | IPC_CREAT);
        if (shmemId == -1) {
            OnDestroy();
            throw std::runtime_error(m_TmpFileInbox + ": could get write ot temp file sheared memory if  ... abort");
        }

        writeLookUpFile(lookUpFile,shmemId);

        // Attach to the shared memory
        I* sharedMemory = (I*)shmat(shmemId, nullptr, 0);
        if (sharedMemory == (I*)-1) {
            OnDestroy();
            throw std::runtime_error("cshmat: " + std::to_string(shmemId));
        }
        return std::make_tuple(key, shmemId, sharedMemory);
    }

};