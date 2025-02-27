/*
 * Copyright (C) 2019 Open Source Robotics Foundation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include <cmath>
#include <tf2/impl/utils.h>

#include "free_fleet/ros2/utilities.hpp"

#include <tf2/LinearMath/Matrix3x3.h>
#include <tf2_geometry_msgs/tf2_geometry_msgs.h>
#include <geometry_msgs/msg/quaternion.hpp>

namespace free_fleet
{
namespace ros2
{

double get_yaw_from_pose(
    const geometry_msgs::msg::PoseStamped& _pose_stamped)
{
  return tf2::impl::getYaw(
      tf2::impl::toQuaternion(_pose_stamped.pose.orientation));
}

geometry_msgs::msg::Quaternion get_quat_from_yaw(double _yaw)
{
  tf2::Quaternion quat_tf;
  quat_tf.setRPY(0.0, 0.0, _yaw);
  quat_tf.normalize();

  geometry_msgs::msg::Quaternion quat = tf2::toMsg(quat_tf);
  return quat;
}

bool is_pose_close(
    const geometry_msgs::msg::PoseStamped& _first,
    const geometry_msgs::msg::PoseStamped& _second)
{
  if (_first.header.frame_id != _second.header.frame_id)
    return false;


  double elapsed_sec = (rclcpp::Time(_second.header.stamp) - _first.header.stamp).seconds();
  double distance = hypot(_second.pose.position.x - _first.pose.position.x,
    _second.pose.position.y - _first.pose.position.y,
    _second.pose.position.z - _first.pose.position.z);
  double speed = abs(distance / elapsed_sec);
  if (speed > 0.01)
    return false;

  double first_yaw = get_yaw_from_pose(_first);
  double second_yaw = get_yaw_from_pose(_second);
  double turning_speed = abs((second_yaw - first_yaw) / elapsed_sec);
  if (turning_speed > 0.01)
    return false;

  return true;
}

double toGPS_longitude(double x, double long_origin_, double lat_origin_){
  return x/mdeglon(lat_origin_) + long_origin_;
}

double toGPS_latitude(double y, double long_origin_, double lat_origin_){
  return y/mdeglat(lat_origin_) + lat_origin_;
}

double toUTM_x(double longitude, double long_origin_, double lat_origin_){
  return (longitude - long_origin_) * mdeglon(lat_origin_);
}
double toUTM_y(double latitude, double long_origin_, double lat_origin_){
  return (latitude - lat_origin_) * mdeglat(lat_origin_);
}

double mdeglat(double lat){
    double latrad = lat*2.0*M_PI/360.0 ;
    double dy = - 111132.09 - 566.05 * cos(2.0*latrad) \
         + 1.20 * cos(4.0*latrad) \
         - 0.002 * cos(6.0*latrad);
    return dy;
}

double mdeglon(double lat){
    double latrad = lat*2.0*M_PI/360.0 ;
    double dx = - 111415.13 * cos(latrad) \
         - 94.55 * cos(3.0*latrad) \
	     + 0.12 * cos(5.0*latrad);
    return dx;
}


} // namespace ros2
} // namespace free_fleet