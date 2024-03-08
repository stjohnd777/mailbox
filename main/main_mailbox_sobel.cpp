#include <iostream>
#include <cstdlib>
#include <exception>

#include "MonitorDirectory.h"
#include "utils.h"

#include <opencv2/opencv.hpp>
using namespace std;

#include <filesystem>
namespace fs = std::filesystem;

#include <boost/log/trivial.hpp>


#define EVENT_SIZE (sizeof(struct inotify_event))
#define EVENT_BUF_LEN (1024 * (EVENT_SIZE + 16))

struct MailboxParams {
   string inbox = "/data/inbox";
   string outbox = "/data/outbox";
   string cmdbox = "/data/cmdbox";
   string errbox = "/data/errbox";
   string processing ;
};


#include <getopt.h>
MailboxParams handleCommandLineOptions(int argc, char** argv){
    MailboxParams params;

    const char* shortOptions = "i:o:e:c:o:p";
    int option;
    while ((option = getopt(argc, argv, shortOptions)) != -1) {
        switch (option) {
            case 'i':
                params.inbox = optarg;
                break;
            case 'o':
                params.outbox = optarg;
                break;
            case 'e':
                params.errbox = optarg;
                break;
            case 'c':
                params.cmdbox = optarg;
                break;
            case 'p':
                params.processing = optarg;
                break;
            case '?':
                // Handle unrecognized options or missing arguments
                std::cerr << "Usage: " << argv[0] << " -i <inbox|required>  -o <outbox|required>  -c <cmdbox> -p <processing> " << std::endl;
                //return 1; // Return an error code
        }
    }

    return params;
}


std::string inbox = "/data/inbox";
std::string outbox = "/data/outbox";

using namespace boost;
namespace logging = boost::log;
void init_logging(){

//    logging::add_file_log  (
//        keywords::file_name = "sample_%N.log",                                        1
//        keywords::rotation_size = 10 * 1024 * 1024,                                   2
//        keywords::time_based_rotation = sinks::file::rotation_at_time_point(0, 0, 0), 3
//        keywords::format = "[%TimeStamp%]: %Message%"                                 4
//    );

//    logging::core::get()->set_filter(logging::trivial::severity >= logging::trivial::info);
}



int main(int argc, char** argv) {
    // my sobel
    auto imageProcessor =  [&](const std::string& imgPath){
        cv::Mat ret;
        if ( fs::exists(imgPath) ) {
            auto image = cv::imread(imgPath, cv::IMREAD_UNCHANGED);
            if( ! image.empty() ) {
                ret= utils::vision::Edge(image);
            }
        }
        return ret;
    };
    auto params = handleCommandLineOptions(argc,argv);

    init_logging();

    auto aInbox = new MonitorDirectory( params.inbox,params.outbox, imageProcessor);

    aInbox->Start();

    return EXIT_SUCCESS;
}