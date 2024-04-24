/*
 * Zesilovac++.cpp
 *
 * Created: 08.12.2023 18:34:50
 * Author : vacek
 */ 
#define F_CPU 16000000UL
#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>
#include <C:\Users\vacek\Documents\Atmel Studio\7.0\Zesilovac++\Zesilovac++\Encoder.h>
#include <C:\Users\vacek\Documents\Atmel Studio\7.0\Zesilovac++\Zesilovac++\Max7219.h>
#include <C:\Users\vacek\Documents\Atmel Studio\7.0\Zesilovac++\Zesilovac++\i2c_master.h>
#include <C:\Users\vacek\Documents\Atmel Studio\7.0\Zesilovac++\Zesilovac++\TDA7313.h>
#include <C:\Users\vacek\Documents\Atmel Studio\7.0\Zesilovac++\Zesilovac++\Radio.h>
#include <C:\Users\vacek\Documents\Atmel Studio\7.0\Zesilovac++\Zesilovac++\Display.h>
#include <C:\Users\vacek\Documents\Atmel Studio\7.0\Zesilovac++\Zesilovac++\remote.h>

#define BAUD_PRESCALE (((F_CPU / (9600 * 16UL))) - 1)



#define BIT_IS_CLEAR(byte,position)\
((byte & 1<<position) == 0 ? 1 : 0)
 
 bool previous[4] = {true,true,true,true};
 
 void update_encoders(Encoder * encoders,TDA7313 &amplifier,Radio &radio,Max7219 &max,Display &display){
	 
	 int values[4];
	 for(int i=0;i<4;i++){
		 values[i] = encoders[i].getValue();
	 }
	 amplifier.toggleAll(values);
	 amplifier.getAll(values);
	 
	 char buffer [50];
	 for (int i=0;i<4;i++)
	 {
		 max.update(values[i],3-i);
	 }
	 
	 //max.updateAll(values);
	 
	 if(BIT_IS_CLEAR(PINB,0) && previous[0] == true){
		 amplifier.toggleInput();
		 sprintf(buffer,"%d",amplifier.getInput());
		 display.printText(buffer);
	 }
	 if(BIT_IS_CLEAR(PINB,1)&& previous[1] == true){
		 radio.nextStation(true);
		 sprintf(buffer,"%d",radio.selectedStation());
		 display.printText(buffer);
	 }
	 if(BIT_IS_CLEAR(PINB,2)&& previous[2] == true){
		 radio.nextStation(false);
		 sprintf(buffer,"%d",radio.selectedStation());
		 display.printText(buffer);
	 }
	 previous[0] = BIT_IS_CLEAR(PINB,0);
	 previous[1] = BIT_IS_CLEAR(PINB,1);
	 previous[2] = BIT_IS_CLEAR(PINB,2);
	 
 }
 void update_remote(TDA7313 &amplifier,Radio &radio,Max7219 &max,Display &display){
	 uint8_t cmd=GetRemoteCmd(1);
	 char buffer[10];
	 switch(cmd){
		case 0:
			amplifier.toggleVolume(1);
			break;
		case 4:
			amplifier.toggleVolume(-1);
			break;
		case 1:
			amplifier.toggleBass(1);
			break;
		case 5:
			amplifier.toggleBass(-1);
			break;
		case 2:
			amplifier.toggleTreble(1);
			break;
		case 6:
			amplifier.toggleTreble(-1);
			break;
		case 3:
			radio.nextStation(true);
			sprintf(buffer,"%d",radio.selectedStation());
			display.printText(buffer);
			break;
		case 7:
			radio.nextStation(false);
			sprintf(buffer,"%d",radio.selectedStation());
			display.printText(buffer);
			break;
		case 11:
			amplifier.toggleInput();
			sprintf(buffer,"%d",amplifier.getInput());
			display.printText(buffer);
			break;
		default:
			return;
	 }
	 int values[4];
	 amplifier.getAll(values);
	  
	 for (int i=0;i<4;i++)
	 {
		 max.update(values[i],3-i);
	 }
 }

int main(void)
{
	_delay_ms(1000);
	RemoteInit();
	DDRA = 0x00;
	PORTA = 0xFF;
	i2cMasterInit(I2C_SCL_FREQUENCY_100);
	Max7219 max = Max7219(8,8);
	TDA7313 amplifier = TDA7313();
	Radio radio = Radio();
	Display display = Display();
	Encoder encoders [] = {	Encoder(&PINA,PINA0,PINA1),
							Encoder(&PINA,PINA2,PINA3),
							Encoder(&PINA,PINA4,PINA5),
							Encoder(&PINA,PINA6,PINA7)};
	for (int i=0;i<4;i++)
	{
		max.update(0,3-i);
	}
	char buffer[10];
	sprintf(buffer,"%d",radio.selectedStation());
	display.printText(buffer);
	DDRB &= 0b11110000;
	PORTB |= 0b00001111;
	/* Insert application code here, after the board has been initialized. */
    while (1) 
    {
		update_encoders(encoders,amplifier,radio,max,display);
		update_remote(amplifier,radio,max,display);
		for(int i=0;i<4;i++)
			encoders[i].readEncoder();
    }
}

