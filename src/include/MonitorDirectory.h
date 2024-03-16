//
// Created by overman on 3/1/24.
//

#pragma once

#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <chrono>
#include <ctime>
#include <sstream>
#include <fstream>
#include <exception>
#include <getopt.h>
#include <exception>
#include <functional>
#include <utility>
#include "utils.h"
#include <sys/inotify.h>
#include <poll.h>

#include <boost/log/trivial.hpp>
using namespace std;

#include <nlohmann/json.hpp>
using namespace nlohmann;

#define EVENT_SIZE (sizeof(struct inotify_event))
#define EVENT_BUF_LEN (1024 * (EVENT_SIZE + 16))

class InitException : public std::exception{
public:
    explicit InitException(string  msg) : msg(std::move(msg)){

    }
    const char * what () {
        return msg.c_str();
    }
private :
    string msg;
};

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

class MonitorDirectory {

public:
    MonitorDirectory(
            const string& inbox,
            const string& outbox,
            std::function < cv::Mat(const std::string& imgPath)>  processImage ) : inbox(inbox), outbox(outbox), processImage(std::move(processImage)) {

        // OnInit
        if( ! fs::exists(inbox) ) {
            auto msg = inbox + " does not exist";
            throw  InitException(msg);
        }
        if( ! fs::exists(outbox) ) {
            auto msg = outbox + " does not exist";
            throw  InitException(msg);
        }

        inotifyFd = inotify_init();
        if (inotifyFd == -1) {
            auto msg =  "inotify_init failed";
            throw InitException(msg);
        }

        watchDescriptor = inotify_add_watch(inotifyFd, inbox.c_str(), IN_CREATE | IN_MOVED_TO | IN_CLOSE_WRITE);
        if (watchDescriptor == -1){
            auto msg =  outbox + " inotify_add_watch failed, permissions?";
            throw  InitException( msg);
        }
    }

    void Start(){
        isRunning = true;
        while (isRunning) {
            processEvent();
        }
    }

    void Stop(){
        isRunning = false;
    }

    ~MonitorDirectory(){
        isRunning = false;
        inotify_rm_watch(inotifyFd, watchDescriptor);
        close(inotifyFd);
    }
private:

    std::function < cv::Mat (const std::string& imgPath )>  processImage;

    void processEvent() {
        char buffer[EVENT_BUF_LEN];
        // Use poll to wait until there are events to be processed
        struct pollfd fds[1];
        fds[0].fd = inotifyFd;
        fds[0].events = POLLIN;
        int pollResult = poll(fds, 1, -1);  // Wait indefinitely until an event occurs
        if (pollResult == -1) {
            perror("poll");
            exit(EXIT_FAILURE);
        }
        if (pollResult > 0) {
            ssize_t bytesRead = read(inotifyFd, buffer, EVENT_BUF_LEN);
            if (bytesRead < 0) {
                perror("read");
                exit(EXIT_FAILURE);
            }

            for (ssize_t i = 0; i < bytesRead; ) {
                struct inotify_event *event = reinterpret_cast<struct inotify_event*>(&buffer[i]);

                std::string imagePath = inbox + "/" + event->name;


                if (event->len > 0) {
                    cv::Mat ret;
                    if (event->mask & (IN_CREATE | IN_MOVED_TO | IN_CLOSE_WRITE)) {
                        if (event->mask & IN_CREATE) {
                            std::cout << "File created: " << event->name << " wait for IN_CLOSE_WRITE " << std::endl;
                            ret = processImage(imagePath);
                        }
                        if (event->mask & IN_MOVED_TO) {
                            std::cout << "IN_MOVED_TO: File moved into inbox: " << event->name << std::endl;
                            ret = processImage(imagePath);
                        }
                        if (event->mask & IN_CLOSE_WRITE) {
                            std::cout << "IN_CLOSE_WRITE: File in write mode transitioned to closed: " << event->name << std::endl;
                            ret = processImage(imagePath);
                        }
                    }

                    RemoveProcessedFile(imagePath);


                    this->WriteToOutbox(ret);
                }

                i += EVENT_SIZE + event->len;
            }
        }
    }

    static void RemoveProcessedFile(std::string imagePath){
        if(fs::exists(imagePath) ) {
            utils::fs::RemoveFile(imagePath);
        }
    }

    void WriteToOutbox(cv::Mat mat){
        if( ! mat.empty() ) {
            std::stringstream ss;
            string timeString = utils::GetCurrentTime();
            ss << outbox << "/" << timeString << "-processed" << ".png";
            cv::imwrite(ss.str(), mat);
        }
    }

    bool isRunning = false;
    int inotifyFd ;
    int watchDescriptor;
    string inbox;
    string outbox;
};




