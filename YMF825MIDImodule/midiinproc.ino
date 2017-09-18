
/*
 *      MIDI Running Status Proc.
 *      
 *  Apendix-2
 *  http://amei.or.jp/midistandardcommittee/MIDIspcj.html
 *  
 *    fix  2017 9/14
 */


void bit71(char);   //  Bit7 = 1 proc. 
void bit70(char);   //  Bit7 = 0 proc.
 
void midi_in_proc(unsigned char d)
{
  
  if((d & 0x80)){
    bit71(d);
    return;
  } else {
    bit70(d);
  }
}

// bit7=1 Status Byte proc

void bit71(unsigned char d)
{
  if(d>=0xF8){ // do nothing
   return;
  }
  runsts = d;
  byt3flg=0;
  if(d == 0xF6){  // data push
    mbuff[(inpt++)&0xFF]=d;
  }
  else {
    // not increment pt 
  }
}

// Data Byte proc

void bit70(unsigned char d)
{
 if(byt3flg){
   byt3flg=0;
   mbuff[(inpt+2)&0xFF]=d;
   inpt=inpt+3;
   return;
   }
 //  else
 
 if(runsts==0){  
    return; // do nothing
    }

  if(runsts < 0xC0){
    goto proc2;
  }
  if (runsts<0xE0){    // fix 9/14
      goto proc3;
  }
  
  if(runsts < 0xF0){
    goto proc2;
  }
  
  if(runsts==0xF2){ // song position pt
    runsts=0;
    goto proc2;
  }
  
  if(runsts==0xF3 || runsts == 0xF1){
    runsts = 0;  // 2bytes Status
    goto proc3;
  }
  // else do nothing
  // !! SysExc not proceed. !!
  runsts=0;
  return;
  
  
proc2:  // byt 2 of 3-Bytes data proc
  byt3flg=1;
  mbuff[inpt]=runsts;
  mbuff[(inpt+1)&0xFF]=d;
  //　not increment. wait 3byt
  return;

proc3:  // 2-bytes Status
  mbuff[inpt]=runsts;
  mbuff[(inpt+1)&0xFF]=d;
  inpt = inpt+2;
  return;
  
}
