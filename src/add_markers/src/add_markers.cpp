#include <ros/ros.h>
#include <visualization_msgs/Marker.h>
#include "nav_msgs/Odometry.h"
#include <cmath>
#include <complex>
// %EndTag(INCLUDES)%

//Coordinates
float xi = 4.0;
float yi = 0.0;
float xii = 4.0;
float yii = -4.0;

//Conditionals
bool picked = false;
bool at_pzone = false;
bool dropped = false;
bool at_gzone = false;

//double distance(x1,y1,x2,y2){
//  return (sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2)))
//}

void odomCallback(const nav_msgs::Odometry::ConstPtr& msg) {
  float posX = msg->pose.pose.position.x;
  float posY = msg->pose.pose.position.y;
  float to_pzone = sqrt(pow(posX - xi, 2) + pow(posY - yi, 2));
  float to_gzone = sqrt(pow(posX- xii, 2) + pow(posY - yii, 2));

  if (to_pzone < 0.3) {
    at_pzone = true;
  }
  else{
    at_pzone = false;
  }
  if (to_gzone < 0.3) {
    at_gzone = true;
  }
  else{
    at_gzone = false;
  }
}

// %Tag(INIT)%
int main( int argc, char** argv )
{
  ros::init(argc, argv, "add_markers");
  ros::NodeHandle n;
  ros::Publisher marker_pub = n.advertise<visualization_msgs::Marker>("visualization_marker", 1);
  ros::Subscriber odom_sub = n.subscribe("odom", 10, odomCallback);
  // %EndTag(INIT)%

  // %Tag(SHAPE_INIT)%
  // Set our initial shape type to be a cube
  uint32_t shape = visualization_msgs::Marker::CUBE;
  // %EndTag(SHAPE_INIT)%

  while (ros::ok())
  {
    visualization_msgs::Marker marker;
    // Set the frame ID and timestamp.  See the TF tutorials for information on these.
    marker.header.frame_id = "map";
    marker.header.stamp = ros::Time::now();
    // %EndTag(MARKER_INIT)%

    // Set the namespace and id for this marker.  This serves to create a unique ID
    // Any marker sent with the same namespace and id will overwrite the old one
    // %Tag(NS_ID)%
    marker.ns = "cube";
    marker.id = 0;
    // %EndTag(NS_ID)%

    // Set the marker type.  Initially this is CUBE, and cycles between that and SPHERE, ARROW, and CYLINDER
    // %Tag(TYPE)%
    marker.type = shape;
    // %EndTag(TYPE)%

    // Set the marker action.  Options are ADD, DELETE, and new in ROS Indigo: 3 (DELETEALL)
    // %Tag(ACTION)%
    marker.action = visualization_msgs::Marker::ADD;
    // %EndTag(ACTION)%

    // Set the pose of the marker.  This is a full 6DOF pose relative to the frame/time specified in the header
    // %Tag(POSE)%
    marker.pose.position.x = xi;
    marker.pose.position.y = yi;
    marker.pose.position.z = 0;
    marker.pose.orientation.x = 0.0;
    marker.pose.orientation.y = 0.0;
    marker.pose.orientation.z = 0.0;
    marker.pose.orientation.w = 1.0;
    // %EndTag(POSE)%

    // Set the scale of the marker -- 1x1x1 here means 1m on a side
    // %Tag(SCALE)%
    marker.scale.x = 0.2;
    marker.scale.y = 0.2;
    marker.scale.z = 0.2;
    // %EndTag(SCALE)%

    // Set the color -- be sure to set alpha to something non-zero!
    // %Tag(COLOR)%
    marker.color.r = 0.0f;
    marker.color.g = 1.0f;
    marker.color.b = 0.0f;
    marker.color.a = 1.0;
    // %EndTag(COLOR)%

    // %Tag(LIFETIME)%
    marker.lifetime = ros::Duration();
    // %EndTag(LIFETIME)%

    // Publish the marker
    // %Tag(PUBLISH)%
    // %Tag(MARKER_INIT)%
    while (marker_pub.getNumSubscribers() < 1)
    {
      if (!ros::ok())
      {
        return 0;
      }
      ROS_WARN_ONCE("Please create a subscriber to the marker");
      sleep(1);
    }

    marker_pub.publish(marker);
    ROS_INFO("Object placed at pick-up zone");
    while(!at_pzone){
	ros::spinOnce();
    }
    if (!picked && at_pzone)
    {
      marker.action = visualization_msgs::Marker::DELETE;
      marker_pub.publish(marker);
      ROS_INFO("Object picked up!");
      picked = true;
    }
    while(!at_gzone){	
	ros::spinOnce();
    }
    if (!dropped && at_gzone){
      marker.pose.position.x = xii;
      marker.pose.position.y = yii;
      marker.action = visualization_msgs::Marker::ADD;
      marker_pub.publish(marker);
      ROS_INFO("Item dropped");
      dropped = true;
      ros::Duration(5.0).sleep();
    }
    ros::spinOnce();
  }
}

