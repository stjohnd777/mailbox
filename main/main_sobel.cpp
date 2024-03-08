//
// Created by overman on 3/1/2024.
//
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include <iostream>
#include <sstream>
using namespace cv;
using namespace std;



cv::Mat Edge(cv::Mat mat, int ksize =-1,int ddepth = CV_16S, int scale =1, int delta =0){
    Mat src= mat.clone();
    GaussianBlur(src, src, Size(3, 3), 0, 0, BORDER_DEFAULT);// Remove noise by blurring with a Gaussian filter ( kernel size = 3 )
    cvtColor(src, src, COLOR_BGR2GRAY);
    Mat grad_x;
    Sobel(src, grad_x, ddepth, 1, 0,  ksize, scale, delta, BORDER_DEFAULT);
    Mat grad_y;
    Sobel(src, grad_y,  ddepth, 0, 1, ksize, scale, delta, BORDER_DEFAULT);
    Mat abs_grad_x, abs_grad_y;
    convertScaleAbs(grad_x, abs_grad_x);
    convertScaleAbs(grad_y, abs_grad_y);
    addWeighted(abs_grad_x, 0.5, abs_grad_y, 0.5, 0, src);
    return src;
}


int main( int argc, char** argv )
{
    cv::CommandLineParser parser(argc, argv,
                                 "{@input   |/data/pia23810.jpg|input image}"
                                 "{ksize   k|1|ksize (hit 'K' to increase its value at run time)}"
                                 "{scale   s|1|scale (hit 'S' to increase its value at run time)}"
                                 "{delta   d|0|delta (hit 'D' to increase its value at run time)}"
                                 "{help    h|false|show help message}");
    cout << "The sample uses Sobel or Scharr OpenCV functions for edge detection\n\n";
    parser.printMessage();
    cout << "\nPress 'ESC' to exit program.\nPress 'R' to reset values ( ksize will be -1 equal to Scharr function )";
    // First we declare the variables we are going to use

    Mat grad;

    int ksize = parser.get<int>("ksize");
    int scale = parser.get<int>("scale");
    int delta = parser.get<int>("delta");
    int ddepth = CV_16S;

    Mat image = imread( "/data/pia23810.jpg", IMREAD_COLOR );   if( image.empty() ) return EXIT_FAILURE;

    imshow ("me" ,Edge(image));
    waitKey(0);

    for (;;)
    {

        // blur
        Mat blurImage ;
        GaussianBlur(image, blurImage, Size(3, 3), 0, 0, BORDER_DEFAULT);// Remove noise by blurring with a Gaussian filter ( kernel size = 3 )

        // gray
        Mat grayBlurImage;
        cvtColor(blurImage, grayBlurImage, COLOR_BGR2GRAY);        // Convert the image to grayscale

        // sobel on x then y with kernel size kSize,
//        src	    input image.
//        dst	    output image of the same size and the same number of channels as src .
//        ddepth	output image depth,
//                  see combinations; in the case of 8-bit input images it will result in truncated derivatives.
//        dx	    order of the derivative x.
//        dy	    order of the derivative y.
//        ksize	    size of the extended Sobel kernel; it must be 1, 3, 5, or 7.
//        scale	    optional scale factor for the computed derivative values; by default, no scaling is applied (see getDerivKernels for details).
//        delta	    optional delta value that is added to the results prior to storing them in dst.
        Mat grad_x;
        Sobel(grayBlurImage,
              grad_x,
              ddepth,
              1, 0,
              ksize,
              scale,
              delta,
              BORDER_DEFAULT);
        Mat grad_y;
        Sobel(grayBlurImage,
              grad_y,
              ddepth,
              0, 1,
              ksize,
              scale,
              delta,
              BORDER_DEFAULT);

        // converting back to CV_8U
        Mat abs_grad_x, abs_grad_y;
        convertScaleAbs(grad_x, abs_grad_x);
        convertScaleAbs(grad_y, abs_grad_y);


        addWeighted(abs_grad_x, 0.5, abs_grad_y, 0.5, 0, grad);
        stringstream ss;
        ss << "sobel:" << " kSize:" << ksize << " scale:" << scale << " delta" << delta;
        const String window_name = ss.str();
        imshow(window_name, grad);
        char key = (char)waitKey(0);
        if(key == 27)
        {
            return EXIT_SUCCESS;
        }
        if (key == 'k' || key == 'K')
        {
            ksize = ksize < 30 ? ksize+2 : -1;
        }
        if (key == 's' || key == 'S')
        {
            scale++;
        }
        if (key == 'd' || key == 'D')
        {
            delta++;
        }
        if (key == 'r' || key == 'R')
        {
            scale =  1;
            ksize = -1;
            delta =  0;
        }
    }
    return EXIT_SUCCESS;
}