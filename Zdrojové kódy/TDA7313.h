/*
* TDA7313.h
*
* Created: 26.12.2023 19:08:57
*  Author: vacek
*/
#include "i2c_master.h"
#define TDA7313_address 0x44
#ifndef TDA7313_H_
#define TDA7313_H_
char BTvalues[] = {0,1,2,3,4,5,6,7,15,14,13,12,11,10,9,8};
char Bvalues[] = {0,1,2,4,6,8,16,24,31};
int Ovalues[] = { ~(1<<PIND4),~(1<<PIND3),0xf};
class TDA7313{
	private:
	int volume;
	int bass;
	int treble;
	int balance;
	int input;
	bool loudness;
	int output;
	
	
	void write(int data){
		i2cMasterStart(TDA7313_address, I2C_WRITE);
		i2cMasterSendByte(TDA7313_address, data);
		i2cMasterStop();
	}
	
	public:
	TDA7313(){
		volume = bass = treble = balance = input = loudness = 0;
		write(volume);
		write(0b01000000);
		write(0b10011111);
		write(0b10111111);
		write(0b11011111);
		write(0b11111111);
		write(0b10000000);
		write(0b10100000);
		write(bass| 0x60);
		write(treble| 0x70);
		DDRD |= 1<< PIND3 | 1 <<PIND4;
		PORTD |= 1<< PIND3;
		output = 0;
	}
	
	/************************************************************************/
	/* this function toggles output of amplifier                            */
	/************************************************************************/
	void toggleOutput(){
		output = (output+1)%3;
		PORTD &= Ovalues[output];
	}
	
	int getOutput(){
		return output;
	}
	
	/************************************************************************/
	/* this function toggles input of amplifier                             */
	/************************************************************************/
	void toggleInput(){
		input = (input+1)%4;
		write(input| 0x40);
	}
	int getInput(){
		return input;
	}
	/************************************************************************/
	/* this function toggles volume of amplifier                            */
	/************************************************************************/
	void toggleVolume(int direction){
		if(direction == 1){
			if(volume < 63){
				volume++;
				write(volume);
			}
		}else if(direction == -1){
			if(volume > 0){
				volume--;
				write(volume);
			}
		}
	}
	int getVolume(){
		return 63-volume;
	}
	/************************************************************************/
	/* this function toggles bass of amplifier                    ¨         */
	/************************************************************************/
	void toggleBass(int direction){
		if(direction == 1){
			if(bass < 15){
				bass++;
				write(BTvalues[bass]| 0x60);
			}
		}else if (direction == -1){
			if(bass > 0){
				bass--;
				write(BTvalues[bass]| 0x60);
			}
		}
	}
	int getBass(){
		return bass;
	}
	/************************************************************************/
	/* this function toggles treble of amplifier                            */
	/************************************************************************/
	void toggleTreble(int direction){
		if(direction == 1){
			if(treble < 15){
				treble++;
				write(BTvalues[treble]| 0x70);
			}
		}else if (direction == -1){
			if(treble > 0){
				treble--;
				write(BTvalues[treble]| 0x70);
			}
		}
	}
	int getTreble(){
		return treble;
	}
	/************************************************************************/
	/* this function toggles balance of amplifier                           */
	/************************************************************************/
	void toggleBalance(int direction){
		if(direction == 1){
			if(balance < 18){
				balance++;
			}
		}else if(direction == -1){
			if(balance > 0){
				balance--;
			}
		}
		if(balance>9){
			if(PORTD & (1<< PIND3))
				write(160|Bvalues[balance-9]);
			if(PORTD & (1<< PIND4))
				write(224|Bvalues[balance-9]);
		}
		else if(balance<9){
			if(PORTD & (1<< PIND3))
				write(128|Bvalues[9-balance]);
			if(PORTD & (1<< PIND4))
				write(192|Bvalues[9-balance]);
		}
		else{
			if(PORTD & (1<< PIND3)){
				write(128|Bvalues[0]);
				write(160|Bvalues[0]);
			}
			if(PORTD & (1<< PIND4)){
				write(192|Bvalues[0]);
				write(224|Bvalues[0]);
			}
			
		}
	}
	int getBalance(){
		if(balance>9){
			return (balance-9)*10;
		}
		else if(balance<9){
			return 9-balance;
		}
		else{
			return 0;
		}
	}
	
	void toggleAll(int * values){
		toggleVolume(values[3]);
		toggleBass(values[2]);
		toggleTreble(values[1]);
		toggleBalance(values[0]);
	}	
	void getAll(int * values){
		values [0]= getVolume();
		values [1]= getBass();
		values [2]= getTreble();
		values [3]= getBalance();
	}
};




#endif /* TDA7313_H_ */