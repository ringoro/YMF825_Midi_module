/*
 *  YMF825 Legacy MIDI Module
 * 
 *    Ringoro
 * 
 *    2017/9/18
 *
*/

#include <avr/pgmspace.h>


unsigned char runsts=0;
unsigned char byt3flg=0;
unsigned char inpt=0;
unsigned char outpt=0;
unsigned char mbuff[256];
unsigned char laston;

#define KON   0x90
#define KOFF  0x80
#define CONT  0xB0
#define PRG   0xC0
#define BEND  0xE0
#define PRES  0xD0
#define AFT   0xA0

extern int fnumtable[12];

#define C4  60

void noteon(int nt ,int vn,int vel)
{
    int n,fn;
    unsigned char blk,fnl0,fnl,fnh;
    char s[50];

    vel = vel  >>2;   // 7bit to 5bit
    n = nt % 12;
    blk = (unsigned char)(nt / 12) - 1;
    fn = fnumtable[n];
    fnh =  fn & 0b01111111;
    fnl =  (fn & 0b001110000000) >> 4 | blk;
//    sprintf(s,"note= %2X  %2X",fnl,fnh);
//    Serial.println(s);
    keyon(fnl,fnh,vn,vel);   /* KEY-ON! */
}

void noteoff(int nt,int v)
{ 
    keyoff(v); 
}


/*
 * Note OO/OFF Send 
 */

unsigned char keybuff[16]={0};
unsigned char setpt=0;

int noteonsend(unsigned char nt,unsigned char vel)
{
   int  v;
   for(v=0;v<16;v++){
      if(keybuff[setpt]==0){
        keybuff[setpt]=nt;
        noteon(nt,setpt,vel);
        setpt = (setpt+1) & 0x0F;
        digitalWrite(7, !digitalRead(7));
        return 1;
      } else {
         setpt = (setpt+1) & 0x0F;
      }
   }

   return 0;
}

int noteoffsend(unsigned char nt)
{
   int  v;
   for(v=0;v<16;v++){
      if(keybuff[v]==nt){
       keybuff[v]=0;
       noteoff(nt,v);
       return 1;
      }
   }
   return 0;
}

/*
 * SET UP 
 */

void setup() {

  ymf825setup();

  Serial.begin(31250);
  runsts=0;
  byt3flg=0;
  inpt=0;
  outpt=0;
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);

}

/*
 * Main LOOP
 */


bool first=true;

void loop()
{
  if(first){
    testplay();
    first=false;
  }

  
  // MIDI IN Buffer check
  if(inpt == outpt)
     return;

  if(mbuff[outpt] <= 0x7F){
      outpt++;
      return;
  }
   
  if((mbuff[outpt]  & 0xF0)==0x90){
    if(mbuff[(outpt+2)&0xFF] == 0){
         noteoffsend(mbuff[(outpt+1)&0xFF]);
         outpt=outpt+3;
         return;
    } else { 
      noteonsend(mbuff[(outpt+1)&0xFF], mbuff[(outpt+2)&0xFF]);
      outpt = outpt+3;
      return;
      }
    }
    
    if((mbuff[outpt]  & 0xF0)==0x80){
      noteoffsend(mbuff[(outpt+1)&0xFF]);
      outpt = outpt+3;
      return;
    }

    if((mbuff[outpt]  & 0xF0) == 0xC0){
      progchang(mbuff[(outpt+1)&0xFF]);
      outpt = outpt+2;
      return;
    }


   // else...
  outpt++;

}

/*
 * When MIDI IN
 */

void serialEvent() {
  unsigned char  c;
  while (Serial.available()) {
    c = (char)Serial.read();
    midi_in_proc(c);
  }
  
   digitalWrite(6, !digitalRead(6));

}


/*
 * Test Play
 *  C E G 'C
 */

char notes[]={60,64,67,72};

void testplay() {
  int i;

  for(i=0;i<sizeof(notes);i++){
    noteon(notes[i],0,90);
    delay(200);
    noteoff(notes[i],0);
    delay(50);
    }

}


/*
 * Tone Parameters
 * 
 */

const  unsigned char  program[4][30]={
{
/*
Piano-1
*/  
0x01,0x85,
0x00,0x72,0xF9,0xD6,0x00,0x20,0x40,
0x00,0xA3,0xFC,0x02,0x03,0x10,0x40,
0x00,0x92,0xF3,0xA2,0x00,0x20,0x41,
0x00,0xA3,0xFC,0x1A,0x01,0x10,0x40
},
{
/*
Piano-2
*/  
0x01,0x85,
0x30,0x75,0xF2,0x7F,0x00,0x40,0x40,
0x30,0x73,0xF0,0x12,0x03,0x10,0x40,
0x60,0x76,0xF3,0x5B,0x00,0x20,0x41,
0x50,0xA7,0xF0,0x12,0x01,0x10,0x40
},
{
/*
Brass 1
*/
0x01,0x85,
0x00,0x74,0x7A,0x55,0x00,0x20,0x00,
0x00,0x8D,0xA0,0x0E,0x03,0x10,0x00,
0x00,0x74,0x7A,0x42,0x00,0x10,0x00,
0x00,0x8C,0xA0,0x0E,0x01,0x10,0x10
} ,

/*
Brass 2
*/
{  
0x01,0x85,
0x00,0x74,0x7B,0x56,0x00,0x10,0x00,
0x00,0x8D,0xA0,0x02,0x03,0x00,0x08,
0x00,0x74,0x7B,0x42,0x00,0x10,0x00,
0x00,0x8C,0xA0,0x02,0x01,0x10,0x08,
}


};



/*
 *   Prog change 
 * 
 */

extern unsigned char tone_data[35];

void progchang(unsigned char p)
{
  int i;
  p = p & 0x3;
 
  for(i=0;i<30;i++){
    tone_data[i+1]=program[p][i];
  }
  set_tone();
  
}


