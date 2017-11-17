#ifndef NAOMIC_2_WAV_H
#define NAOMIC_2_WAV_H

#include <cmath>
#include <fstream>
#include <iostream>
#include <ros/ros.h>
#include <naoqi_bridge_msgs/AudioBuffer.h>
#include <std_msgs/Bool.h>
#include <signal.h>
#include <cstring>
#include <atomic>
using namespace std;

namespace little_endian_io
{
  template <typename Word>
  std::ostream& write_word( std::ostream& outs, Word value, unsigned size = sizeof( Word ) )
  {
    for (; size; --size, value >>= 8)
      outs.put( static_cast <char> (value & 0xFF) );
    return outs;
  }
}
using namespace little_endian_io;

class WavWriter
{

private:
  ofstream f;
  size_t data_chunk_pos;
  ros::Subscriber sub_mic, sub_playstopped;
  ros::Publisher pub_ready;
  size_t count = 0;
  ros::NodeHandle  n;
  string prefix;
  

  void audioReceived(const naoqi_bridge_msgs::AudioBuffer ab);
  void playStopped(const std_msgs::Bool b);
  void genFile();
  void finishFile();

public:
  WavWriter(ros::NodeHandle , string);
  ~WavWriter();

  atomic<bool> wwquit;// =false;//(false);

};

#endif
