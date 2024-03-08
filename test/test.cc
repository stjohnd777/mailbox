#include <gtest/gtest.h>

#include <string>
#include <sstream>
#include <memory>

#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

#include <nlohmann/json.hpp>
using namespace nlohmann;

#include "image.h"
#include "utils.h"
#include "macros.h"


// Demonstrate some basic assertions.
TEST(HelloTest, BasicAssertions) {
// Expect two strings not to be equal.
EXPECT_STRNE("hello", "world");
// Expect equality.
EXPECT_EQ(7 * 6, 42);
}


TEST(Transforms, RndTripImg8) {

    std::string path = "/data/sun.jpg";
    if ( ! utils::fs::FileExist(path)){
        throw std::runtime_error("file dne " + path);
    }
    cv::Mat raw = cv::imread(path,cv::IMREAD_GRAYSCALE);
    if ( raw.empty()){
        throw std::runtime_error("image empty");
    }

    cv::Mat mat = utils::vision::GetCvMAtAsGray8(path, 800, 600);
    cv::imshow("src",mat);

    std::shared_ptr<ImageGray8>  img = MatToImage8U( mat);
    auto mat2 = ImageToMat(*img);

    cv::imshow("transform",mat2);


    std::shared_ptr<ImageGray8> img2 = MatToImage<ImageGray8> (mat);
    auto mat3 = ImageToMat(*img2);

    cv::Mat diff;
    cv::absdiff(mat,mat2,diff);
    cv::imshow("diff",diff);

    cv::waitKey(0);
}