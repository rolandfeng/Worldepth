//
// Created by Soren Dahl on 12/2/18.
//

#include "Converter.h"

namespace SLAM
{
    std::vector<cv::Mat> Converter::toDescriptorVector(const cv::Mat &Descriptors)
    {
        std::vector<cv::Mat> vDesc;
        vDesc.reserve(Descriptors.rows);
        for (int j=0;j<Descriptors.rows;j++)
            vDesc.push_back(Descriptors.row(j));

        return vDesc;
    }
}