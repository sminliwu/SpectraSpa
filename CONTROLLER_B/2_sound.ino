// sound playing
#define SWITCH_INTERVAL 240  // minimum time for switching tracks
// TODO: make this longer and configurable
#define TRACK_NUM 5

// track numbers for Spectra Spa music files
#define NUM_TRACKS      8
#define TRK_WELCOME_1     0
#define TRK_WELCOME_2     1
#define TRK_SOUNDBATH_1   2
#define TRK_SOUNDBATH_2   3
#define TRK_GALACTIC      4
#define TRK_LUNAVOCALS    5
#define TRK_AFFIRM        6
#define TRK_JUNGLE        7

void setup_sound() {
  MP3Stream.begin(MD_YX5300::SERIAL_BPS, SERIAL_8N1, 18, 19);
  mp3.begin();

  mp3.setSynchronous(true);
  vol = mp3.volumeMax(); // max seems to be 30, and higher values does not affect it
  PRINT("\nSetting volume to max: ", vol);
  bool b = mp3.volume(vol);
  PRINT(" result ", b);
  mp3.playFolderShuffle(PLAY_FOLDER);
  mp3.playPause();
}

void loop_sound() {
  static unsigned long prevSwitch; // the most recent track switch
  //uint8_t playMode = MODE_SENS_DIGITAL;

  if ((millis()/1000 - prevSwitch) >= SWITCH_INTERVAL && ir) {
//    mp3.playTrack(trackNum);
    mp3.playNext();
    prevSwitch = millis()/1000;
  }
}
