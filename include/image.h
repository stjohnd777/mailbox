//
// Created by overman on 3/4/2024.
//
#pragma once
#include <opencv2/core.hpp>
#include <memory>
#include <exception>

#include "macros.h"

template<typename T, size_t width=IMAGE_WIDTH, size_t height=IMAGE_HEIGHT >
struct Image {
    T pixels[width][height];
};
typedef Image<uint8_t , IMAGE_WIDTH, IMAGE_HEIGHT> Image8U;
typedef Image<uint16_t, IMAGE_WIDTH, IMAGE_HEIGHT> Image16U;

cv::Mat ImageToMat(const Image8U& img) {
    cv::Mat mat(IMAGE_HEIGHT, IMAGE_WIDTH, CV_16U);
    for (int i = 0; i < IMAGE_WIDTH; ++i) {
        for (int j = 0; j < IMAGE_HEIGHT; ++j) {
            mat.at<uint8_t>(j, i) = img.pixels[i][j];
        }
    }
    return mat;
}

cv::Mat ImageToMat(const Image16U& img) {
    cv::Mat mat(IMAGE_HEIGHT, IMAGE_WIDTH, CV_16U);
    for (int i = 0; i < IMAGE_WIDTH; ++i) {
        for (int j = 0; j < IMAGE_HEIGHT; ++j) {
            mat.at<uint16_t>(j, i) = img.pixels[i][j];
        }
    }
    return mat;
}

std::shared_ptr<Image16U> MatToImage16U(const cv::Mat mat) {
    std::shared_ptr<Image16U> outputImage = std::make_shared<Image16U>();
    if (mat.empty() || mat.type() != CV_16U) {
        throw std::runtime_error( "image empty of not CV_16U");
    }
    if (mat.cols != IMAGE_WIDTH || mat.rows != IMAGE_HEIGHT) {
        throw std::runtime_error("image wrong size");
    }
    for (int i = 0; i < IMAGE_HEIGHT; ++i) {
        for (int j = 0; j < IMAGE_WIDTH; ++j) {
            outputImage->pixels[j][i] = mat.at<uint16_t>(i, j);
        }
    }
    return outputImage;
}

std::shared_ptr<Image8U> MatToImage8U(const cv::Mat mat) {
    std::shared_ptr<Image8U> outputImage = std::make_shared<Image8U>();
    if (mat.empty() || mat.type() != CV_8U) {
        throw std::runtime_error( "image empty of not CV_16U");
    }
    if (mat.cols != IMAGE_WIDTH || mat.rows != IMAGE_HEIGHT) {
        throw std::runtime_error("image wrong size");
    }
    for (int i = 0; i < IMAGE_HEIGHT; ++i) {
        for (int j = 0; j < IMAGE_WIDTH; ++j) {
            outputImage->pixels[j][i] = mat.at<uint16_t>(i, j);
        }
    }
    return outputImage;
}

template<typename I>
std::shared_ptr<I> MatToImage(const cv::Mat mat) {
    std::shared_ptr<I> outputImage = std::make_shared<I>();

    if (mat.cols != IMAGE_WIDTH || mat.rows != IMAGE_HEIGHT) {
        throw std::runtime_error("image wrong size");
    }
    for (int i = 0; i < IMAGE_HEIGHT; ++i) {
        for (int j = 0; j < IMAGE_WIDTH; ++j) {
            outputImage->pixels[j][i] = mat.at<uint16_t>(i, j);
        }
    }
    return outputImage;
}