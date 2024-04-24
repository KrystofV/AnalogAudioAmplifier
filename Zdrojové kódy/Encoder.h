/*
 * Encoder.h
 *
 * Created: 16.12.2023 12:03:21
 *  Author: vacek
 */


#ifndef ENCODER_H_
#define ENCODER_H_

class Encoder 
{
	private:
	volatile uint8_t*  port;
	int dt;
	int clk;
	int lastState;
	int distance;
	int direction;
	uint8_t state;
	
	public:
	Encoder(volatile uint8_t* p,int d,int c):port(p),dt(1<<d),clk(1<<c){
		state = 0;
		direction = 0;
	}
	void rotate(){
		int state = ~(*port & clk);
		if (lastState != state){
			if ((*port & dt) != ~(state>>1)) {
				direction = 1;
				} else {
					direction = -1;
			}
			lastState = state;
		}
	}
	void readEncoder() {
		bool CLKstate = (*port & clk) == 0;
		bool DTstate =  (*port & dt) == 0;
		switch (state) {
			case 0:                         // Idle state, encoder not turning
			if (!CLKstate){             // Turn clockwise and CLK goes low first
				state = 1;
				} else if (!DTstate) {      // Turn anticlockwise and DT goes low first
				state = 4;
			}
			break;
			// Clockwise rotation
			case 1:
			if (!DTstate) {             // Continue clockwise and DT will go low after CLK
				state = 2;
			}
			break;
			case 2:
			if (CLKstate) {             // Turn further and CLK will go high first
				state = 3;
			}
			break;
			case 3:
			if (CLKstate && DTstate) {  // Both CLK and DT now high as the encoder completes one step clockwise
				state = 0;
				direction=1;
			}
			break;
			// Anticlockwise rotation
			case 4:                         // As for clockwise but with CLK and DT reversed
			if (!CLKstate) {
				state = 5;
			}
			break;
			case 5:
			if (DTstate) {
				state = 6;
			}
			break;
			case 6:
			if (CLKstate && DTstate) {
				state = 0;
				direction=-1;
			}
			break;
		}
	}
	int getValue(){
		int i = direction;
		direction = 0;
		return i;
	}
};




#endif /* ENCODER_H_ */