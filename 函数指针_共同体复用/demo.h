
/*来源：madplay/audio.h
*/

enum audio_command {
  AUDIO_COMMAND_INIT,
  AUDIO_COMMAND_CONFIG,
  AUDIO_COMMAND_PLAY,
  AUDIO_COMMAND_STOP,
  AUDIO_COMMAND_FINISH
};

union audio_control {
  enum audio_command command;

  struct audio_init {
    enum audio_command command;
    char const *path;
  } init;

  struct audio_config {
    enum audio_command command;
    unsigned int channels;
    unsigned int speed;
    unsigned int precision;
  } config;

  struct audio_play {
    enum audio_command command;
    unsigned int nsamples;
    mad_fixed_t const *samples[2];
    enum audio_mode mode;
    struct audio_stats *stats;
  } play;

  struct audio_stop {
    enum audio_command command;
    int flush;
  } stop;

  struct audio_finish {
    enum audio_command command;
  } finish;
};

typedef int audio_ctlfunc_t(union audio_control *);