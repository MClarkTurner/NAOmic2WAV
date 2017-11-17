cmake_minimum_required(VERSION 2.8.3)
project(naomic2_wav)

include_directories(
  ${catkin_INCLUDE_DIRS}
)


find_package(catkin REQUIRED COMPONENTS
  roscpp
  std_msgs
  sensor_msgs
  nao_msgs
)

find_package(Boost  REQUIRED COMPONENTS
)

################################################
## Declare ROS messages, services and actions ##
################################################

catkin_package(

)

#############
## Testing ##
#############

add_executable(wav_generatorpre src/naomic_2_wav.cpp)
target_link_libraries(wav_generatorpre ${catkin_LIBRARIES})
