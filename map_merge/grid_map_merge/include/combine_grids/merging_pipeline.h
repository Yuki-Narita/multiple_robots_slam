/*********************************************************************
 *
 * Software License Agreement (BSD License)
 *
 *  Copyright (c) 2015-2016, Jiri Horner.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials provided
 *     with the distribution.
 *   * Neither the name of the Jiri Horner nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 *  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 *  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 *  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 *  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *
 *********************************************************************/

#ifndef MERGING_PIPELINE_H_
#define MERGING_PIPELINE_H_

#include <vector>

#include <geometry_msgs/Transform.h>
#include <nav_msgs/OccupancyGrid.h>

#include <opencv2/core/utility.hpp>

namespace combine_grids
{
enum class FeatureType { AKAZE, ORB, SURF };

/**
 * @brief Pipeline for merging overlapping occupancy grids
 * @details Pipeline works on internally stored grids.
 */
class MergingPipeline
{
public:
  template <typename InputIt>
  bool feed(InputIt grids_begin, InputIt grids_end);
  bool estimateTransforms(FeatureType feature = FeatureType::AKAZE,
                          double confidence = 1.0);
  nav_msgs::OccupancyGrid::Ptr composeGrids(int map_num, bool errorAvoidance);

  std::vector<geometry_msgs::Transform> getTransforms() const;
  template <typename InputIt>
  bool setTransforms(InputIt transforms_begin, InputIt transforms_end);

private:
  std::vector<nav_msgs::OccupancyGrid::ConstPtr> grids_;
  std::vector<cv::Mat> images_;
  std::vector<cv::Mat> transforms_;
  
  std::vector<int> mapOrder;

  void fixRois(std::vector<cv::Rect>& rois, std::vector<cv::Mat>& transforms, int originNum);
};

template <typename InputIt>
bool MergingPipeline::feed(InputIt grids_begin, InputIt grids_end)
{
  static_assert(std::is_assignable<nav_msgs::OccupancyGrid::ConstPtr&,
                                   decltype(*grids_begin)>::value,
                "grids_begin must point to nav_msgs::OccupancyGrid::ConstPtr "
                "data");

  // we can't reserve anything, because we want to support just InputIt and
  // their guarantee validity for only single-pass algos
  images_.clear();
  grids_.clear();
  mapOrder.clear();
  bool errorAvoidance = false;
  //std::cout << "test20" << std::endl;
  //std::cout << "grid_size1 : " << grids_.size() << "\n";
  for (InputIt it = grids_begin; it != grids_end; ++it) {
    //std::cout << "test21" << std::endl;
    if (*it && !(*it)->data.empty()) {
      //std::cout << "test22" << std::endl;
      grids_.push_back(*it);
      //std::cout << "test23" << std::endl;
      //std::cout << "grids_" << (*it) -> info << '\n';
      /* convert to opencv images. it creates only a view for opencv and does
       * not copy or own actual data. */
      images_.emplace_back((*it)->info.height, (*it)->info.width, CV_8UC1,
                           const_cast<signed char*>((*it)->data.data()));
      //std::cout << "test24" << std::endl;
    } else {
      //std::cout << "test25" << std::endl;
      grids_.emplace_back();
      //std::cout << "test26" << std::endl;
      images_.emplace_back();
      //std::cout << "test27" << std::endl;
      errorAvoidance = true;
    }
  }
  //std::cout << "grid_size2 : " << grids_.size() << "\n";
  //std::cout << "grids_ << " << grids_[0] << std::endl;

  if(!errorAvoidance)
  {
    for(int i=0;i<grids_.size();i++)
    {
      //std::cout << "test28" << std::endl;
      //std::cout << "****grids frame ******\n" << grids_[i] -> header.frame_id << std::endl;
      //std::cout << "grid_size : " << grids_.size() << "\n";
      //std::cout << "i : " << i << "\n";
      //std::cout << grids_[i] -> header.frame_id << "\n";
      std::string num = grids_[i] -> header.frame_id;
      //std::cout << "test29" << std::endl;
      //std::cout << "a"  << std::endl;
      char numPick = num[6];
      //std::cout << "test30" << std::endl;
      //std::cout << "b"  << std::endl;
      mapOrder.push_back((int)(numPick - '0'));
      //std::cout << "test31" << std::endl;
      //std::cout << "c"  << std::endl;
      //std::cout << "order_num\n" << mapOrder[i] << std::endl;
      //std::cout << "d"  << std::endl;
    }
  }

  return errorAvoidance;
}

template <typename InputIt>
bool MergingPipeline::setTransforms(InputIt transforms_begin,
                                    InputIt transforms_end)
{
  static_assert(std::is_assignable<geometry_msgs::Transform&,
                                   decltype(*transforms_begin)>::value,
                "transforms_begin must point to geometry_msgs::Transform "
                "data");

  decltype(transforms_) transforms_buf;
  for (InputIt it = transforms_begin; it != transforms_end; ++it) {
    const geometry_msgs::Quaternion& q = it->rotation;
    if ((q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w) <
        std::numeric_limits<double>::epsilon()) {
      // represents invalid transform
      //std::cout << "unknown point" << "\n";
      transforms_buf.emplace_back();
      continue;
    }
    double s = 2.0 / (q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w);
    double a = 1 - q.y * q.y * s - q.z * q.z * s;
    double b = q.x * q.y * s + q.z * q.w * s;
    double tx = it->translation.x;
    double ty = it->translation.y;

    //std::cout << "tx : " << tx << "ty : " << ty << "\n";

    cv::Mat transform = cv::Mat::eye(3, 3, CV_64F);
    transform.at<double>(0, 0) = transform.at<double>(1, 1) = a;
    transform.at<double>(1, 0) = b;
    transform.at<double>(0, 1) = -b;
    transform.at<double>(0, 2) = tx;
    transform.at<double>(1, 2) = ty;

    transforms_buf.emplace_back(std::move(transform));
  }

  if (transforms_buf.size() != images_.size()) {
    //std::cout << "return false" << "\n";
    return false;
  }
  for(int i=0;i<transforms_buf.size();i++)
  {
    //std::cout << transforms_buf[i] << "\n";
  }

  std::swap(transforms_, transforms_buf);

  for(int i=0;i<transforms_.size();i++)
  {
    //std::cout << transforms_[i] << "\n";
  }
  //std::cout << "return true" << "\n";
  return true;
}

}  // namespace combine_grids

#endif  // MERGING_PIPELINE_H_
