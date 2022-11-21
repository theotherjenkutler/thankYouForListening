#include <MIDI.h>
#include <MozziGuts.h>
#include <Oscil.h> // oscillator template
#include <Line.h> // for envelope
#include <mozzi_midi.h>
#include <ADSR.h>
#include <mozzi_fixmath.h>
#include <tables/sin2048_int8.h> // sine table for oscillator
#include <tables/saw2048_int8.h> // saw table for oscillator
#include <tables/triangle2048_int8.h> // triangle table for oscillator
//#include <tables/square_no_alias2048_int8> // square table for oscillator
//#include <tables/bass.h>
//#include <tables/strings.h>
//#include <tables/voice.h>
//#include <tables/breath.h>


#define LED 13

// set other constants
#define CONTROL_RATE 64 // low to save processor

#define ATTACK 50 // long enough for control rate to catch it
#define DECAY 50
#define SUSTAIN 600000 // Sustain 60 seconds unless a noteOff comes.
#define RELEASE 1000
// higher resolution for smoother envelopes

#define ATTACK_LEVEL 255
#define DECAY_LEVEL 255


// declare with or without a wavetable, and use setTable() later
Oscil <2048, AUDIO_RATE> oscil1; 
Oscil <2048, AUDIO_RATE> oscil2; 
Oscil <2048, AUDIO_RATE> oscil3; 
Oscil <2048, AUDIO_RATE> oscil4; 
Oscil <2048, AUDIO_RATE> oscil5; 
Oscil <2048, AUDIO_RATE> oscil6; 



// envelope generators
// requires latest Mozzi (April 2014), enables envelope.next() at control rate, using latest version of Mozzi
// use: ADSR <unsigned int CONTROL_UPDATE_RATE, unsigned int LERP_RATE> envName;
ADSR <CONTROL_RATE, CONTROL_RATE> envelope1;
ADSR <CONTROL_RATE, CONTROL_RATE> envelope2;
ADSR <CONTROL_RATE, CONTROL_RATE> envelope3;
ADSR <CONTROL_RATE, CONTROL_RATE> envelope4;
ADSR <CONTROL_RATE, CONTROL_RATE> envelope5;
ADSR <CONTROL_RATE, CONTROL_RATE> envelope6;


// notes
byte note=0;
byte note1=0;
byte note2=0;
byte note3=0;
byte note4=0;
byte note5=0;
byte note6=0;
const byte NOTEON = 0x09;
const byte NOTEOFF = 0x08;


// gains to carry control rate envelope levels from updateControl() to updateAudio()
byte gain1,gain2,gain3,gain4,gain5,gain6 = 0;

// wave type control
boolean waveLatchOn=false; // acts as a semaphore to handle button push/release actions, to avoid looping
byte waveNumber=2; // defines selected wave type
byte notecounter = 0;
//ADSR control
int sensorPin = A0;
int decayValue = 50;
byte noteonnumber = 0;
byte noteoffumber = 0;
int mozziraw = 0;
int lastmozziraw = 0;
int vibrato = 0;
int mode = 2;
int detune = 7;
void setup() {

 // Serial.begin(9600);
  
  pinMode(LED, OUTPUT); // set led to indicate playing note(s)


  envelope1.setADLevels(ATTACK_LEVEL,DECAY_LEVEL);
  envelope1.setTimes(ATTACK,DECAY,SUSTAIN,RELEASE); 
  envelope2.setADLevels(ATTACK_LEVEL,DECAY_LEVEL);
  envelope2.setTimes(ATTACK,DECAY,SUSTAIN,RELEASE); 
  envelope3.setADLevels(ATTACK_LEVEL,DECAY_LEVEL);
  envelope3.setTimes(ATTACK,DECAY,SUSTAIN,RELEASE); 
  envelope4.setADLevels(ATTACK_LEVEL,DECAY_LEVEL);
  envelope4.setTimes(ATTACK,DECAY,SUSTAIN,RELEASE);  
  envelope5.setADLevels(ATTACK_LEVEL,DECAY_LEVEL);
  envelope5.setTimes(ATTACK,DECAY,SUSTAIN,RELEASE); 
  envelope6.setADLevels(ATTACK_LEVEL,DECAY_LEVEL);
  envelope6.setTimes(ATTACK,DECAY,SUSTAIN,RELEASE);  

  setWave(waveNumber);

  startMozzi(CONTROL_RATE); 
  

}


void setWave(int waveNumber) { // good practice to use local parameters to avoid global confusion

  switch (waveNumber) {
  case 1:

    oscil1.setTable(SIN2048_DATA);
    oscil2.setTable(SIN2048_DATA);
    oscil3.setTable(SIN2048_DATA);
    oscil4.setTable(SIN2048_DATA);
    oscil5.setTable(SIN2048_DATA);
    oscil6.setTable(SIN2048_DATA);

    break;
  case 2:
  
    oscil1.setTable(TRIANGLE2048_DATA);
    oscil2.setTable(TRIANGLE2048_DATA);
    oscil3.setTable(TRIANGLE2048_DATA);
    oscil4.setTable(TRIANGLE2048_DATA);
    oscil5.setTable(TRIANGLE2048_DATA);
    oscil6.setTable(TRIANGLE2048_DATA);
    break;
  case 3:
    
    oscil1.setTable(SAW2048_DATA);
    oscil2.setTable(SAW2048_DATA);
    oscil3.setTable(SAW2048_DATA);
    oscil4.setTable(SAW2048_DATA);
    oscil5.setTable(SAW2048_DATA);
    oscil6.setTable(SAW2048_DATA);
    break;
  /*  
  case 4:
    
    oscil1.setTable(SQUARE_NO_ALIAS_2048_DATA);
    oscil2.setTable(SQUARE_NO_ALIAS_2048_DATA);
    oscil3.setTable(SQUARE_NO_ALIAS_2048_DATA);
    oscil4.setTable(SQUARE_NO_ALIAS_2048_DATA);
    oscil5.setTable(SQUARE_NO_ALIAS_2048_DATA);
    oscil6.setTable(SQUARE_NO_ALIAS_2048_DATA);
    break;

        case 5:
    
    oscil1.setTable(PERC_TABLE_DATA);
    oscil2.setTable(PERC_TABLE_DATA);
    oscil3.setTable(PERC_TABLE_DATA);
    oscil4.setTable(PERC_TABLE_DATA);
    oscil5.setTable(PERC_TABLE_DATA);
    oscil6.setTable(PERC_TABLE_DATA);
    break;

        case 6:
    
    oscil1.setTable(STRINGS_TABLE_DATA);
    oscil2.setTable(STRINGS_TABLE_DATA);
    oscil3.setTable(STRINGS_TABLE_DATA);
    oscil4.setTable(STRINGS_TABLE_DATA);
    oscil5.setTable(STRINGS_TABLE_DATA);
    oscil6.setTable(STRINGS_TABLE_DATA);
    break;

         case 7:
    
    oscil1.setTable(VOICE_TABLE_DATA);
    oscil2.setTable(VOICE_TABLE_DATA);
    oscil3.setTable(VOICE_TABLE_DATA);
    oscil4.setTable(VOICE_TABLE_DATA);
    oscil5.setTable(VOICE_TABLE_DATA);
    oscil6.setTable(VOICE_TABLE_DATA);
    break;

      case 8:
    
    oscil1.setTable(BREATH_TABLE_DATA);
    oscil2.setTable(BREATH_TABLE_DATA);
    oscil3.setTable(BREATH_TABLE_DATA);
    oscil4.setTable(BREATH_TABLE_DATA);
    oscil5.setTable(BREATH_TABLE_DATA);
    oscil6.setTable(BREATH_TABLE_DATA);
    break;
  */
  
  }



}


void updateControl(){


  while(MIDIUSB.available() > 0) {
    MIDIEvent e = MIDIUSB.read();

    if((e.type == NOTEON) && (e.m3 > 0)){
      digitalWrite(LED, HIGH);
      note = e.m2;
        notecounter++;
        if (notecounter > 0)
        {
      USBNoteon(note);
  
        }

      }

      
    if((e.type == NOTEON) && (e.m3 == 0)){
      note = e.m2;
      USBNoteoff(note);
      notecounter--;
  Serial.println(notecounter);
     
     
      }
    
    if(e.type == NOTEOFF){
      notecounter--;

note = e.m2;

USBNoteoff(note);

  
    
    }
}


mozziraw = mozziAnalogRead(A1);

if (mozziraw != lastmozziraw)
{
 waveNumber = map(mozziraw, 0, 1023, 0, 8);
 
  setWave(waveNumber);
  lastmozziraw = mozziraw;

}
 
  envelope1.update();
  envelope2.update();
  envelope3.update();
  envelope4.update();
  envelope5.update();
  envelope6.update();

  if (waveNumber < 4) 
  {

  gain1 = envelope1.next() >> 3;
  gain2 = envelope2.next() >> 3;
  gain3 = envelope3.next() >> 3;
  gain4 = envelope4.next() >> 3;
  gain5 = envelope5.next() >> 3;
  gain6 = envelope6.next() >> 3;
  }
  if (waveNumber > 5)
  {
     gain1 = envelope1.next() >> 3;
  gain2 = envelope2.next() >> 3;
  gain3 = envelope3.next() >> 3;
  gain4 = envelope4.next() >> 3;
  gain5 = envelope5.next() >> 3;
  gain6 = envelope6.next() >> 3;
  }
}


int updateAudio(){

 
  return ((long)+ gain1 * oscil1.next() + 
    (int)gain2 * oscil2.next() + 
    (int)gain3 * oscil3.next() + 
    (int)gain4 * oscil4.next() +
    (int)gain5 * oscil5.next() + 
    (int)gain6 * oscil6.next()) >> 6;
    
}


void loop() {
  audioHook(); // required here
} 



void  USBNoteon(byte note)
{

  

if(note1==0){
    note1=note;
    oscil1.setFreq_Q16n16(Q16n16_mtof(Q8n0_to_Q16n16(note)));
    envelope1.noteOn();
  }
  else if(note2==0){
    note2=note;
    oscil2.setFreq_Q16n16(Q16n16_mtof(Q8n0_to_Q16n16(note)));
    envelope2.noteOn();
  }
  else if(note3==0){
    note3=note;
    oscil3.setFreq_Q16n16(Q16n16_mtof(Q8n0_to_Q16n16(note)));
    envelope3.noteOn();
  }
  else if(note4==0){
    note4=note;
    oscil4.setFreq_Q16n16(Q16n16_mtof(Q8n0_to_Q16n16(note)));
    envelope4.noteOn();
  }
  else if(note5==0){
    note5=note;
    oscil5.setFreq_Q16n16(Q16n16_mtof(Q8n0_to_Q16n16(note)));
    envelope5.noteOn();
  }
  else if(note6==0){
    note6=note;
    oscil6.setFreq_Q16n16(Q16n16_mtof(Q8n0_to_Q16n16(note)));
    envelope6.noteOn();
  }


  

 

  if(note1+note2+note3+note4+note5+note6==0){
   digitalWrite(LED,LOW); // no notes playing
  
  }

}




void  USBNoteoff(byte note)
{

if(note==note1){
    envelope1.noteOff();
    note1=0;
  }
  else if(note==note2){
    envelope2.noteOff();
    note2=0;
  }
  else if(note==note3){
    envelope3.noteOff();
    note3=0;
  }
  else if(note==note4){
    envelope4.noteOff();
    note4=0;
  }
  else if(note==note5){
    envelope5.noteOff();
    note5=0;
  }
  else if(note==note6){
    envelope6.noteOff();
    note6=0;
  }

  if(notecounter==0)
  {  
         
  digitalWrite(LED,LOW); // no notes playing

 envelope1.noteOff();
     envelope2.noteOff();
       envelope3.noteOff();
         envelope4.noteOff();
           envelope5.noteOff();
             envelope6.noteOff();
  
   } 

  
}




