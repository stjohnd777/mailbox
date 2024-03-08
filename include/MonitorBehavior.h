//
// Created by e438262 on 3/4/2024.
//
#pragma once

#include <opencv2/core.hpp>
#include <nlohmann/json.hpp>

using namespace nlohmann;

template<typename IN = cv::Mat, typename OUT = cv::Mat>
class MonitorBehavior {
public:
    virtual void OnInit( ) = 0;

    virtual void Invoke() =0 ;

    virtual OUT OnPreProcess(IN mat) = 0;

    virtual OUT Process(IN img) = 0;

    virtual OUT OnPostProcess(IN mat) = 0;

    virtual void OnDisable() = 0;

    virtual void OnEnable() = 0;

    virtual void OnDestroy() = 0;

public:
    std::function<OUT(const IN in)> processImage;
};

