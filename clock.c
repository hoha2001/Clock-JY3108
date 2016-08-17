// Clock/calander programm for JY 3208 Clock from DX. 32*8 LED Matrix + HT1632C + ATmega8; 
//
// Byoung Oh. 9/30/12
// Based on code from DrJones
//
//
// Key1 : Setup Button. Hold 3 sec to enter or exit setup mode. In setup mode, use to switch mode.
// Key2 : Date Button. Press to display date. In setup mode, increase number.
// Key3 : LED Brightness Button. In setup mode, decrease number.



#define F_CPU 1000000

#include <avr/pgmspace.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

#define byte uint8_t
#define word uint16_t


// Font Map
#define upper_char_offset -55
#define lower_char_offset -61

// Not using all characters, but keeping in code to future use.
static const byte font[77][5] PROGMEM = {
	{0x7E, 0x91, 0x89, 0x85, 0x7E}, //  0          [0]
	{0x00, 0x82, 0xFF, 0x80, 0x00}, //  1          [1]
	{0xC2, 0xA1, 0x91, 0x89, 0x86}, //  2          [2]
	{0x42, 0x81, 0x89, 0x89, 0x76}, //  3          [3]
	{0x18, 0x14, 0x12, 0xFF, 0x10}, //  4          [4]
	{0x4F, 0x89, 0x89, 0x89, 0x71}, //  5          [5]
	{0x78, 0x94, 0x92, 0x91, 0x61}, //  6          [6]
	{0x01, 0xF1, 0x09, 0x05, 0x03}, //  7          [7]
	{0x76, 0x89, 0x89, 0x89, 0x76}, //  8          [8]
	{0x0E, 0x91, 0x91, 0x51, 0x3E}, //  9          [9]
	{0xFE, 0x11, 0x11, 0x11, 0xFE}, //  A          [10]
	{0xFF, 0x89, 0x89, 0x89, 0x76}, //  B          [11]
	{0x7E, 0x81, 0x81, 0x81, 0x42}, //  C          [12]
	{0xFF, 0x81, 0x81, 0x42, 0x3C}, //  D          [13]
	{0xFF, 0x89, 0x89, 0x89, 0x81}, //  E          [14]
	{0xFF, 0x09, 0x09, 0x09, 0x01}, //  F          [15]
	{0x7E, 0x81, 0x91, 0x91, 0x72}, //  G          [16]
	{0xFF, 0x08, 0x08, 0x08, 0xFF}, //  H          [17]
	{0x81, 0x81, 0xFF, 0x81, 0x81}, //  I          [18]
	{0x60, 0x80, 0x80, 0x80, 0x7F}, //  J          [19]
	{0xFF, 0x18, 0x24, 0x42, 0x81}, //  K          [20]
	{0xFF, 0x80, 0x80, 0x80, 0x80}, //  L          [21]
	{0xFF, 0x02, 0x0C, 0x02, 0xFF}, //  M          [22]
	{0xFF, 0x02, 0x0C, 0x10, 0xFF}, //  N          [23]
	{0x7E, 0x81, 0x81, 0x81, 0x7E}, //  O          [24]
	{0xFF, 0x11, 0x11, 0x11, 0x0E}, //  P          [25]
	{0x7E, 0x81, 0xA1, 0x41, 0xBE}, //  Q          [26]
	{0xFF, 0x11, 0x31, 0x51, 0x8E}, //  R          [27]
	{0x86, 0x89, 0x89, 0x89, 0x71}, //  S          [28]
	{0x01, 0x01, 0xFF, 0x01, 0x01}, //  T          [29]
	{0x7F, 0x80, 0x80, 0x80, 0x7F}, //  U          [30]
	{0x0F, 0x30, 0xC0, 0x30, 0x0F}, //  V          [31]
	{0xFF, 0x40, 0x30, 0x40, 0xFF}, //  W          [32]
	{0xC3, 0x24, 0x18, 0x24, 0xC3}, //  X          [33]
	{0x07, 0x08, 0xF0, 0x08, 0x07}, //  Y          [34]
	{0xC1, 0xA1, 0x99, 0x85, 0x83}, //  Z          [35]
	{0x60, 0x94, 0x94, 0x94, 0xF8}, //  a          [36]
	{0xFF, 0x90, 0x88, 0x88, 0x70}, //  b          [37]
	{0x78, 0x84, 0x84, 0x84, 0x48}, //  c          [38]
	{0x70, 0x88, 0x88, 0x90, 0xFF}, //  d          [39]
	{0x70, 0xA8, 0xA8, 0xA8, 0x30}, //  e          [40]
	{0x10, 0xFE, 0x11, 0x01, 0x02}, //  f          [41]
	{0x0C, 0x92, 0x92, 0x92, 0x7E}, //  g          [42]
	{0xFF, 0x10, 0x08, 0x08, 0xF0}, //  h          [43]
	{0x00, 0x00, 0xFA, 0x00, 0x00}, //  i          [44]
	{0x40, 0x80, 0x84, 0x7D, 0x00}, //  j          [45]
	{0xFF, 0x20, 0x50, 0x88, 0x00}, //  k          [46]
	{0x00, 0x81, 0xFF, 0x80, 0x00}, //  l          [47]
	{0xFC, 0x04, 0x18, 0x04, 0xF8}, //  m          [48]
	{0xFC, 0x08, 0x04, 0x04, 0xF8}, //  n          [49]
	{0x70, 0x88, 0x88, 0x88, 0x70}, //  o          [50]
	{0xFC, 0x24, 0x24, 0x24, 0x18}, //  p          [51]
	{0x08, 0x14, 0x14, 0x18, 0xFC}, //  q          [52]
	{0xF8, 0x10, 0x08, 0x08, 0x10}, //  r          [53]
	{0x90, 0xA8, 0xA8, 0xA8, 0x40}, //  s          [54]
	{0x04, 0x7F, 0x84, 0x80, 0x40}, //  t          [55]
	{0x78, 0x80, 0x80, 0x40, 0xF8}, //  u          [56]
	{0x38, 0x40, 0x80, 0x40, 0x38}, //  v          [57]
	{0x78, 0x80, 0x70, 0x80, 0x78}, //  w          [58]
	{0x88, 0x50, 0x20, 0x50, 0x88}, //  x          [59]
	{0x00, 0x9C, 0xA0, 0xA0, 0x7C}, //  y          [60]
	{0x88, 0xC8, 0xA8, 0x98, 0x88}, //  z          [61]
	{0x00, 0x00, 0xBF, 0x00, 0x00}, //  !          [62]
	{0x24, 0xFF, 0x24, 0xFF, 0x24}, //  #          [63]
	{0x48, 0x54, 0xFE, 0x54, 0x24}, //  $          [64]
	{0x6E, 0x91, 0xA9, 0x46, 0xA0}, //  &          [65]
	{0x00, 0x3C, 0x42, 0x81, 0x00}, //  (          [66]
	{0x00, 0x81, 0x42, 0x3C, 0x00}, //  )          [67]
	{0x10, 0x10, 0x7C, 0x10, 0x10}, //  "+"        [68]
	{0x10, 0x10, 0x10, 0x10, 0x10}, //  -          [69]
	{0x28, 0x28, 0x28, 0x28, 0x28}, //  =          [70]
	{0x06, 0x01, 0xB1, 0x09, 0x06}, //  ?          [71]
	{0x66, 0x91, 0xF1, 0x81, 0x7E}, //  @          [72]
	{0x08, 0x08, 0x2A, 0x1C, 0x08}, //  R arrow    [73]
	{0x10, 0x38, 0x54, 0x10, 0x10}, //  L arrow    [74]
	{0x22, 0x40, 0x48, 0x40, 0x22}, //  smile      [75]
	{0xFF, 0xFF, 0xFF, 0xFF, 0xFF}, //  5x8 block  [76]
};                                                                
// Font Map End

//pins and macros

#define HTport   PORTB
#define HTddr    DDRB
#define HTstrobe 3
#define HTclk    4
#define HTdata   5

#define HTclk0    HTport&=~(1<<HTclk)
#define HTclk1    HTport|= (1<<HTclk)
#define HTstrobe0 HTport&=~(1<<HTstrobe)
#define HTstrobe1 HTport|= (1<<HTstrobe)
#define HTdata0   HTport&=~(1<<HTdata)
#define HTdata1   HTport|= (1<<HTdata)
#define HTpinsetup() do{  HTddr |=(1<<HTstrobe)|(1<<HTclk)|(1<<HTdata); HTport|=(1<<HTstrobe)|(1<<HTclk)|(1<<HTdata);  }while(0)
        // set as output and all high


#define key1_r ((PIND&(1<<7))==0)
#define key2_r ((PIND&(1<<6))==0)
#define key3_r ((PIND&(1<<5))==0)
#define keysetup() do{ DDRD&=0xff-(1<<7)-(1<<6)-(1<<5); PORTD|=(1<<7)+(1<<6)+(1<<5); }while(0)  //input, pull up


byte leds[32];  //the screen array, 1 byte = 1 column, left to right, lsb at top. 


#define HTstartsys   0b100000000010 //start system oscillator
#define HTstopsys    0b100000000000 //stop sytem oscillator and LED duty    <default
#define HTsetclock   0b100000110000 //set clock to master with internal RC  <default
#define HTsetlayout  0b100001000000 //NMOS 32*8 // 0b100-0010-ab00-0  a:0-NMOS,1-PMOS; b:0-32*8,1-24*16   default:ab=10
#define HTledon      0b100000000110 //start LEDs
#define HTledoff     0b100000000100 //stop LEDs    <default
#define HTsetbright  0b100101000000 //set brightness b=0..15  add b<<1  //0b1001010xxxx0 xxxx:brightness 0..15=1/16..16/16 PWM
#define HTblinkon    0b100000010010 //Blinking on
#define HTblinkoff   0b100000010000 //Blinking off  <default
#define HTwrite      0b1010000000   // 101-aaaaaaa-dddd-dddd-dddd-dddd-dddd-... aaaaaaa:nibble adress 0..3F   (5F for 24*16)

//ADRESS: MSB first
//DATA: LSB first     transferring a byte (msb first) fills one row of one 8*8-matrix, msb left, starting with the left matrix
//timing: pull strobe LOW, bits evaluated at rising clock edge, strobe high
//commands can be queued: 100-ccccccccc-ccccccccc-ccccccccc-... (ccccccccc: without 100 at front)
//setup: cast startsys, setclock, setlayout, ledon, brightness+(15<<1), blinkoff



void HTsend(word data, byte bits) {  //MSB first
  word bit=((word)1)<<(bits-1);
  while(bit) {
    HTclk0;
    if (data & bit) HTdata1; else HTdata0;
    HTclk1;
    bit>>=1;
  }
}

void HTcommand(word data) {
  HTstrobe0;
  HTsend(data,12);
  HTstrobe1;
}

void HTsendscreen(void) {
  HTstrobe0;
  HTsend(HTwrite,10);
  for (byte mtx=0;mtx<4;mtx++)  //sending 8x8-matrices left to right, rows top to bottom, MSB left
    for (byte row=0;row<8;row++) {  //while leds[] is organized in columns for ease of use.
      byte q=0;
      for (byte col=0;col<8;col++)  q = (q<<1) | ( (leds[col+(mtx<<3)]>>row)&1 ) ;
      HTsend(q,8);
    }
  HTstrobe1;
}


void HTsetup() {  //setting up the display
  HTcommand(HTstartsys);
  HTcommand(HTledon);
  HTcommand(HTsetclock);
  HTcommand(HTsetlayout);
  HTcommand(HTsetbright+(8<<1));
  HTcommand(HTblinkoff);
}

void HTbrightness(byte b) {
  HTcommand(HTsetbright + ((b&15)<<1) );
}


// Global Variables
volatile byte sec=5;
byte sec0=200, minute, hour, day, month; word year;
byte setup=0;
byte mode=0;
byte src_changed = 0;
byte key1=0, key2=0, key3=0;
byte prev_key1, prev_key2, prev_key3;
byte hr_mode=0;
// End Global Variables


// =============================== CALANDER CODE ==========================



byte is_leap_year(const word yr) // Return 1 if leap year return 0 else.
{
	if (yr%400 == 0)
		return 1;
	else if (yr%100 == 0)
		return 0;
	else if (yr%4 == 0)
		return 1;
	else
		return 0;
}

byte last_day(const word yr, const byte m) // Return last date of given year and month
{
	if ( m == 2 ) {
		if ( is_leap_year(yr) ) 
			return 29;
		else
			return 28;
	}
	else if ( m < 8 ) { 
		if (m%2)
			return 31;
		else
			return 30;
	}
	else {
		if (m%2)
			return 30;
		else
			return 31;
	}
}

void incmonth() // Increase month
{
	if (month<12)
		month++;
	else {
		year++;	
		month=1;	
	}
}

void incday() // Increase day
{
	if (day<last_day(year, month))
		day++;
	else
	{
		incmonth();
		day=1;
	}
}

void renderyear(void) { // Render Year

	byte col = 0;
	
	leds[col++]=0;
	leds[col++]=0;
	
	for (byte i=0;i<5;i++) {
		if (setup && mode==2 && !key2 && !key3){ // Setup Year Mode
			if (sec%2)
				leds[col++]=pgm_read_byte(&font[(year/1000)][i]);
			else
	   	 		leds[col++]=0;
   	 	}
   	 	else {
   	 		leds[col++]=pgm_read_byte(&font[(year/1000)][i]);
	 	}
	}
	
	for (byte i=0;i<5;i++) {
		if (setup && mode==2 && !key2 && !key3){ // Setup Year Mode
			if (sec%2)
				leds[col++]=pgm_read_byte(&font[((year%1000)/100)][i]);
			else
	   	 		leds[col++]=0;
   	 	}
   	 	else {
   	 		leds[col++]=pgm_read_byte(&font[((year%1000)/100)][i]);
	 	}
	}
	
	for (byte i=0;i<5;i++) {
		if (setup && mode==2 && !key2 && !key3){ // Setup Year Mode
			if (sec%2)
				leds[col++]=pgm_read_byte(&font[((year%100)/10)][i]);
			else
	   	 		leds[col++]=0;
   	 	}
   	 	else {
   	 		leds[col++]=pgm_read_byte(&font[((year%100)/10)][i]);
	 	}
	}
	
	for (byte i=0;i<5;i++) {
		if (setup && mode==2 && !key2 && !key3){ // Setup Year Mode
			if (sec%2)
				leds[col++]=pgm_read_byte(&font[(year%10)][i]);
			else
	   	 		leds[col++]=0;
   	 	}
   	 	else {
   	 		leds[col++]=pgm_read_byte(&font[(year%10)][i]);
	 	}
	}
	
	leds[col++]=0;
	leds[col++]=0;
	leds[col++]=0;
	leds[col++]=0;
	leds[col++]=0;
	leds[col++]=0;
	leds[col++]=0;
	leds[col++]=0;
	leds[col++]=0;
	leds[col++]=0;
	
}
		

void renderdate(void) { // Render Date

	byte col = 0;
	
	leds[col++]=0;
		
	for (byte i=0;i<5;i++) {
		if (setup && mode==3 && !key2 && !key3){ // Setup Month Mode
			if (sec%2)
				leds[col++]=pgm_read_byte(&font[(month/10)][i]);
			else
	   	 		leds[col++]=0;
   	 	}
   	 	else {
   	 		leds[col++]=pgm_read_byte(&font[(month/10)][i]);
	 	}
   	 		
	}
	
	leds[col++]=0;
	
	for (byte i=0;i<5;i++) {
		if (setup && mode==3 && !key2 && !key3){ // Setup Month Mode
			if (sec%2)
				leds[col++]=pgm_read_byte(&font[(month%10)][i]);
			else
				leds[col++]=0;
		}
		else {
			leds[col++]=pgm_read_byte(&font[(month%10)][i]);
		}
	}
	
	leds[col++]=0;
	
	for (byte i=0;i<5;i++) {
		leds[col++]=pgm_read_byte(&font[69][i]);
	}
	
	leds[col++]=0;
	
	for (byte i=0;i<5;i++) {
		if (setup && mode==4 && !key2 && !key3){ // Setup Day Mode
			if (sec%2)
				leds[col++]=pgm_read_byte(&font[(day/10)][i]);
			else
				leds[col++]=0;
		}
		else {
			leds[col++]=pgm_read_byte(&font[(day/10)][i]);
		}
	}
	
	leds[col++]=0;
	
	for (byte i=0;i<5;i++) {
		if (setup && mode==4 && !key2 && !key3){ // Setup Day Mode
			if (sec%2)
				leds[col++]=pgm_read_byte(&font[(day%10)][i]);
			else
				leds[col++]=0;
		}
		else {
			leds[col++]=pgm_read_byte(&font[(day%10)][i]);
		}
	}
	
	leds[col++]=0;
	leds[col++]=0;


}

//------------------------------ Calender Code End ------------------------------------------------------------


//------------------------------------------------------------------------------------- CLOCK ------------------



inline void clocksetup() {  // CLOCK, interrupt every second
  ASSR |= (1<<AS2);    //timer2 async from external quartz
  TCCR2=0b00000101;    //normal,off,/128; 32768Hz/256/128 = 1 Hz
  TIMSK |= (1<<TOIE2); //enable timer2-overflow-int
  sei();               //enable interrupts
}


// CLOCK interrupt
ISR(TIMER2_OVF_vect) {     //timer2-overflow-int
  sec++;
}



void incsec(const byte add) {
  sec+=add;
  while (sec>=60) { 
    sec-=60;  minute++;
    while (minute>=60) {
      minute -= 60;  hour++;
      while (hour >=24) {
        hour-=24;  incday();
      }//24hours
    }//60min
  }//60sec
}


byte clockhandler(void) {
  if (sec==sec0) return 0;   //check if something changed
  sec0=sec;
  incsec(0);  //just carry over
  return 1;
}

byte is_src_changed(void) {
	if (src_changed) {
		src_changed = 0;
		return 1;
	}
	else
		return 0;
}


//-------------------------------------------------------------------------------------- clock render ----------

void renderclock(void) {
  byte col=0;
  byte tmp_hour;
  if (!hr_mode) // 12 Hr Mode
  {
	  if (hour>12) 
	  	tmp_hour = hour-12;
	  else if (hour==0)
	  	tmp_hour = 12;
	  else
	  	tmp_hour = hour;
  }
  else
  {
	  tmp_hour = hour;
  }
	    
   for (byte i=0;i<5;i++) {
	  if (setup && mode==0 && !key2 && !key3){ // Hour Setup Mode
		  if (sec%2)
		  	leds[col++]=pgm_read_byte(&font[(tmp_hour/10)][i]);
		  else
		  	leds[col++]=0;
  		}
	  else {
		  leds[col++]=pgm_read_byte(&font[(tmp_hour/10)][i]);
	  }
  }
  leds[col++]=0;
  for (byte i=0;i<5;i++) {
	  if (setup && mode==0 && !key2 && !key3){ // Hour Setup Mode
	   	  if (sec%2)
	   		leds[col++]=pgm_read_byte(&font[tmp_hour%10][i]);
	   	  else
	   	 	leds[col++]=0;
  	  	}
  	  else {
	  	  leds[col++]=pgm_read_byte(&font[tmp_hour%10][i]);
  	  }
  }
   	  	 	
  leds[col++]=0;
  //if (sec%2) {leds[col++]=0x66;leds[col++]=0x66;} else {leds[col++]=0; leds[col++]=0;} // Use this to blink colon
  leds[col++]=0x66;leds[col++]=0x66; // Use this to not blink colon
  leds[col++]=0;
  
  for (byte i=0;i<5;i++) {
	  if (setup && mode == 1 && !key2 && !key3) { // Minute Setup Mode
	  		if (sec%2)
		  		leds[col++]=pgm_read_byte(&font[(minute/10)][i]);
		  	else
		  		leds[col++]=0;
	  	}
	  else {
		  leds[col++]=pgm_read_byte(&font[minute/10][i]);
	  }
  }
  
  leds[col++]=0;
  for (byte i=0;i<5;i++) {
	  if (setup && mode == 1 && !key2 && !key3) { // Minute Setup Mode
	  		if (sec%2)
	  	  		leds[col++]=pgm_read_byte(&font[(minute%10)][i]);
	  	  	else
	  	  		leds[col++]=0;
  	  	}
	  else {
		  leds[col++]=pgm_read_byte(&font[(minute%10)][i]);
	  } 
  }
 
  leds[col++]=0;
  
  if (!hr_mode) { // 12 Hour mode
	  	if (hour>=12){ // PM
	   		for (byte i=0;i<5;i++) {
		   		leds[col++]=pgm_read_byte(&font['P'+upper_char_offset][i]);
	   		}
   		}
   		else {  // AM
  			for (byte i=0;i<5;i++) {
		   		leds[col++]=pgm_read_byte(&font['A'+upper_char_offset][i]);
	   		}	  			
  		}
  }
  else {
  
  	leds[col++]=0;
  	leds[col++]=0;
  	leds[col++]=sec;
  	leds[col++]=0;
  	leds[col++]=0;
  }
  
}

void render_hr_mode(void) { 
	byte col=0;
	
	leds[col++]=0;
	if (hr_mode) {
		for (byte i=0;i<5;i++) {
			leds[col++]=pgm_read_byte(&font[2][i]);
		}
		leds[col++]=0;
		for (byte i=0;i<5;i++) {
			leds[col++]=pgm_read_byte(&font[4][i]);
		}
	}
	else {
		for (byte i=0;i<5;i++) {
			leds[col++]=pgm_read_byte(&font[1][i]);
		}
		leds[col++]=0;
		for (byte i=0;i<5;i++) {
			leds[col++]=pgm_read_byte(&font[2][i]);
		}
	}
	leds[col++]=0;
	for (byte i=0;i<5;i++) {
		leds[col++]=pgm_read_byte(&font['h'+lower_char_offset][i]);
	}
	leds[col++]=0;
	for (byte i=0;i<5;i++) {
		leds[col++]=pgm_read_byte(&font['r'+lower_char_offset][i]);
	}
	for (byte i=0; i<8; i++) {
		leds[col++]=0;
	}
}

// ====================== Clock Code End =============================




int main(void) {  //==================================================================== main ==================

  HTpinsetup();
  HTsetup();
  keysetup();
  clocksetup();
  
  for (byte i=0;i<32;i++) leds[i]=0b01010101<<(i%2);  HTsendscreen();

  byte key1_duration = 0;
  byte clock_stat = 0;
  byte count=0;
  byte bright=3;
  byte brights[4]={0,2,6,15}; //brightness levels

  
  prev_key1 = 0;
  prev_key2 = 0;
  prev_key3 = 0;
  

  setup = 0;
  mode = 0;
  hour=1;minute=0;
  year=2012;month=1;day=1;
  
    	while(1){
	    	
	    	clock_stat = clockhandler();
	    		    	
	    	// Poll key with debounce
	    	if (key1_r + key2_r + key3_r) {
		    	if (count>250) {
			    	key1=key1_r;
			    	key2=key2_r;
			    	key3=key3_r;
			    	count=0;
		    	}
		    	else
		    	{
			    	count++;
		    	}
	    	}
	    	else
	    	{
		    	key1 = 0;
		    	key2 = 0;
		    	key3 = 0;
		    	count=0;
	    	} // Poll key end
	    	

	    	// Setup Mode
	    		if (setup){ // Set time
	  				if (key1) { // Change Mode

							if (prev_key1) {
						  		if (clock_stat) key1_duration++; 
					  		}
					  		else {
						  		key1_duration = 0;
						  		mode++;
					  		}
					  		
							if (key1_duration == 3) {
			  						setup=0; 
									mode=0;
									key1_duration = 0;
							}
					}
					else {
							key1_duration = 0;
					}

	  				
	  				if (mode==0) { // Hour
	  						if (key2 && !prev_key2) { // Increase Hour
	  							src_changed = 1;
	  							
	  							if (hour<23)
	  								hour++;
	  							else
	  								hour=0;
  							}
  							else if (key3 && !prev_key3) { // Decrease Hour
  								src_changed = 1;
  								
  								if (hour>0)
  									hour--;
  								else
  									hour=23;
							}
						}
					else if (mode==1) { // Minute
							if (key2 && !prev_key2) { // Increase Minute
								src_changed = 1;
								
								if (minute<59)
									minute++;
								else
									minute=0;
							}
							else if (key3 && !prev_key3) { // Decrease Minute
								src_changed = 1;
								
								if (minute>0)
									minute--;
								else
									minute=59;
							}
						}
					else if (mode==2) { // Year
							if (key2 && !prev_key2) { // Increase Year
								src_changed = 1;
								year++;
								
							}
							else if (key3 && !prev_key3) { // Decrease Month
								src_changed = 1;
								year--;
							}
						}					
					else if (mode==3) { // Month
							if (key2 && !prev_key2) { // Increase Month
								src_changed = 1;
								
								if (month<12)
									month++;
								else
									month=1;
							}
							else if (key3 && !prev_key3) { // Decrease Month
								src_changed = 1;
								
								if (month>1)
									month--;
								else
									month=12;
							}
						}
					else if (mode==4) { // Day
							if (key2 && !prev_key2) { // Increase Day
								src_changed = 1;
								
								if (day<last_day(year, month))
									day++;
								else
									day=1;
							}
							else if (key3 && !prev_key3) { // Decrease Day
								src_changed = 1;
								
								if (day>1)
									day--;
								else
									day=last_day(year, month);
							}
						}
					else if (mode==5) { // 24 hr Mode Select
						if ((key2 && !prev_key2) || (key3 && !prev_key3)) {
								if (hr_mode)
									hr_mode=0;
								else
									hr_mode=1;
							}
						}
				
					else { // Exit Setup
						mode = 0;
						setup = 0;
					}
				}
					    		
				else { // Normal Operation
					// Process Keys	
				    if (key1) {
					    
					  		if (prev_key1) {
						  		if (clock_stat) key1_duration++; 
					  		}
					  		else {
						  		key1_duration = 0;
					  		}
					  		
					  		
							if (key1_duration==3) { // Enter or Exit setup mode if key1 is pressed for 3 seconds.
								
								setup=1;
								mode=0;
								key1_duration = 0;
							} 
					}
					else if (key2 && !prev_key2) { // Display Date
							if (mode!=6)
								mode=6;
							else
								mode=0;
						}
					else if (key3 && !prev_key3) { //only once per press
							bright=(bright+1)%4; HTbrightness(brights[bright]);
						} 
					else {
							key1_duration = 0; 
						}
					// End Process Keys
				}
			
				
			
			// Update Diplay
						
			if (clock_stat || is_src_changed()) {	
				if (mode==6 || (setup && (mode == 3 || mode == 4 ))) {
					renderdate();
				}
				else if (setup && mode == 2) {
					renderyear();
				}
				else if (setup && mode == 5) {
					render_hr_mode();
				}
				else {
					renderclock();
				}
				HTsendscreen();
			}
			
			
			// Update key status.
			prev_key1 = key1;
			prev_key2 = key2;
			prev_key3 = key3;
			
		  	
		}
			
    return(0);
}//main
