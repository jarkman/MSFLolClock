
// MSFTime demo for LolShield
// Jarkman, 01/2011
// http://www.jarkman.co.uk/catalog/robots/msftime.htm

// Prerequisites:
//
// An MSF time receiver, wired to analogue pin 0, like this: http://www.pvelectronics.co.uk/index.php?main_page=product_info&cPath=9&products_id=2
//
// A LOLShield: http://jimmieprodgers.com/kits/lolshield/
//
// Time library: http://www.pjrc.com/teensy/td_libs_Time.html - unzip it into your libraries folder
// 
// LOLShield library: http://code.google.com/p/lolshield/source/browse/#svn%2Ftrunk%2Flib%253Fstate%253Dclosed
//
// I have modified Font.cpp (part of the LOLShield library) to add letters_58, the : character.


#include "Font.h"            // from http://code.google.com/p/lolshield/source/browse/#svn%2Ftrunk%2Flib%253Fstate%253Dclosed
#include "Charliplexing.h"   // ditto

//#include "WProgram.h"
#include <Time.h>            // fromy: http://www.pjrc.com/teensy/td_libs_Time.html

#include "MSFTime.h"

MSFTime MSF;// = MSFTime();

time_t msfTimeSync();
byte prevStatus = 255;



void setup()                    // run once, when the sketch starts
{
  Serial.begin(9600);

  LedSign::Init(GRAYSCALE);
  
  MSF.init( 255 ); // LED pin for status
   
  setSyncProvider(msfTimeSync);  // tell the Time library what to ask for a new time value 

}

time_t msfTimeSync() // called periodically by Time library to syncronise itself
{
   return MSF.getTime();
}


void drawLine( int offset, int val, int range )
{
  
  int total = 9;

  val = val%range;
  int at = ( val * total ) / range;  
  

  for( int i =0; i < at-1; i ++ )
    LedSign::Set(i,offset, 1);
    
  LedSign::Set(at-1, offset, 3);
  
}



int offset = 13;

void loop()                     // run over and over again
{
  LedSign::Clear();
  
  //drawLine( 0, 7, 8);
  
  char buff[20];
  now(); // make sure Time has attempted to sync
	 
  byte currStatus = MSF.getStatus();
 
  #define MESSAGE_LEN 20

  char test[MESSAGE_LEN];
 
  int chars =0;
  
  if( timeStatus() == timeNotSet )
  {
    

     if( ! (currStatus & MSF_STATUS_CARRIER))
       chars = sprintf( test, "NO CARRIER   ");
     else if( (currStatus & MSF_STATUS_WAITING))
     {
       drawLine( 8, MSF.getProgess(), 60 ); 

       chars = sprintf( test, "WAITING   ");
     }
     else if( (currStatus & MSF_STATUS_READING))
     {
       drawLine( 8, MSF.getProgess(), 60 );

       chars = sprintf( test, "READING   "); 
     }
     else
       chars = sprintf( test, "CONFUSED   "); // should never happen
  }
  else
  {
    int mins = MSF.getFixAge()/60000L;
    if( mins < 0 )
      mins = 0;
      
    drawLine( 8, mins, 14 ); // fix age, one LED per min
   chars = sprintf( test, "%02d:%02d:%02d   ", hour(), minute(), second() );
   // could also display date here
  }


  int8_t x, x2;

  x = offset;
 

  for(int i=0;i<chars;i++) 
  {
    x2=Font::Draw(test[i],x,0,3);
    x+=x2;
    if (x>=13) break;
  }  
  
  if( x >= 13 )
    offset --; // there was more stuff to draw off the right, keep scrolling
  else
    offset = 13; // set the scroll back to draw at the right of the screen
    
  delay(80);
    

}


