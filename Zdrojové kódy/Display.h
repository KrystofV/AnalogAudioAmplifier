/*
 * Display.h
 *
 * Created: 13.01.2024 23:08:40
 *  Author: vacek
 */ 


#ifndef DISPLAY_H_
#define DISPLAY_H_
#include "i2c_master.h"
#include "Font.h"
#include <stdio.h>

#define OLED_I2c_ADDRESS       0x3C
#define COMMAND_REG            0x80
#define DATA_REG               0x40
#define ON_CMD                 0xAF
#define NORMAL_DISPLAY_CMD     0xA6
#define PAGE_ADDRESSING_MODE   0x02

class Display{
	private:
	void writeCommand(uint8_t command) {
		uint8_t arr[] = {COMMAND_REG,command};
		i2cMasterSend(OLED_I2c_ADDRESS, arr,2);
		

	}

	void writeData(uint8_t data) {
		uint8_t arr[] = {DATA_REG,data};
		i2cMasterSend(OLED_I2c_ADDRESS, arr,2);
		

	}
	void setCursor(uint8_t X, uint8_t Y) {
		writeCommand(0x00 + (X & 0x0F));        //set column lower address
		writeCommand(0x10 + ((X >> 4) & 0x0F)); //set column higher address
		writeCommand(0xB0 + Y);                 //set page address

	}
	void printBig(int k,int pos){
		for(int i = 0;i<3;i++){
			setCursor(pos, 2+i);
			for(int j=0;j<char_width[k];j++){
				writeData(char_addr[k][i*char_width[k]+j]);
			}
		}
	}
	
	
	public:
	Display(){
		writeCommand(0xAE);
		writeCommand(0xD5);
		writeCommand(0x80);
		writeCommand(0xA8);
		writeCommand(0x3F);
		writeCommand(0xD3);
		writeCommand(0x00);
		writeCommand(0x40);
		writeCommand(0x8D);
		writeCommand(0x14);
		writeCommand(0xA1);
		writeCommand(0xC8);
		writeCommand(0xDA);
		writeCommand(0x12);
		writeCommand(0x81);
		writeCommand(0xCF);
		writeCommand(0xD9);
		writeCommand(0xF1);
		writeCommand(0xDB);
		writeCommand(0x40);
		writeCommand(0xA4);
		writeCommand(0xA6);
		writeCommand(0xAF);
		
		clearFullDisplay();
	}
	
	
	void clearFullDisplay() {
		for (uint8_t page = 0; page < 8; page++) {

			setCursor(0, page);
			for (uint8_t column = 0; column < 128; column++) { //clear all columns
				writeData(0x00);
			}

		}
		setCursor(0, 0);
	}
	
	void printText(char * str){
		for(int i=0;str[i]!= '\0';i++){
			if(str[i]>='0' && str[i]<='9')
			printBig(str[i]-'0',i*16);
			else if (str[i]>='a' && str[i]<='z')
			printBig(str[i]-'a'+11,i*16);
			else if (str[i]>='A' && str[i]<='Z')
			printBig(str[i]-'A'+11,i*16);
			else
			continue;
			
		}

	}
	
	};




#endif /* DISPLAY_H_ */