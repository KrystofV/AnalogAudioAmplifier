/*
 * Max7219.h
 *
 * Created: 16.12.2023 12:10:16
 *  Author: vacek
 */ 


#ifndef MAX7219_H_
#define MAX7219_H_

#define PIN_SCK                   PORTB7
#define PIN_MOSI                  PORTB5
#define PIN_SS                    PORTB4

#define ON                        1
#define OFF                       0

#define MAX7219_LOAD1             PORTB |= (1<<PIN_SS)
#define MAX7219_LOAD0             PORTB &= ~(1<<PIN_SS)

#define MAX7219_MODE_DECODE       0x09
#define MAX7219_MODE_INTENSITY    0x0A
#define MAX7219_MODE_SCAN_LIMIT   0x0B
#define MAX7219_MODE_POWER        0x0C
#define MAX7219_MODE_TEST         0x0F
#define MAX7219_MODE_NOOP         0x00


#define MAX7219_DIGIT0            0x01
#define MAX7219_DIGIT1            0x02
#define MAX7219_DIGIT2            0x03
#define MAX7219_DIGIT3            0x04
#define MAX7219_DIGIT4            0x05
#define MAX7219_DIGIT5            0x06
#define MAX7219_DIGIT6            0x07
#define MAX7219_DIGIT7            0x08


#define MAX7219_CHAR_BLANK        0xF
#define MAX7219_CHAR_NEGATIVE     0xA


class Max7219{
	private:
		int intensity;
		int digits;
		int values[4];
	
	void spiSendByte (char databyte)
	{
		// Copy data into the SPI data register
		SPDR = databyte;
		// Wait until transfer is complete
		while (!(SPSR & (1 << SPIF)));
	}
	
	void MAX7219_writeData(char data_register, char data)
	{
		MAX7219_LOAD0;
		// Send the register where the data will be stored
		spiSendByte(data_register);
		// Send the data to be stored
		spiSendByte(data);
		MAX7219_LOAD1;
	}
	
	public:
		Max7219(int i,int d):intensity(i),digits(d){
			// SCK MOSI CS/LOAD/SS
			DDRB |= (1 << PIN_SCK) | (1 << PIN_MOSI) | (1 << PIN_SS);

			// SPI Enable, Master mode
			SPCR |= (1 << SPE) | (1 << MSTR)| (1<<SPR1);

			// Decode mode to "Font Code-B"
			MAX7219_writeData(MAX7219_MODE_DECODE, 0xFF);

			// Scan limit runs from 0.
			MAX7219_writeData(MAX7219_MODE_SCAN_LIMIT, digits - 1);
			MAX7219_writeData(MAX7219_MODE_INTENSITY, intensity);
			MAX7219_writeData(MAX7219_MODE_POWER, ON);
			for(int i=0;i<4;i++)
				values[i]=0;
		}
		
		void MAX7219ClearDisplay()
		{
			char i = digits;
			// Loop until 0, but don't run for zero
			do {
				// Set each display in use to blank
				MAX7219_writeData(i, MAX7219_CHAR_BLANK);
			} while (--i);
		}
		
		void MAX7219DisplayNumber()
		{
			MAX7219ClearDisplay();

			for(int j=0;j<4;j++){
			// If number = 0, only show one zero then exit
				int number = values[j];
				/*if (number == 0) {
					MAX7219_writeData(j*2, 0);
					continue;
				}*/
			
			// Initialization to 0 required in this case,
			// does not work without it. Not sure why.
				char i = j*2;
			
			// Loop until number is 0.
				do {
					MAX7219_writeData(++i, number % 10);
				// Actually divide by 10 now.
					number /= 10;
				} while (number);
				}

			// Bear in mind that if you only have three digits, and
			// try to display something like "-256" all that will display
			// will be "256" because it needs an extra fourth digit to
			// display the sign.
		}
		
		void MAX7219UpdateNumber(int pos){
			if(values[pos]<10){
				MAX7219_writeData(2*pos, values[pos]);
				MAX7219_writeData(2*pos+1, MAX7219_CHAR_BLANK);
			}else{
				MAX7219_writeData(2*pos, values[pos]%10);
				MAX7219_writeData(2*pos+1, values[pos]/10);
			}
		}
		
		
		
		void update(int number,int pos){
			if (values[pos] != number){
				values[pos]=number;
				MAX7219DisplayNumber();
			}
		}
		
		void updateAll(int * v){
			for(int i=0;i<4;i++)
				if (values[i] != v[3-i]){
					values[i] = v[3-i];
					MAX7219UpdateNumber(i);
				}
		}
		
	
	
	};




#endif /* MAX7219_H_ */