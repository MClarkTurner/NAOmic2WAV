#include "naomic_2_wav.hpp"

std::atomic<bool> quit (false);

void got_signal(int)
{
    quit = true;
}
/*
void genTestFile()
{
  ofstream f( "example.wav", ios::binary );

  // Write the file headers
  f << "RIFF----WAVEfmt ";     // (chunk size to be filled in later)
  write_word( f,     16, 4 );  // no extension data
  write_word( f,      1, 2 );  // PCM - integer samples
  write_word( f,      2, 2 );  // two channels (stereo file)
  write_word( f,  44100, 4 );  // samples per second (Hz)
  write_word( f, 176400, 4 );  // (Sample Rate * BitsPerSample * Channels) / 8
  write_word( f,      4, 2 );  // data block size (size of two integer samples, one for each channel, in bytes)
  write_word( f,     16, 2 );  // number of bits per sample (use a multiple of 8)

  // Write the data chunk header
  size_t data_chunk_pos = f.tellp();
  f << "data----";  // (chunk size to be filled in later)
  
  // Write the audio samples
  // (We'll generate a single C4 note with a sine wave, fading from left to right)
  constexpr double two_pi = 6.283185307179586476925286766559;
  constexpr double max_amplitude = 32760;  // "volume"

  double hz        = 44100;    // samples per second
  double frequency = 261.626;  // middle C
  double seconds   = 2.5;      // time

  int N = hz * seconds;  // total number of samples
  for (int n = 0; n < N; n++)
  {
    double amplitude = (double)n / N * max_amplitude;
    double value     = sin( (two_pi * n * frequency) / hz );
    write_word( f, (int)(                 amplitude  * value), 2 );
    write_word( f, (int)((max_amplitude - amplitude) * value), 2 );
  }
  
  // (We'll need the final file size to fix the chunk sizes above)
  size_t file_length = f.tellp();

  // Fix the data chunk header to contain the data size
  f.seekp( data_chunk_pos + 4 );
  write_word( f, file_length - data_chunk_pos + 8 );

  // Fix the file header to contain the proper RIFF chunk size, which is (file size - 8) bytes
  f.seekp( 0 + 4 );
  write_word( f, file_length - 8, 4 ); 

}*/

WavWriter::WavWriter(ros::NodeHandle node, string pre): n(node), prefix(pre)
{
  wwquit = false;
  ROS_INFO("Begin wav file");
  genFile();

  ROS_INFO("Ready to Listen");
  sub_playstopped = n.subscribe<std_msgs::Bool>("/playstopped", 1000, &WavWriter::playStopped, this);
  sub_mic = n.subscribe<naoqi_bridge_msgs::AudioBuffer>("/nao_robot/microphone/naoqi_microphone/audio_raw", 1000, &WavWriter::audioReceived, this);
  pub_ready = n.advertise<std_msgs::Bool>("/readyforplay", 1000);
  std_msgs::Bool ready;
  ready.data = true;

  while(pub_ready.getNumSubscribers() == 0){
  }
  pub_ready.publish(ready);
}

WavWriter::~WavWriter()
{
  finishFile();
}

void WavWriter::audioReceived(const naoqi_bridge_msgs::AudioBuffer ab){
  /*if(count % 10 == 0)
    cout << "receiving data ..." << endl;*/
  for(int i = 0; i < ab.data.size(); i++){
    write_word( f, (int)( ab.data[i]), 2 );
  }
  count++;
}

void WavWriter::genFile(){
  f.open( prefix+".wav", ios::binary );

  f << "RIFF----WAVEfmt ";     // (chunk size to be filled in later)
  write_word( f,     16, 4 );  // no extension data
  write_word( f,      1, 2 );  // PCM - integer samples
  write_word( f,      4, 2 );  // four channels (stereo file)
  write_word( f,  16000, 4 );  // samples per second (Hz)
  write_word( f, 128000, 4 );  // (Sample Rate * BitsPerSample * Channels) / 8
  write_word( f,      8, 2 );  // data block size (size of two integer samples, one for each channel, in bytes)
  write_word( f,     16, 2 );  // number of bits per sample (use a multiple of 8)

  // Write the data chunk header
  data_chunk_pos = f.tellp();
  f << "data----";  // (chunk size to be filled in later)
}

void WavWriter::finishFile(){
  // (We'll need the final file size to fix the chunk sizes above)
  size_t file_length = f.tellp();
  cout << "fileLength: " << file_length << endl;
  // Fix the data chunk header to contain the data size
  f.seekp( data_chunk_pos + 4 );
  write_word( f, file_length - data_chunk_pos + 8 );

  // Fix the file header to contain the proper RIFF chunk size, which is (file size - 8) bytes
  f.seekp( 0 + 4 );
  write_word( f, file_length - 8, 4 ); 
  f.close();
}

void WavWriter::playStopped(const std_msgs::Bool b){
  if(b.data)
    wwquit = true;
}

int main(int argc, char** argv){
  struct sigaction sa;
  memset( &sa, 0, sizeof(sa) );
  sa.sa_handler = got_signal;
  sigfillset(&sa.sa_mask);
  sigaction(SIGINT,&sa,NULL);

  ros::init(argc, argv, "wav_generator");
  ros::NodeHandle n;
  ros::Rate r(30);

  string prefix;// = "test";
  ros::NodeHandle private_node_handle_("~");
  private_node_handle_.param("prefix", prefix, string("test"));
  //cout << "prefix: " << prefix << endl;
  WavWriter ww(n, prefix);

  //ros::Duration(5).sleep();
  while (ros::ok()){
    ros::spinOnce();
    if(quit || ww.wwquit) break;
  }
  return 0;
}