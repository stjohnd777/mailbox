#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <chrono>
#include <ctime>
#include <sstream>
#include <fstream>
#include <exception>
using namespace std;

#include <sys/inotify.h>
#include <poll.h>

#include <filesystem>
namespace fs = std::filesystem;

#include <opencv2/opencv.hpp>

#define EVENT_SIZE (sizeof(struct inotify_event))
#define EVENT_BUF_LEN (1024 * (EVENT_SIZE + 16))

std::string inbox = "/data/inbox";
std::string outbox = "/data/outbox";
std::string errbox = "/data/errbox";
std::string cmdbox = "/data/errbox";

void RemoveFile(const string& src){
    if (fs::exists(src)) {
        fs::remove(src);
        std::cout << "File '" << src << "' successfully processed and removed." << std::endl;
    }
}
void MoveFile(const string& src, const string& dst){
    fs::rename(src, dst);
}
void CreatErrorFile(const string& timeString, const string& imgPath,  const std::exception& e){
    std::stringstream ss;
    ss << errbox << "/" << timeString << "-error-" << imgPath << ".txt";
    const std::string fileName = ss.str();

    std::ofstream outputFile(fileName);
    outputFile << e.what() << std::endl;
    outputFile.close();
}

std::string GetCurrentTime(){
    auto currentTimePoint = std::chrono::system_clock::now();
    std::time_t currentTime = std::chrono::system_clock::to_time_t(currentTimePoint);
    char timeString[100];
    std::strftime(timeString, sizeof(timeString), "%Y-%m-%d.%H:%M:%S", std::localtime(&currentTime));
    return timeString;
}

void processImage(const std::string& imgPath, const std::string& outbox) {

    fs::path filePath = imgPath;
    fs::path fileName = filePath.filename();

    auto timeString = GetCurrentTime();
    std::cout << "processed "<< fileName << " @ timestamp: " << timeString << std::endl;

    try {
        cv::Mat image = cv::imread(imgPath, cv::IMREAD_UNCHANGED);

        std::stringstream ss;
        ss << outbox << "/" << timeString << "-" << fileName << "-processed" << ".png";
        cv::imwrite(ss.str(), image);

        RemoveFile(imgPath);


    }catch(const std::exception& e){
        std::stringstream ss0;
        ss0 << errbox << "/" << timeString << "-" << fileName ;
        fs::rename(imgPath, ss0.str());
        CreatErrorFile(timeString,imgPath,e);
    }
}



void processEvent(int fd, const char* directoryToWatch, std::string outbox) {

    char buffer[EVENT_BUF_LEN];

    // Use poll to wait until there are events to be processed
    struct pollfd fds[1];
    fds[0].fd = fd;
    fds[0].events = POLLIN;

    int pollResult = poll(fds, 1, -1);  // Wait indefinitely until an event occurs
    if (pollResult == -1) {
        perror("poll");
        exit(EXIT_FAILURE);
    }

    if (pollResult > 0) {
        ssize_t bytesRead = read(fd, buffer, EVENT_BUF_LEN);
        if (bytesRead < 0) {
            perror("read");
            exit(EXIT_FAILURE);
        }

        for (ssize_t i = 0; i < bytesRead; ) {

            struct inotify_event *event = reinterpret_cast<struct inotify_event*>(&buffer[i]);

            std::string imagePath = std::string(directoryToWatch) + "/" + event->name;
            if (event->len > 0) {

                if (event->mask & (IN_CREATE | IN_MOVED_TO | IN_CLOSE_WRITE)) {

                    // NOT DRY ... TODO: DRY OUT
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

#include <getopt.h>



void handleCommandLineOptions(int argc, char** argv){
    std::string processing;
    const char* shortOptions = "i:o:e:c:o:p";
    int option;
    while ((option = getopt(argc, argv, shortOptions)) != -1) {
        switch (option) {
            case 'p':
                processing = optarg;
                break;
            case 'i':
                inbox = optarg;
                break;
            case 'o':
                outbox = optarg;
                break;
            case 'e':
                errbox = optarg;
                break;
            case 'c':
                cmdbox = optarg;
                break;
            case '?':
                // Handle unrecognized options or missing arguments
                std::cerr << "Usage: " << argv[0] << " -i <inbox>  -o <outbox>  -c <cmdbox> -p <processing> " << std::endl;
                //return 1; // Return an error code
        }
    }
}

/**
 * Shuts the system down by polling a cmd directoru
 * for a shutdown file, if the file 'shutdown.cmd is
 * present it stop the main thread
 * @return
 */
bool isShutDown(){
    bool isShutdown = false;
    const std::string fileName = "shutdown.cmd";
    fs::path filePath = cmdbox + fs::path::preferred_separator + fileName;
    if (fs::exists(filePath)) {
        std::cout << "Shutdown command file found. Initiating shutdown..." << std::endl;
        isShutdown = true;
    }
    return isShutdown;
}

void processCmd (){

}


int main(int argc, char** argv) {

    //handleCommandLineOptions(argc,argv);

    int inotifyFd = inotify_init();
    if (inotifyFd == -1) {
        perror("inotify_init");
        return EXIT_FAILURE;
    }

    int watchDescriptor = inotify_add_watch(inotifyFd, inbox.c_str(), IN_CREATE | IN_MOVED_TO | IN_CLOSE_WRITE);
    if (watchDescriptor == -1) {
        perror("inotify_add_watch");
        close(inotifyFd);
        return EXIT_FAILURE;
    }

    // process any files in the directory when you start, this can happen
    // if the system is not started and production on another system has commenced,
    // or the was some failure, at any rate this a fault-tolerant
    for (const auto& entry : fs::directory_iterator(inbox)) {
        std::cout << entry.path() << std::endl;
        processImage(entry.path(),outbox);
    }


    std::cout << "Monitoring directory: " << inbox << std::endl;
    while (!isShutDown()) {
        processEvent(inotifyFd, inbox.c_str(), outbox);
        processCmd();
    }

    // Cleanup
    inotify_rm_watch(inotifyFd, watchDescriptor);
    close(inotifyFd);

    return EXIT_SUCCESS;
}