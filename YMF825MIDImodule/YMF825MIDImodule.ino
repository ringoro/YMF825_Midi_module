/*
 *  YMF825 Legacy MIDI Module
 * 
 *    Ringoro
 * 
 *  2017/9/18
 *  2017/9/25  
 *    use case 
 *    PROGMEM
 *       9/29
 *    CC 1,7,123
 *       10/3
 *    P.Bnend Table
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
#define CTRL  0xB0
#define PRG   0xC0
#define BEND  0xE0
#define PRES  0xD0
#define AFT   0xA0

#define CC_MOD  1
#define CC_VOL  7
#define CC_DMP  64
#define CC_ALLOFF 123


extern int fnumtable[12];

#define C4  60

void noteon(int nt ,int vn,int vel)
{
    int n,fn;
    unsigned char blk,fnl0,fnl,fnh;
    char s[50];

    vel = vel  >>2;   // 7bit to 5bit
    n = nt % 12;
    blk = (unsigned char)(nt / 12) - 1; // fix 9/28
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

  Serial.begin(31500);
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
   unsigned char sts,b1,b2;
  if(first){
    testplay();
    first=false;
  }

  // MIDI IN Buffer check
  if(inpt == outpt)
     return;

  sts=mbuff[outpt] & 0xF0;
  b1=mbuff[(outpt+1)&0xFF];
  b2=mbuff[(outpt+2)&0xFF];

  if(sts <= 0x7F){
      outpt++;
      return;
  }

//  each event

  switch(sts){
    case KON:
      if(b2==0){
        noteoffsend(b1);
      } else {
        noteonsend(b1,b2);
      }
      outpt=outpt+3;
      break;

    case KOFF:
      noteoffsend(b1);
      outpt=outpt+3;
      break;
      
    case PRG:
      progchang(b1);
      outpt=outpt+2;
      break;
      
    case CTRL:
      ctrlchange(b1,b2);
      outpt=outpt+3;
      break;
      
    case BEND:
      ptchbend(b1,b2);
      outpt=outpt+3;
      break;
      
   defalut:
    outpt++;

  }


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
  int d;

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

// const  unsigned char   program[4][30]={

 const  unsigned char PROGMEM   program[4][30]={

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
 */
extern unsigned char tone_data[35];
void progchang(unsigned char p)
{
  int i;
  p = p & 0x3;
 
  for(i=0;i<30;i++){
    tone_data[i+1]=pgm_read_byte_near(&(program[p][0])+i);
//    tone_data[i+1]=program[p][i];
  }
  set_tone();
  
}

/*
 * Control Change
 */
void ctrlchange(unsigned char b1,unsigned char b2)
{

  switch(b1){
    case CC_VOL:
      setmastervol(b2);
      break;
   case CC_MOD:
      setmodulate(b2);
      break;

   case CC_ALLOFF:
      alloff();
      break;

   default:
    break;
    
  }
  
}

/*
 * Pitch Bend
 * 
 *  conver to REG #18,19   256...512...1023
 *  by Table
 *  
 */
int ptchbend(unsigned char b1,unsigned char b2)
{
PROGMEM static const unsigned short tPitTbl[256] = {
  256,257,259,260,262,263,264,266,267,269,270,272,273,275,276,278,
  279,281,282,284,285,287,288,290,292,293,295,296,298,300,301,303,
  304,306,308,309,311,313,314,316,318,320,321,323,325,327,328,330,
  332,334,336,337,339,341,343,345,347,349,350,352,354,356,358,360,
  362,364,366,368,370,372,374,376,378,380,382,384,386,388,391,393,
  395,397,399,401,403,406,408,410,412,415,417,419,421,424,426,428,
  431,433,435,438,440,442,445,447,450,452,454,457,459,462,464,467,
  470,472,475,477,480,482,485,488,490,493,496,498,501,504,506,509,
  512,515,518,520,523,526,529,532,535,538,540,543,546,549,552,555,
  558,561,564,567,571,574,577,580,583,586,589,593,596,599,602,606,
  609,612,616,619,622,626,629,632,636,639,643,646,650,653,657,660,
  664,668,671,675,679,682,686,690,693,697,701,705,709,712,716,720,
  724,728,732,736,740,744,748,752,756,760,764,769,773,777,781,785,
  790,794,798,803,807,811,816,820,825,829,834,838,843,847,852,856,
  861,866,870,875,880,885,890,894,899,904,909,914,919,924,929,934,
  939,944,949,954,960,965,970,975,981,986,991,997,1002,1007,1013,1023 };

  extern int pbendval;
  int k,bd,bv;
  bv = b2*128 + b1;
  k = bv / 64;

  bd = pgm_read_word_near(tPitTbl + k);

  setpbend(bd);
  return bd;  
}





/*
 * pitch Bend   unused...
 * 
 *  conver to REG #18,19   256...512...1023
 *  
 */
 /*
int _ptchbend(unsigned char b1,unsigned char b2)
{
  extern int pbendval;
  long   dlt,bv,bd;

  bv = b2*128 + b1;
  dlt = bv-8192;

  if (dlt==0){
    bd = 512;
    }
  if (dlt > 0){
    bd = 512 * (dlt+8192)/8192;
  }

  if (dlt < 0){
    dlt = 0 - dlt;
    bd = 512L * 8192L /(8192+dlt);
  }
  setpbend((int)bd);
  return (int)bd;  
}

*/

