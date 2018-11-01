#include "LCD.h"
#include "Keypad.h"
#include "stm32l476xx.h"
#include <stdint.h>
#include <stdbool.h>

int location[2];//columns then rows of the keypad
int ColumnResultIDR;
int prevColumnResultIDR;
int Digit;//used in displayMessage
uint8_t message[6];
int charcount = 0;

void initKeypad()
{
    /*
    enable clock GPIO E
    for the output pins PE[10:13]
    set MODER to output mode            		 01
    set otyper to output open drain 	 1
    set pupdr to no pull up/pull down 00
    for the input pins PA[0:3]
    set MODER to input mode 00
    set pupdr to pull up		 01
    */
	 //Enable clock GPIOE
  			  Digit = 0;
   	 RCC->AHB2ENR |= RCC_AHB2ENR_GPIOEEN;
    //for the output pins PE[10:13]
      //set MODER to output mode		 01
   	 GPIOE->MODER |= GPIO_MODER_MODE10_0; //pin 10 bit 0 want value 1
   	 GPIOE->MODER &= ~GPIO_MODER_MODE10_1;//pin 10 bit 1 want value 0
   	 GPIOE->MODER |= GPIO_MODER_MODE11_0; //pin 11 bit 0 want value 1
   	 GPIOE->MODER &= ~GPIO_MODER_MODE11_1;// pin 11 bit 1 want value 0
   	 GPIOE->MODER |= GPIO_MODER_MODE12_0; // pin 12 bit 0 want value 1
   	 GPIOE->MODER &= ~GPIO_MODER_MODE12_1;// pin 12 bit 1 want value 0
   	 GPIOE->MODER |= GPIO_MODER_MODE13_0; // pin 13 bit 0 want value 1
   	 GPIOE->MODER &= ~GPIO_MODER_MODE13_1;// pin 13 bit 1 want value 0
    //set otyper to output open drain  
   	 GPIOE->OTYPER |= GPIO_OTYPER_OT_10;//pin 10 want value 1
   	 GPIOE->OTYPER |= GPIO_OTYPER_OT_11;//pin 11 want value 1
   	 GPIOE->OTYPER |= GPIO_OTYPER_OT_12;//pin 12 want value 1
   	 GPIOE->OTYPER |= GPIO_OTYPER_OT_13;//pin 13 want value 1
    //set pupdr to no pull up/pull down 00
		 GPIOE->PUPDR &= ~GPIO_PUPDR_PUPD10_0;//pin 10 bit 0 want value 0
   	 GPIOE->PUPDR &= ~GPIO_PUPDR_PUPD10_1;//pin 10 bit 1 want value 0
   	 GPIOE->PUPDR &= ~GPIO_PUPDR_PUPD11_0;//pin 11 bit 0 want value 0
   	 GPIOE->PUPDR &= ~GPIO_PUPDR_PUPD11_1;//pin 11 bit 1 want value 0
   	 GPIOE->PUPDR &= ~GPIO_PUPDR_PUPD12_0;//pin 12 bit 0 want value 0
   	 GPIOE->PUPDR &= ~GPIO_PUPDR_PUPD12_1;//pin 12 bit 1 want value 0
   	 GPIOE->PUPDR &= ~GPIO_PUPDR_PUPD13_0;//pin 13 bit 0 want value 0
   	 GPIOE->PUPDR &= ~GPIO_PUPDR_PUPD13_1;//pin 13 bit 1 want value 0
    //for the input pins PA[0:3]
    //set MODER to input mode 00
   	 GPIOA->MODER &= ~GPIO_MODER_MODE0_0;//pin 0 bit 0 want value 0
   	 GPIOA->MODER &= ~GPIO_MODER_MODE0_1;//pin 0 bit 1 want value 0
   	 GPIOA->MODER &= ~GPIO_MODER_MODE1_0;//pin 1 bit 0 want value 0
   	 GPIOA->MODER &= ~GPIO_MODER_MODE1_1;//pin 1 bit 1 want value 0
   	 GPIOA->MODER &= ~GPIO_MODER_MODE2_0;//pin 2 bit 0 want value 0
   	 GPIOA->MODER &= ~GPIO_MODER_MODE2_1;//pin 2 bit 1 want value 0
   	 GPIOA->MODER &= ~GPIO_MODER_MODE3_0;//pin 3 bit 0 want value 0
   	 GPIOA->MODER &= ~GPIO_MODER_MODE3_1;//pin 3 bit 1 want value 0
    //set pupdr to pull up		 01
		 GPIOA->PUPDR |= GPIO_PUPDR_PUPD0_0;//pin 0 bit 0  want value 1
   	 GPIOA->PUPDR &= ~GPIO_PUPDR_PUPD0_1;//pin 0 bit 1 want value 0
   	 GPIOA->PUPDR |= GPIO_PUPDR_PUPD1_0;//pin 1 bit 0 want value 1
   	 GPIOA->PUPDR &= ~GPIO_PUPDR_PUPD1_1;//pin 1 bit 1 want value 0
   	 GPIOA->PUPDR |= GPIO_PUPDR_PUPD2_0;//pin 2 bit 0 want value 1
   	 GPIOA->PUPDR &= ~GPIO_PUPDR_PUPD2_1;//pin 2 bit 1 want value 0
   	 GPIOA->PUPDR |= GPIO_PUPDR_PUPD3_0;//pin 3 bit 0 want value 1
   	 GPIOA->PUPDR &= ~GPIO_PUPDR_PUPD3_1;//pin 3 bit 1 want value 0

    //row most recently pressed. contains values of 0 to 4 zero means no rows selected. when button is pressed only store 1:4
    location[0] = 0;
    //col mos recently pressed. contains values of 0 to 4 zero means no colums selected. when button is pressed only store values from 1 to 4
    location[1] = 0;
    prevColumnResultIDR = 0;
}

void scanKeypad()
{
    /*
    set output pins PE[10:13] ODR to 0000
    read values of input pins PA[0:3]
    
    if one of the pins is zero and it has not changed do not allow another press(dont allow long button press to print multiple #s)
    if one of the pins is zero begin the looping through the scan process
    
    after scanning wait for an amount of time to debounce
    */

    GPIOE->ODR = 0x00000000;
    //possibly insert a delay here to make sure we get a reading*********************
    ColumnResultIDR = GPIOA->IDR;
  ColumnResultIDR &= 0x0000000F;  

    //procede only if the button is not being held
   	 switch(ColumnResultIDR){
  		 
   		 case 0x0000000E:
       		 location[0] = 1;
       		 findLocation();
   					   debounce(100);
       		 break;

   		 case 0x0000000D:
       		 location[0] = 2;
       		 findLocation();
   						 debounce(100);
       		 break;
  	 
   		 case 0x0000000B:
       		 location[0] = 3;
       		 findLocation();
   					   debounce(100);
       		 break;
  		 
   		 case 0x00000007:
       		 location[0] = 4;
       		 findLocation();
       		 debounce(100);
   						 break;
   	 }
   		 //display message will pull the data from location[] and display the correct character in the next slot
   		 char character = gridToChar();
   		 displayMessage(character);
   		 //reset the location array so that it is not re-used
   		 location[0] = 0;
   		 location[1] = 0;
   	 prevColumnResultIDR = ColumnResultIDR;
}


void findLocation(){
/*
    Scan process()
    set output pins PE[10:13] ODR to 0000
    read values of input pins PA[0:3]
    save the index of the zero input pin as col. this is the column number
    
    set output pins PE[10:13] ODR to 1000
    read values of input pins PA[0:3]
    if the PE[col] = 0 then the button pressed was 1,col
    display and return
    else
   	 keep going
  	 
    repeat for each config 0100, 0010, 0001
0000 0000 0000 0000 0011 1000 0000 0000, = 0x00003800
0000 0000 0000 0000 0011 0100 0000 0000, = 0x00003400
0000 0000 0000 0000 0010 1100 0000 0000, = 0x00002C00
0000 0000 0000 0000 0001 1100 0000 0000  = 0x00001C00
0000 0000 0000 0000 0011 1100 0000 0000  = 0x00003C00
    */
int rowTestIDR = 0x0;
int rowTestODR = 0x0;

    //GPIOE->ODR = 0x00002000;
    GPIOE->ODR = 0x00003800;
    rowTestIDR = GPIOA->IDR;
    rowTestIDR &= 0x0000000F;
		
		rowTestODR = GPIOE->ODR;
		rowTestODR &= 0x00003C00;
		
		/*
		if (rowTestIDR == ColumnResultIDR)
		{
			if(rowTestIDR == 14)
			{
				location[1] = 1;
			}
			
			if(rowTestIDR == 13)
			{
				location[1] = 2;
			}
			
			if(rowTestIDR == 11)
			{
				location[1] = 3;
			}
			
			if(rowTestIDR == 7)
			{
				location[1] = 4;
			}
			
		}
		*/
		
    //if the IDR reads the same value that it was given originally after setting the ODR then we have found the correct row
   
	 if(rowTestIDR == ColumnResultIDR)
    {
   	 if(rowTestODR == 0x00003800)
			{
				location[1] = 1;
				return;
			}
			
			if(rowTestIDR == 0x00003400)
			{
				location[1] = 2;
				return;
			}
			
			if(rowTestIDR == 0x00002C00)
			{
				location[1] = 3;
				return;
			}
			
			if(rowTestIDR == 0x00001C00)
			{
				location[1] = 4;
				return;
			}
   	 return;
    }
    
    //GPIOE->ODR = 0x00001000;
    GPIOE->ODR = 0x00003400;
    rowTestIDR = GPIOA->IDR;
    rowTestIDR &= 0x0000000F;
    //if the IDR reads the same value that it was given originally after setting the ODR then we have found the correct row
    if(rowTestIDR == ColumnResultIDR)
    {
   	 if(rowTestODR == 0x00003800)
			{
				location[1] = 1;
				return;
			}
			
			if(rowTestIDR == 0x00003400)
			{
				location[1] = 2;
				return;
			}
			
			if(rowTestIDR == 0x00002C00)
			{
				location[1] = 3;
				return;
			}
			
			if(rowTestIDR == 0x00001C00)
			{
				location[1] = 4;
				return;
			}
   	 return;
    }
    
    //GPIOE->ODR = 0x00000800;
    GPIOE->ODR = 0x00002C00;
    rowTestIDR = GPIOA->IDR;
    rowTestIDR &= 0x0000000F;
		
    //if the IDR reads the same value that it was given originally after setting the ODR then we have found the correct row
    if(rowTestIDR == ColumnResultIDR)
    {
   	 if(rowTestODR == 0x00003800)
			{
				location[1] = 1;
				return;
			}
			
			if(rowTestIDR == 0x00003400)
			{
				location[1] = 2;
				return;
			}
			
			if(rowTestIDR == 0x00002C00)
			{
				location[1] = 3;
				return;
			}
			
			if(rowTestIDR == 0x00001C00)
			{
				location[1] = 4;
				return;
			}
   	 return;
    }
    
    
    //GPIOE->ODR = 0x00000400;
    GPIOE->ODR = 0x00001C00;
    rowTestIDR = GPIOA->IDR;
    rowTestIDR &= 0x0000000F;
    //if the IDR reads the same value that it was given originally after setting the ODR then we have found the correct row
     if(rowTestIDR == ColumnResultIDR)
    {
   	 if(rowTestODR == 0x00003800)
			{
				location[1] = 1;
				return;
			}
			
			if(rowTestIDR == 0x00003400)
			{
				location[1] = 2;
				return;
			}
			
			if(rowTestIDR == 0x00002C00)
			{
				location[1] = 3;
				return;
			}
			
			if(rowTestIDR == 0x00001C00)
			{
				location[1] = 4;
				return;
			}
   	 return;
    }
		
    
    
}

char gridToChar()
{
    if(location[0] == 1)
    {
   		 switch(location[1]){
       		 case 1://1
             		 return '1';  
       		 case 2://4
       		 return '4';
       		 case 3://7
             		 return '7';
       		 case 4://*
       		 return 'S';
   	 }
    }
    
   	 if(location[0] == 2)
    {
   		 switch(location[1]){
       		 case 1://2
           		 return '2';
       		 case 2://5
       		 return '5';
       		 case 3://8
           		 return '8';
       		 case 4://0
       		 return '0';
   	 }
    }
    
   	 if(location[0] == 3)
    {
   		 switch(location[1]){
       		 case 1://3
           		 return '3';
       		 case 2://6
       		 return '6';
       		 case 3://9
             		 return '9';
       		 case 4://#
       		 return 'H';
   	 }
    }
    
   	 if(location[0] == 4)
    {
   		 switch(location[1]){
       		 case 1://A
              		 return 'A';
       		 case 2://B
      		 return 'B';
       		 case 3://C
           		 return 'C';
       		 case 4://D
      		 return 'D';
   	 }
    }
	return 'z';//just for debug
}

void debounce(int duration)
{
  for (int i =0 ; i < duration; i++)
    {
   	 (void)0;
    }
}

void displayMessage(char newChar)
{
    //int dispLength = sizeof(message);
   // if (dispLength <= 6)
    
	if(charcount < 6)
	{
    message[charcount] = (uint8_t)newChar;
		charcount++;
    uint8_t* lcdOutput;
    lcdOutput = message;
   	 for (uint8_t i=0; i < 6; i++)
   	 {
   		 LCD_WriteChar(lcdOutput, false, false, i);
			 lcdOutput++;
   	 }
	 }
}



//void clear()
//{
    //uint8_t erase = 0
    //for (uint8_t i=0; i < 6; i++)
   //     {
   //		 LCD_Writechar(erase , false, false, i);
   //     }
//}









