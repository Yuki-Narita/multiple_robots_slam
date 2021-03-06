#ifndef CONSTRUCT_H
#define CONSTRUCT_H

#include <memory>
#include <vector>
#include <Eigen/Core>

// 前方宣言

/// my packages
namespace exploration_msgs{
    template <class ContainerAllocator>
    struct Branch_;
    typedef ::exploration_msgs::Branch_<std::allocator<void>> Branch;
    template <class ContainerAllocator>
    struct Frontier_;
    typedef ::exploration_msgs::Frontier_<std::allocator<void>> Frontier;
    template <class ContainerAllocator>
    struct RobotInfo_;
    typedef ::exploration_msgs::RobotInfo_<std::allocator<void>> RobotInfo;
}
/// rosmsgs
namespace geometry_msgs{
    template <class ContainerAllocator>
    struct Point_;
    typedef ::geometry_msgs::Point_<std::allocator<void>> Point;
    template <class ContainerAllocator>
    struct Pose_;
    typedef ::geometry_msgs::Pose_<std::allocator<void>> Pose;
    template <class ContainerAllocator>
    struct Quaternion_;
    typedef ::geometry_msgs::Quaternion_<std::allocator<void>> Quaternion;
    template <class ContainerAllocator>
    struct Twist_;
    typedef ::geometry_msgs::Twist_<std::allocator<void>> Twist; 
    template <class ContainerAllocator>
    struct Vector3_;
    typedef ::geometry_msgs::Vector3_<std::allocator<void>> Vector3;
}
namespace std_msgs{
    template <class ContainerAllocator>
    struct Bool_;
    typedef ::std_msgs::Bool_<std::allocator<void>> Bool;
    template <class ContainerAllocator>
    struct Float64_;
    typedef ::std_msgs::Float64_<std::allocator<void>> Float64;
    template <class ContainerAllocator>
    struct Int8_;
    typedef ::std_msgs::Int8_<std::allocator<void>> Int8;
    template <class ContainerAllocator>
    struct Int32_;
    typedef ::std_msgs::Int32_<std::allocator<void>> Int32;
    template <class ContainerAllocator>
    struct Twist_;
    typedef ::std_msgs::Twist_<std::allocator<void>> Twist; 
}
namespace visualization_msgs{
    template <class ContainerAllocator>
    struct Marker_;
    typedef ::visualization_msgs::Marker_<std::allocator<void>> Marker;
}
/// others
namespace pcl{
    struct PointXYZRGB;
}
// 前方宣言ここまで

namespace ExpLib{
    namespace Construct{
        visualization_msgs::Marker msgMarker(const std::string& frame_id, double scale=0.5, float r=1.0, float g=0.0, float b=0.0, float a=1.0, int type = 6);
        geometry_msgs::Point msgPoint(double x=0,double y=0,double z=0);
        geometry_msgs::Pose msgPose(const geometry_msgs::Point& p, const geometry_msgs::Quaternion& q);
        geometry_msgs::Vector3 msgVector(double x=0,double y=0,double z=0);
        geometry_msgs::Twist msgTwist(double x=0,double z=0);
        std_msgs::Bool msgBool(bool b=true);
        std_msgs::Float64 msgDouble(double d);
        std_msgs::Int32 msgInt(int i);
        std_msgs::Int8 msgInt8(int i);
        exploration_msgs::Frontier msgFrontier(const geometry_msgs::Point& p, double a, const geometry_msgs::Vector3& v, double c);
        exploration_msgs::Branch msgBranch(const geometry_msgs::Point& p);
        pcl::PointXYZRGB pclXYZRGB(float x,float y,float z,float r,float g,float b);
        Eigen::Matrix2d eigenMat2d(double a,double b,double c,double d);
        exploration_msgs::RobotInfo msgRobotInfo(const std::string& n, const geometry_msgs::Pose& p);
        geometry_msgs::Quaternion msgGeoQuaternion(double x, double y, double z, double w);
        template <typename T>
        std::vector<T> oneFactorVector(const T& factor){
            std::vector<T> v;
            v.push_back(factor);
            return v;
        }
    }
}
#endif // CONSTRUCT_H
