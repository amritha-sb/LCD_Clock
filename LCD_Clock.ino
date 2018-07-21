
#include<avr/io.h>
#include<avr/interrupt.h>
#include<util/delay.h>

#define LCD_CLEARDISPLAY 0x01
#define LCD_ENTRYMODESET 0x04
#define LCD_DISPLAYCONTROL 0x08
#define LCD_FUNCTIONSET 0x20
#define LCD_ENTRYLEFT 0x02
#define LCD_ENTRYSHIFTDECREMENT 0x00
#define LCD_DISPLAYON 0x04
#define LCD_CURSOROFF 0x00
#define LCD_BLINKOFF 0x00
#define LCD_8BITMODE 0x10
#define LCD_4BITMODE 0x00
#define LCD_2LINE 0x08
#define LCD_1LINE 0x00
#define LCD_5x10DOTS 0x04
#define LCD_5x8DOTS 0x00

class LiquidCrystal {
public:
  LiquidCrystal(uint8_t rs, uint8_t rw, uint8_t enable,
    uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3);
void init(uint8_t fourbitmode, uint8_t rs, uint8_t rw, uint8_t enable,
      uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3,
      uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7);
    
    
  void begin(uint8_t cols, uint8_t rows, uint8_t charsize = LCD_5x8DOTS);

  void clear();
  void display();
  virtual size_t write(uint8_t);
  void command(uint8_t);
private:
  void send(uint8_t, uint8_t);
  void write4bits(uint8_t);
  void pulseEnable();
  
  
  uint8_t _rs_pin; 
  uint8_t _rw_pin;
  uint8_t _enable_pin; 
  uint8_t _data_pins[8];
  
  uint8_t _displayfunction;
  uint8_t _displaycontrol;
  uint8_t _displaymode;

  uint8_t _initialized;

  uint8_t _numlines,_currline;
};
LiquidCrystal::LiquidCrystal(uint8_t rs, uint8_t rw, uint8_t enable,
           uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3)
{
  init(1, rs, rw, enable, d0, d1, d2, d3, 0, 0, 0, 0);
}
void LiquidCrystal::init(uint8_t fourbitmode, uint8_t rs, uint8_t rw, uint8_t enable,
       uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3,
       uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7)
{
  _rs_pin = rs;
  _rw_pin = rw;
  _enable_pin = enable;
  
  _data_pins[0] = d0;
  _data_pins[1] = d1;
  _data_pins[2] = d2;
  _data_pins[3] = d3; 
  _data_pins[4] = d4;
  _data_pins[5] = d5;
  _data_pins[6] = d6;
  _data_pins[7] = d7; 
  DDRD|=(1<<(rs-8));
 DDRD|=(1<<(rw-8)); 
  DDRD|=(1<<(enable-8));
  if (fourbitmode)
    _displayfunction = LCD_4BITMODE | LCD_1LINE | LCD_5x8DOTS;
  else 
    _displayfunction = LCD_8BITMODE | LCD_1LINE | LCD_5x8DOTS;
  
  begin(16, 1);
}

void LiquidCrystal::begin(uint8_t cols, uint8_t lines, uint8_t dotsize) {
  if (lines > 1) {
    _displayfunction |= LCD_2LINE;
  }
  _numlines = lines;
  _currline = 0;
  if ((dotsize != 0) && (lines == 1)) {
    _displayfunction |= LCD_5x10DOTS;
  }
  _delay_us(50000); 
  PORTD&=~(1<<(_rs_pin-8));
  PORTD&=~(1<<(_enable_pin-8));
  PORTD&=~(1<<(_rw_pin-8));
  if (! (_displayfunction & LCD_8BITMODE)) {
    write4bits(0x03);
    _delay_us(4500); // wait min 4.1ms
    write4bits(0x03);
    _delay_us(4500); // wait min 4.1ms
    write4bits(0x03); 
    _delay_us(150);
    write4bits(0x02); 
  } else {
    command(LCD_FUNCTIONSET | _displayfunction);
    _delay_us(4500); 
    command(LCD_FUNCTIONSET | _displayfunction);
    _delay_us(150);
    command(LCD_FUNCTIONSET | _displayfunction);
  }
  command(LCD_FUNCTIONSET | _displayfunction);  
  _displaycontrol = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;  
  display();
  clear();
  _displaymode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
  command(LCD_ENTRYMODESET | _displaymode);

}

void LiquidCrystal::clear()
{
  command(LCD_CLEARDISPLAY);
  _delay_us(2000);  }
  
void LiquidCrystal::display() {
  _displaycontrol |= LCD_DISPLAYON;
  command(LCD_DISPLAYCONTROL | _displaycontrol);
}
inline void LiquidCrystal::command(uint8_t value) {
  send(value, LOW);
 PORTD&=~(1<<(_rs_pin-8));
 }

inline size_t LiquidCrystal::write(uint8_t value) {
  PORTD|=(1<<(_rs_pin-8));
  send(value, HIGH);
  return 1;
}
void LiquidCrystal::send(uint8_t value, uint8_t mode) {
  
      PORTD&=~(1<<(_rw_pin-8)); 
      write4bits(value>>4);
      write4bits(value);
    }

void LiquidCrystal::pulseEnable(void) {
     PORTD&=~(1<<(_enable_pin-8));
    _delay_us(1);   
      PORTD|=(1<<(_enable_pin-8));
    _delay_us(1);    // enable pulse must be >450ns
    PORTD&=~(1<<(_enable_pin-8));
    _delay_us(100);   // commands need > 37us to settle
  }
  void LiquidCrystal::write4bits(uint8_t value) {
    for (int i = 0; i < 4; i++) {
      DDRB|=(1<<_data_pins[i]);
      uint8_t v=(value>>i)&(0x01);
      if(v==1)
      PORTB|=(1<<_data_pins[i]);
      else
       PORTB&=~(1<<_data_pins[i]);
    }
  pulseEnable();
}
LiquidCrystal lcd{12,13,15,5,4,3,2};
int ch,ch1,ch2,h1,h2=0,m2=0,s2=0,m1,cm1,s1,a=1,f=2,alarm=0,z=0;
int cm=0;
int s=5;
int m=2;
int h=0;
int dd=15,mm=6,yy=2016;
char b[4];
char c[4];
char d[4];
int temp;
int flag=0,flag1=0,flag2=0,flag3=0,l=0,t=0,w=0;
void pwm_init()
{
  TCCR0A|=(1<<COM0A1)|(1<<WGM01)|(1<<WGM00);// non invertong mode and fast pwm
  TCCR0B|=(1<<CS00);//prescaller1
}
void adc_init()
{
  ADMUX |= (1 << REFS0);
  ADCSRA |= ((1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0));
}
int adc_read(int ch)
{ch=ch&0b00000111;
 ADMUX=(ADMUX&0xF8)|ch;
  ADCSRA |= (1 << ADSC);
  while (ADCSRA != (ADCSRA | (1 << ADIF)));
  ADCSRA |= (1 << ADIF); 
  return ADC;
}
void printlcd(char *s)
{
 while(*s)
 {
   lcd.write(*s);
   *s++;
 }
}
void fun(int k)
{int l= k/10;
if(l==0)
printlcd("0");
  
}
void timer_int()
{  
  TCCR1B=(1<<WGM12)|(1<<CS11)|(1<<CS10);
  OCR1A=250;
  TIMSK1|=(1<<OCIE1A);
  }
void fix()
{ if(ch>1000)
   flag=1;
   if((ch<600)&&(flag==1))
   {
    flag=0;
    l++;
   }
   if(ch<20)
   flag1=1;
    if((ch>400)&&(flag1==1))
   {
    flag1=0;
    l--;
   }
   if((l>5)||(l<0))
   l=0;
   
}
void increase()
{
  switch(l)
{
case 0:h++;
       break;
case 1:m++;
       break;
case 2:s++;
       break;
case 3:dd++;
       break;
case 4:mm++;
       break;
case 5:yy++;
       break;
}
}
void decrease()
{switch(l)
{
case 0:h--;
       break;
case 1:m--;
       break;
case 2:s--;
       break;
case 3:dd--;
       break;
case 4:mm--;
       break;
case 5:yy--;
       break;
}
}
void inc1()
{switch((l%3))
{
case 0:h2++;
       break;
case 1:m2++;
       break;
case 2:s2++;
}
}
void dec1()
{switch((l%3))
{
case 0:h2--;
       break;
case 1:m2--;
       break;
case 2:s2--;
}
}
void edit1(int a)
  {
     if(ch1>1000)
   flag2=1;
   if((ch1<600)&&(flag2==1))
   {
    flag2=0;
    if(a==1)
  increase();
  if(a==2)
  inc1();
   }
   if(ch1<10)
   flag3=1;
    if((ch1>400)&&(flag3==1))
   {
    flag3=0;
    if(a==1)
    decrease();
   if(a==2)
   dec1();
   }
  }
  void disp(int q,char p[4])
{ if(q<0)
  q=0; 
  fun(q);
  itoa(q,p,10);
  printlcd(p);
}
void feb()
{if(yy%4==0)
   {if(yy%100==0)
      {if(yy%400==0)
        {if(dd>29)
          {
            dd=1;
            mm++;
           }}
        else
         { if(dd>28)
         {
          dd=1;
          mm++;
         }}
      }
    else
    {if(dd>29)
    {
      dd=1;
      mm++;
    }
   }
   }
  else
    {if(dd>28)
    {
      dd=1;
      mm++;
    }}
}
void monthcheck()
{
if(mm%2==0)
 {if(mm==2)
   feb();
  else if(mm<7)
    {if(dd>30)
      { dd=1;
        mm++;
      }}
  else
   {
     if(dd>31)
      { dd=1;
        mm++;
      }}
  }
else
  {if(mm<=7)
    {if(dd>31)
      { dd=1;
        mm++;
      }
      }
   else
   {
     if(dd>30)
      { dd=1;
        mm++;
      }}
    
  }
}void clearscreen()
{
 lcd.command(0x80);
 for(int k=0;k<32;k++)
 lcd.write(' ');
 lcd.command(0x80);
 }
void normal_trig()
{lcd.command(0x80+4);
  disp(h,d);
 printlcd(":");
 disp(m,b);
 printlcd(":");
 disp(s,c);
printlcd("     "); 
lcd.clear();
lcd.command(0x80+64);
disp(dd,d);
printlcd(":");
 disp(mm,b);
 printlcd(":");
 disp(yy,c);
 printlcd("  ");
itoa(temp,b,10);
printlcd(b);
lcd.write(char(223));
printlcd("C ");
 disp((t%3),c);
if(alarm==1)
{
  if((h==h2)&&(m==m2))
   w=1;
if(w==1)
 {switch(s2)
 {case 0:TCCR0B=(1<<CS00);
 case 1: TCCR0B=(1<<CS01)|(1<<CS00);
         break;
case 2:  TCCR0B=(1<<CS02);
 
}
OCR0A=z;
z+=6;
if(s>30)
{
  OCR0A=0;
  w=0;
  alarm=0;
  z=0;
}}}
}

void stopwatch_trig()
{ 
lcd.command(0x80+1);
printlcd("STOPWATCH MODE  ");
lcd.clear();
lcd.command(0x80+64);
printlcd("    ");
 disp(m1,d);
 printlcd(":");
 disp(s1,b);
 printlcd("    ");
}
void alarm_trig()
{alarm=1;
lcd.clear();
lcd.command(0x80+2);
printlcd(" ALARM MODE    ");
 lcd.clear();
 lcd.command(0x80+64);
 printlcd("     ");
 disp(h2,d);
 printlcd(":");
 disp(m2,b);
 if(s2>2)
 s2=0;
 printlcd("   ");
 disp(s2,b);
}
int main()
{lcd.begin(16,2);
  sei();
  adc_init();
  timer_int();
  pwm_init();
  EIMSK|=(1<<INT0)|(1<<INT1);
  EICRA|=0b00001111;
  DDRB|=0b11111111; //port b output
   DDRD|=0b01000000;
  while(1)
  {ch=adc_read(1);//to fix
    ch1=adc_read(0);//to increase -decrease
    ch2=adc_read(2);
    temp=(
      ch2-4)/1023.0*500;
 fix();
 if(t%3==0)
 edit1(1);
 if((t-2)%3==0)
  edit1(2);
  }
}
ISR(TIMER1_COMPA_vect)
{cm++;
if((a%2==1)&&(f==1))
{cm1++;
if(cm1==1000)
{
  s1++;
cm1=0;
if(s1==60)
{
  m1++;
  s1=0;
}
}}
if(cm==1000)
 {clearscreen();
 s++;
 cm=0;
 if(s>=60)
 {s=0;
 m++;
 }
 if(m>=60)
 {
  m=0;
  h++;
 }
if(h>=24)
 {
  dd++;
  h=0;
 }
 if(dd>27)
 monthcheck();
 if(mm>12)
 {
  mm=1;
  yy++;
 }
 if((t-2)%3==0)
 alarm_trig();
if(t%3==0)
normal_trig();
if((t-1)%3==0)
stopwatch_trig();
 }
 }
ISR(INT0_vect)
{PORTB^=(1<<PINB5);
t++;
EIFR|=(1<<INTF0);
}
ISR(INT1_vect)
{
if((t-1)%3==0)
{
a++;
if(a%2==1)
{cm1=0;
s1=0;
m1=0;
f=1;
}}
else 
f=0;
EIFR|=(1<<INTF1);
}

