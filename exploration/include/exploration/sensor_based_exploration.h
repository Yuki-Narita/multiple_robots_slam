#ifndef SENSOR_BASED_EXPLORATION_H
#define SENSOR_BASED_EXPLORATION_H

#include <memory>
#include <vector>

// 前方宣言

/// my packages
namespace ExpLib{
    namespace Struct{
        struct listStruct;
        template<typename T>
        struct pubStruct;
        template<typename T>
        struct subStruct;
    }
}
namespace exploration_msgs{
    template <class ContainerAllocator>
    struct PointArray_;
    typedef ::exploration_msgs::PointArray_<std::allocator<void>> PointArray;
    template <class ContainerAllocator>
    struct PoseStampedArray_;
    typedef ::exploration_msgs::PoseStampedArray_<std::allocator<void>> PoseStampedArray;
}
namespace exploration{
    class sensor_based_exploration_parameter_reconfigureConfig;
}
/// ros
namespace dynamic_reconfigure{
    template <class ConfigType>
    class Server;
}
/// rosmsgs
namespace geometry_msgs{
    template <class ContainerAllocator>
    struct Point_;
    typedef ::geometry_msgs::Point_<std::allocator<void>> Point;
    template <class ContainerAllocator>
    struct PointStamped_;
    typedef ::geometry_msgs::PointStamped_<std::allocator<void>> PointStamped;
    template <class ContainerAllocator>
    struct Pose_;
    typedef ::geometry_msgs::Pose_<std::allocator<void>> Pose;
    template <class ContainerAllocator>
    struct PoseStamped_;
    typedef ::geometry_msgs::PoseStamped_<std::allocator<void>> PoseStamped;
}
// 前方宣言ここまで

namespace ExStc = ExpLib::Struct;

class SensorBasedExploration{
    private:
        // static parameters
        bool OUTPUT_SBE_PARAMETERS;
        std::string SBE_PARAMETER_FILE_PATH;

        // variables
        std::unique_ptr<ExStc::subStruct<exploration_msgs::PointArray>> branch_;
        std::unique_ptr<ExStc::subStruct<geometry_msgs::PoseStamped>> pose_;
        std::unique_ptr<ExStc::subStruct<exploration_msgs::PoseStampedArray>> poseLog_;

        // functions
        void duplicateDetection(std::vector<ExStc::listStruct>& ls, const exploration_msgs::PoseStampedArray& log);
        virtual bool decideGoal(geometry_msgs::PointStamped& goal, const std::vector<ExStc::listStruct>& ls, const geometry_msgs::PoseStamped& pose);
        virtual void loadParams(void);
        virtual void dynamicParamsCB(exploration::sensor_based_exploration_parameter_reconfigureConfig &cfg, uint32_t level);
        virtual void outputParams(void);

    protected:
        // dynamic parameters
        double LAST_GOAL_TOLERANCE;
        double DUPLICATE_TOLERANCE;
        double LOG_CURRENT_TIME;//if 30 -> 30秒前までのログで重複検出
        double NEWER_DUPLICATION_THRESHOLD;//最近通った場所の重複とみなす時間の上限,時間の仕様はLOG_NEWER_LIMITと同じ
        
        // variables
        std::unique_ptr<ExStc::pubStruct<geometry_msgs::PointStamped>> goal_;
        std::unique_ptr<dynamic_reconfigure::Server<exploration::sensor_based_exploration_parameter_reconfigureConfig>> drs_;
        std::unique_ptr<geometry_msgs::Point> lastGoal_;

    public:
        SensorBasedExploration();
        virtual ~SensorBasedExploration();
        bool getGoal(geometry_msgs::PointStamped& goal);
};

#endif // SENSOR_BASED_EXPLORATION_H