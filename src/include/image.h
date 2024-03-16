//
// Created by overman on 3/4/2024.
//
#pragma once
#include <opencv2/core.hpp>
#include <memory>
#include <exception>

#include "macros.h"

template<typename T, size_t COLS=IMAGE_WIDTH, size_t ROWS=IMAGE_HEIGHT >
struct ImageGray {
    const T atom = 1;
    const size_t cols = COLS;
    const size_t rows = ROWS;
    T pixels[ROWS][COLS];

    // Default constructor
    ImageGray() = default;

    // Move constructor
    ImageGray(ImageGray&& other) noexcept
            : atom(std::move(other.atom)), cols(other.cols), rows(other.rows) {
        std::memcpy(pixels, other.pixels, sizeof(pixels));
    }

    // Delete the copy constructor
    ImageGray(const ImageGray&) = delete;
};

typedef ImageGray<uint8_t , IMAGE_WIDTH, IMAGE_HEIGHT> ImageGray8;
typedef ImageGray<uint16_t, IMAGE_WIDTH, IMAGE_HEIGHT> ImageGray16;

cv::Mat ImageToMat(const ImageGray8& img) {
    cv::Mat mat(IMAGE_HEIGHT, IMAGE_WIDTH, CV_8U);
    for (int row = 0; row < img.rows; ++row) {
        for (int col = 0; col < img.cols; ++col) {
            mat.at<uint8_t>(row, col) = img.pixels[row][col];
        }
    }
    return mat;
}
cv::Mat ImageToMat(const ImageGray16& img) {
    cv::Mat mat(IMAGE_HEIGHT, IMAGE_WIDTH, CV_16U);
    for (int row = 0; row < img.rows; ++row) {
        for (int col = 0; col < img.cols; ++col) {
            mat.at<uint16_t>(row, col) = img.pixels[row][col];
        }
    }
    return mat;
}


std::shared_ptr<ImageGray16> MatToImage16U(const cv::Mat mat) {
    std::shared_ptr<ImageGray16> outputImage = std::make_shared<ImageGray16>();
    if (mat.empty() || mat.type() != CV_16U) {
        throw std::runtime_error( "image empty of not CV_8U");
    }
    if (mat.cols != outputImage->cols || mat.rows != outputImage->rows) {
        throw std::runtime_error("image wrong size");
    }
    for (int row = 0; row < outputImage->rows; ++row) {
        for (int col = 0; col < outputImage->cols; ++col) {
            outputImage->pixels[row][col] = mat.at<uint16_t>(row, col);
        }
    }
    return outputImage;
}

std::shared_ptr<ImageGray8> MatToImage8U(const cv::Mat mat) {
    std::shared_ptr<ImageGray8> outputImage = std::make_shared<ImageGray8>();
    if (mat.empty() || mat.type() != CV_8U) {
        throw std::runtime_error( "image empty of not CV_8U");
    }
    if (mat.cols != outputImage->cols || mat.rows != outputImage->rows) {
        throw std::runtime_error("image wrong size");
    }
    for (int row = 0; row < outputImage->rows; ++row) {
        for (int col = 0; col < outputImage->cols; ++col) {
            outputImage->pixels[row][col] = mat.at<uint8_t>(row, col);
        }
    }
    return outputImage;
}

template<typename I>
std::shared_ptr<I> MatToImage(const cv::Mat mat) {
    std::shared_ptr<I> outputImage = std::make_shared<I>();

    if (mat.cols != outputImage->cols || mat.rows != outputImage->rows) {
        throw std::runtime_error("image wrong size");
    }
    auto pixelSize = sizeof (outputImage->atom)  ;
    for (int row = 0; row < outputImage->rows; ++row) {
        for (int col = 0; col < outputImage->cols; ++col) {
            if ( pixelSize == 1){
                outputImage->pixels[row][col] = mat.at<uint8_t>(row, col);
            }else {
                outputImage->pixels[row][col] = mat.at<uint16_t>(row, col);
            }
        }
    }
    return outputImage;
}