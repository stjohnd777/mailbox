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
using namespace std;

#include <sys/inotify.h>
#include <poll.h>


#define EVENT_SIZE (sizeof(struct inotify_event))
#define EVENT_BUF_LEN (1024 * (EVENT_SIZE + 16))

class MonitorDir {
public:
    std::function < void(const std::string& imgPath, const std::string& outbox)>  processImage;
public:
    MonitorDir(string inbox, string outbox,std::function < void(const std::string& imgPath, const std::string& outbox)>  processImage ) : inbox(inbox), outbox(outbox), processImage(processImage) {
        inotifyFd = inotify_init();
        if (inotifyFd == -1) throw  "inotify_init failed";

        watchDescriptor = inotify_add_watch(inotifyFd, inbox.c_str(), IN_CREATE | IN_MOVED_TO | IN_CLOSE_WRITE);
        if (watchDescriptor == -1) throw "inotify_add_watch" ;
    }
    void Start(){
        while (!isRunning) {
            processEvent();
        }
    }

    ~MonitorDir(){
        isRunning = false;
        inotify_rm_watch(inotifyFd, watchDescriptor);
        close(inotifyFd);
    }
private:

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
                    if (event->mask & (IN_CREATE | IN_MOVED_TO | IN_CLOSE_WRITE)) {
                        if (event->mask & IN_CREATE) {
                            std::cout << "File created: " << event->name << " wait for IN_CLOSE_WRITE " << std::endl;
                            processImage(imagePath,outbox);
                        }
                        if (event->mask & IN_MOVED_TO) {
                            std::cout << "IN_MOVED_TO: File moved into inbox: " << event->name << std::endl;
                            processImage(imagePath,outbox);
                        }
                        if (event->mask & IN_CLOSE_WRITE) {
                            std::cout << "IN_CLOSE_WRITE: File in write mode transitioned to closed: " << event->name << std::endl;
                            processImage(imagePath,outbox);
                        }
                    }
                }

                i += EVENT_SIZE + event->len;
            }
        }
    }

    bool isRunning;
    int inotifyFd ;
    int watchDescriptor;
    string inbox;
    string outbox;

//	size_t EVENT_SIZE    =  sizeof(struct inotify_event);
//	size_t EVENT_BUF_LEN =  1024 * (EVENT_SIZE + 16);
};


