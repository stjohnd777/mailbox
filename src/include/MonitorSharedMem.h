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
#include <sys/sem.h>

#include <iostream>
#include <fstream>

#include "macros.h"

#include <opencv2/core.hpp>
#include "opencv2/highgui.hpp"


template <typename I >
class MonitorSharedMem : public MonitorBehavior<cv::Mat,cv::Mat> {

public:

    MonitorSharedMem( const std:: string& name){
        m_lookup = std::string("/tmp") + std::string("/") + name + ".txt";
        OnInit();
    }

    bool isRunning;

    cv::Mat srcMat;
    cv::Mat preProcessedMat;
    cv::Mat processedMat;
    cv::Mat postProcessedMat;

    std::function<cv::Mat(cv::Mat)> preProcessing;
    std::function<cv::Mat(cv::Mat)> process ;
    std::function<cv::Mat(cv::Mat)> postProcessing;


    void OnInit() override {


        sem_in_id = semget(IPC_PRIVATE, 1, 0666 | IPC_CREAT);// Create or get a semaphore for synchronization
        semctl(sem_in_id, 0, SETVAL, 0); // Initialize the semaphore
        auto tupIn = GetSharedMemorySegment();
        keyIn = get<0>(tupIn);
        m_SharedMemId_In = get<1>(tupIn);
        m_SharedMemoryIn = get<2>(tupIn);


        sem_out_id = semget(IPC_PRIVATE, 1, 0666 | IPC_CREAT);
        semctl(sem_out_id, 0, SETVAL, 0);
        auto tupOut = GetSharedMemorySegment();
        keyOut = get<0>(tupOut);
        m_SharedMemId_Out = get<1>(tupOut);
        m_SharedMemoryOut = get<2>(tupOut);

        writeLookUpFile(m_SharedMemId_In,m_SharedMemId_Out,sem_in_id,sem_out_id);
    }

    /*
     * We use a semaphore to signal between processes.
     * The semaphore starts with a value of 0.
     * Some producer writes to our inbox (shared memory).
     * The producer increments the semaphore when finished.
     * This process waits for the semaphore to become positive before invoke() is called.
     */
    void listen(){
        uint64 count = 1;
        while( isRunning) {
            // Wait for process finish writing
            semop(sem_in_id, nullptr, -1);
            Invoke();
            cv::imshow("result " + std::to_string(count), postProcessedMat);
            cv::waitKey(0);
            semop(sem_in_id, nullptr, 1);
            count++;
        }
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

        semctl(sem_in_id, 0, IPC_RMID);
        semctl(sem_out_id  , 0, IPC_RMID);

        shmdt(m_SharedMemoryOut);
        shmctl(m_SharedMemId_Out, IPC_RMID, nullptr);

        utils::fs::RemoveFile(m_lookup);
    }

    ~MonitorSharedMem(){
        OnDestroy();
    }


    int sem_in_id;
    key_t keyIn;
    I* m_SharedMemoryIn;
    int m_SharedMemId_In;


    int sem_out_id;
    key_t keyOut;
    I* m_SharedMemoryOut;
    int m_SharedMemId_Out;


    std::string m_lookup;

    void writeLookUpFile( int shmemInId,int shmemOutId, int semIn, int semOut){

        using namespace std;
        ofstream semFile(m_lookup);
        semFile << "mem_in_id=" << shmemInId << endl;
        semFile << "mem_out_id=" << shmemOutId << endl;
        semFile << "sem_in=" << semIn << endl;
        semFile << "sem_out=" << semOut << endl;
        semFile.close();


    }

    std::tuple<key_t,int,I*> GetSharedMemorySegment(){

        key_t key = ftok("/tmp", 'A');
        if (key == -1) {
            throw std::runtime_error("ftok failed");
        }

        // Create or Get Shared Memory Segment
        int shmemId = shmget(key, sizeof(I), 0666 | IPC_CREAT);
        if (shmemId == -1) {
            OnDestroy();
            throw std::runtime_error( "shmget : shared memory if  ... abort");
        }

        // Attach to the shared memory
        I* sharedMemory = (I*)shmat(shmemId, nullptr, 0);
        if (sharedMemory == (I*)-1) {
            OnDestroy();
            throw std::runtime_error("cshmat: " + std::to_string(shmemId));
        }


        return std::make_tuple(key, shmemId, sharedMemory);
    }

};