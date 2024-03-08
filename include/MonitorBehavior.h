//
// Created by e438262 on 3/4/2024.
//
#pragma once

#include <opencv2/core.hpp>
#include <nlohmann/json.hpp>
using namespace nlohmann;

template<typename IN = cv::Mat, typename OUT = cv::Mat>
class MonitorBehavior {

    virtual void OnInit(const json &j) {

    }
    virtual OUT OnPreProcess(IN mat) {

    }

    virtual OUT Process(IN img){
    }

    virtual OUT OnPostProcess(IN mat) = 0;


    virtual void  OnDisable() {

    }
    virtual void  OnEnable() {
    }

    virtual void OnDestroy() {
    }

    std::function < OUT (const IN in )>  processImage;
};

