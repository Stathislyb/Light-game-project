/////////////// Arduino 1 /////////
//// Master arduino



#include "StopWatch.h"
#include <Wire.h>
#include "stdlib.h"

// initialize a stopwatch
StopWatch MySW;

// player switches unlocking or locking their buttons
volatile int player_sw1=0;
volatile int player_sw2=0;
volatile int player_sw3=0;
volatile int player_sw4=0;

// variables to be updated by the interrupt
// required to see which button was pressed
volatile int x1 = 0;
volatile int x2 = 0;
volatile int x3 = 0;
volatile int x4 = 0;

//variables used for the turns
boolean rand_unique=false ;
volatile int player_turn=1;
int rands[]={-1,-1,-1,-1};
volatile int k =0;
volatile int game_turn=0;


// array to identify player pressed to send the data to the other arduino
int playerpressed[]={0,0,0,0};

// the players' buttons pins
int button1pin = 6;
int button2pin = 5;
int button3pin = 7;
int button4pin = 8;

// the rgb led pins
int pin_rgb_red = 9;
int pin_rgb_green = 10;
int pin_rgb_blue = 11;

// ints for the score
volatile int c1,c2,c3,c4=0;

// send signal for win / lose sounds
int winorlose=0;


void setup() {                
  pinMode(pin_rgb_red, OUTPUT);
  pinMode(pin_rgb_blue, OUTPUT);
  pinMode(pin_rgb_green, OUTPUT);
  
  pinMode(button1pin, INPUT);
  pinMode(button2pin, INPUT);
  pinMode(button3pin, INPUT);
  pinMode(button4pin, INPUT);

  //pin 2, players' buttons interrupt
  attachInterrupt(0, checkButton, RISING);

  //turn on communication between the 2 arduinos
  Wire.begin();
  
  //turn on serial communication
  Serial.begin(9600);  
}

void loop() {
  
  //buffer delay	
  delay(50);
  
  if(game_turn==0)
  {
  	// Game initialization,  Stage 1
  	/// buttons disabled (aside restart)
  	/// rgb closed
  	/// game starting music
  	
    player_sw1=0;
    player_sw2=0;
    player_sw3=0;
    player_sw4=0;
    c1=0;
    c2=0;
    c3=0;
    c4=0;
    
    digitalWrite(pin_rgb_red, HIGH);
    digitalWrite(pin_rgb_green,HIGH);
    digitalWrite(pin_rgb_blue, HIGH);

    Wire.beginTransmission(4);
	// use of (+) to declare a user
	// use of (-) to declare a game stage
	Wire.write("-1"); //game start
  	Wire.endTransmission();
    delay(10000);
    
  }else if(game_turn>0 && game_turn<10)
  {
  	// Main game,  Stage 2
  	/// buttons enabled
  	/// rgb randomly gets a value asigned to a player
  	/// short delay for the button press
  	/// turn results
  	/// results send to screen, one player each turn, all the players at the ending stage
  	/// preturn-music before the rgb lights, win/loss music after a button is pressed
  	
    if(k==4){
       k=0; 
       for (int i = 0; i < 4; i = i + 1) {
         rands[i]=-1;
       }
    }
    rand_unique=false;
    player_turn = random(4);
    while (rand_unique==false)
    {
      rand_unique=true;
      for(int u=0 ; u<4 ; u++)
      {
        if(rands[u]==player_turn)
        {
          player_turn = random(4);
          rand_unique=false;
        }
      }
    }
    

	delay(50);
  	Wire.beginTransmission(4);
	// use of (+) to declare a user
	// use of (-) to declare a game stage
	Wire.write("-2"); //turn begin
  	Wire.endTransmission();
  	delay(1200);
  	
    rands[k]=player_turn;
    k++;
    MySW.reset();
    MySW.start();
    player_sw1=1;
    player_sw2=1;
    player_sw3=1;
    player_sw4=1;
    
    if(player_turn==1){
      digitalWrite(pin_rgb_blue, HIGH);
      analogWrite(pin_rgb_red, 140);
      digitalWrite(pin_rgb_green, LOW);
    }else if(player_turn == 0){
      digitalWrite(pin_rgb_red, HIGH);
      digitalWrite(pin_rgb_green,HIGH);
      digitalWrite(pin_rgb_blue, LOW);
    }else if(player_turn == 2)
    {
      digitalWrite(pin_rgb_red, LOW);
      digitalWrite(pin_rgb_green,HIGH);
      digitalWrite(pin_rgb_blue, HIGH);
    }else if(player_turn == 3)
    {
      digitalWrite(pin_rgb_red, HIGH);
      digitalWrite(pin_rgb_green,LOW);
      digitalWrite(pin_rgb_blue, HIGH);
    }
    
    while(winorlose==0){
		delay(100);
    }
	MySW.stop();

	digitalWrite(pin_rgb_red, HIGH);
    digitalWrite(pin_rgb_green,HIGH);
    digitalWrite(pin_rgb_blue, HIGH);
	
	if(winorlose==1){ //winorlose==1 when the correct button was pushed
		winorlose=0;
		delay(50);
		Wire.beginTransmission(4);
		// use of (+) to declare a user
		// use of (-) to declare a game stage
		Wire.write("-3"); //correct
	  	Wire.endTransmission();
	  	delay(750);
	}
	if(winorlose==2){ //winorlose==2 when the wrong button was pushed
		winorlose=0;
		delay(50);
	  	Wire.beginTransmission(4);
		// use of (+) to declare a user
		// use of (-) to declare a game stage
		Wire.write("-4"); //false
	  	Wire.endTransmission();
	  	delay(750);
	}
	
	if(playerpressed[0]==1){
		delay(50);
		Wire.beginTransmission(4);
		// use of (+) to declare a user
		// use of (-) to declare a game stage
		Wire.write("+1"); //send for 1st player
	  	Wire.endTransmission();
	  	delay(50);
	  	Wire.beginTransmission(4);
	  	char score1[4];
	  	itoa(c1,score1,10);
		Wire.write(score1); //send 1st player score
	  	Wire.endTransmission();
	  	playerpressed[0]=0;
	  	delay(50);
	  	
	}else if(playerpressed[1]==1){
		delay(50);
		Wire.beginTransmission(4);
		// use of (+) to declare a user
		// use of (-) to declare a game stage
		Wire.write("+2"); //send for 2nd player
	  	Wire.endTransmission();
	  	delay(50);
	  	Wire.beginTransmission(4);
	  	char score2[4];
	  	itoa(c2,score2,10);
		Wire.write(score2); //send 2nd player score
	  	Wire.endTransmission();
	  	playerpressed[1]=0;
	  	delay(50);
	  	
	}else if(playerpressed[2]==1){
		delay(50);
		Wire.beginTransmission(4);
		// use of (+) to declare a user
		// use of (-) to declare a game stage
		Wire.write("+3"); //send for 3rd player
	  	Wire.endTransmission();
	  	delay(50);
	  	Wire.beginTransmission(4);
	  	char score3[4];
	  	itoa(c3,score3,10);
		Wire.write(score3); //send 3rd player score
	  	Wire.endTransmission();
	  	playerpressed[2]=0;
	  	delay(50);
	  	
	}else if(playerpressed[3]==1){
		delay(50);
		Wire.beginTransmission(4);
		// use of (+) to declare a user
		// use of (-) to declare a game stage
		Wire.write("+4"); //send for 4th player
	  	Wire.endTransmission();
	  	delay(50);
	  	Wire.beginTransmission(4);
	  	char score4[4];
	  	itoa(c4,score4,10);
		Wire.write(score4); //send 4th player score
	  	Wire.endTransmission();
	  	playerpressed[3]=0;
	  	delay(50);
	  	
	}
  }else if(game_turn == 11)
  {
  	// End game,  Stage 3
  	/// buttons disabled (aside restart)
  	/// display scores for all players
  	/// play winner music
  	
    player_sw1=0;
    player_sw2=0;
    player_sw3=0;
    player_sw4=0;
    
    delay(50);
    Wire.beginTransmission(4);
	// use of (+) to declare a user
	// use of (-) to declare a game stage
	Wire.write("+5"); //prepare to send for all players
  	Wire.endTransmission();
  	delay(150);
  	
    Wire.beginTransmission(4);
	// use of (+) to declare a user
	// use of (-) to declare a game stage
	Wire.write("+1"); //send for 1st player
  	Wire.endTransmission();
  	delay(50);
  	Wire.beginTransmission(4);
  	char finalscore1[4];
  	itoa(c1,finalscore1,10);
	Wire.write(finalscore1); //send 1st player score
  	Wire.endTransmission();
    delay(50);
        
    Wire.beginTransmission(4);
	// use of (+) to declare a user
	// use of (-) to declare a game stage
	Wire.write("+2"); //send for 2nd player
  	Wire.endTransmission();
  	delay(50);
  	Wire.beginTransmission(4);
  	char finalscore2[4];
  	itoa(c2,finalscore2,10);
	Wire.write(finalscore2); //send 2nd player score
  	Wire.endTransmission();
  	delay(50);
  	
  	Wire.beginTransmission(4);
	// use of (+) to declare a user
	// use of (-) to declare a game stage
	Wire.write("+3"); //send for 3nd player
  	Wire.endTransmission();
  	delay(50);
  	Wire.beginTransmission(4);
  	char finalscore3[4];
  	itoa(c3,finalscore3,10);
	Wire.write(finalscore3); //send 3nd player score
  	Wire.endTransmission();
  	delay(50);
  	
  	Wire.beginTransmission(4);
	// use of (+) to declare a user
	// use of (-) to declare a game stage
	Wire.write("+4"); //send for 4th player
  	Wire.endTransmission();
  	delay(50);
  	Wire.beginTransmission(4);
  	char finalscore4[4];
  	itoa(c4,finalscore4,10);
	Wire.write(finalscore4); //send 4th player score
  	Wire.endTransmission();
  	delay(50);
  
  	Wire.beginTransmission(4);
	// use of (+) to declare a user
	// use of (-) to declare a game stage
	Wire.write("-5"); //game end
  	Wire.endTransmission();
  	delay(50);
  	
    game_turn = 12;
  }
  
  if(game_turn<=10){
  	//next turn while there are turns
    game_turn += 1;
    
  }
}




void checkButton() {
	//Player button at interrupt 0 (pin 2)
	//if the buttons are enabled, read them to see which is pressed
	//calculate the score based on the turn  using the stopwatch 
	
	if(player_sw1==1){  
		player_sw1=0;
		x1 = digitalRead(button1pin);
		if(x1 == HIGH){
			if(player_turn==0){
				c1=c1+MySW.elapsed();
				winorlose=1;
			}else{
				c1=c1+2*MySW.elapsed();
				winorlose=2;
			}
			playerpressed[0]=1;
		}
		x1 = LOW;
	}
	
	if(player_sw2==1){ 
		player_sw2=0;
		x2 = digitalRead(button2pin);
		if(x2 == HIGH){
			if(player_turn==1){
				c2=c2+MySW.elapsed();
				winorlose=1;
			}else{
				c2=c2+2*MySW.elapsed();
				winorlose=2;
			}
			playerpressed[1]=1;
		}
		x2 = LOW;
	}
	
	if(player_sw3==1){ 
		player_sw3=0;
		x3 = digitalRead(button3pin);
		if(x3 == HIGH){
			if(player_turn==2){
				c3=c3+MySW.elapsed();
				winorlose=1;
			}else{
				c3=c3+2*MySW.elapsed();
				winorlose=2;
			}
			playerpressed[2]=1;
		}
		x3 = LOW;
	}
	if(player_sw4==1){ 
		player_sw4=0;
		x4 = digitalRead(button4pin);
		if(x4 == HIGH){
			if(player_turn==3){
				c4=c4+MySW.elapsed();
				winorlose=1;
			}else{
				c4=c4+2*MySW.elapsed();
				winorlose=2;
			}
			playerpressed[3]=1;
		}
		x4 = LOW;
	}
	
	digitalWrite(pin_rgb_red, HIGH);
    digitalWrite(pin_rgb_green,HIGH);
    digitalWrite(pin_rgb_blue, HIGH);
    
    return;
	
}
