// sound playing
#define SWITCH_INTERVAL 300  // minimum time for switching tracks

void setup_sound() {
  MP3Stream.begin(MD_YX5300::SERIAL_BPS, SERIAL_8N1, SOUND_RX, SOUND_TX);
  mp3.begin();

  mp3.setSynchronous(true);
  vol = mp3.volumeMax(); // max seems to be 30, and higher values does not affect it
  PRINT("\nSetting volume to max: ", vol);
  bool b = mp3.volume(vol);
  PRINT(" result ", b);
  mp3.playFolderShuffle(PLAY_FOLDER);
}

void loop_sound() {
  static unsigned long prevSwitch; // the most recent track switch
  if (currentTime - prevSwitch >= SWITCH_INTERVAL && ir) {
    mp3.playNext();
//    mp3.playTrack(trackNum);
    prevSwitch = millis()/1000;
  }
}
