/*
  Lights up one specific LED in an LED cube
  The input comes from the serial port in an (x, y, z) coordinate format.

  Denis Foo Kune  
  Jan 2013

  
 */
 
// Pin 13 has an LED connected on most Arduino boards.
// give it a name:
int led = 13;

//Pins for the cube
int l1xx = 12;
int l2xx = 13;
int l3xx = 2;
int layer2pin[3];

int lx11 = 3;
int lx12 = 4;
int lx13 = 5;
int lx21 = 6;
int lx22 = 7;
int lx23 = 8;
int lx31 = 9;
int lx32 = 10;
int lx33 = 11;

int cubesize = 3 * 3 * 3;
int x=0;
int y=0;
int z=0; 

String inputString = "";         // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete


// the setup routine runs once when you press reset:
void setup() {                
  // initialize the digital pin as an output.
  pinMode(led, OUTPUT);     
  pinMode(l1xx, OUTPUT);     
  pinMode(l2xx, OUTPUT);     
  pinMode(l3xx, OUTPUT);     
  pinMode(lx11, OUTPUT);     
  pinMode(lx12, OUTPUT);     
  pinMode(lx13, OUTPUT);     
  pinMode(lx21, OUTPUT);     
  pinMode(lx22, OUTPUT);     
  pinMode(lx23, OUTPUT);     
  pinMode(lx31, OUTPUT);     
  pinMode(lx32, OUTPUT);     
  pinMode(lx33, OUTPUT);     

  layer2pin[0]=l1xx;
  layer2pin[1]=l2xx;
  layer2pin[2]=l3xx;

  // initialize serial:
  Serial.begin(9600);
  // reserve 200 bytes for the inputString:
  inputString.reserve(10);
  
  x=0;
  y=0;
  x=0;

}

// the loop routine runs over and over again forever:
void loop() {
  int dots[3][3][3];
  // Resets the patterns
  for (int layer = 0; layer < 3; layer++){
    for (int i = 0; i < 3; i++){
      for (int j = 0; j < 3; j++){
        dots[layer][i][j] = 0;
      }
    }
  }
  
  //
  //
  //   (2,2,0) +----------+ (2,2,2)
  //          /|  (2,0,0)/|
  // (2,0,0) +----------+ |
  //         | |        | |
  //         | +--------|-+ (0,2,2)
  //         |/ (0,2,0) |/
  // (0,0,0) +----------+ (0,0,2)
  //
  //

  // Sets pattern  
  //dots[0][2][0] = 1;
  //dots[0][0][0] = 1;
  //dots[1][1][1] = 1;
  //dots[2][2][2] = 1;
  
  if (stringComplete) {
    x=(int)(inputString[0])-48;
    y=(int)(inputString[1])-48;
    z=(int)(inputString[2])-48; 

    //Serial.println(x);
    // clear the string:
    inputString = "";
    stringComplete = false;
  }
  dots[x][y][z] = 1;

  
  // Commands the cube to light up.
  lightcube(dots);
}



int lightcube(int dots[3][3][3]){
  // Start with everything off
  digitalWrite(l1xx, HIGH);   // turn the LED on (HIGH is the voltage level)
  digitalWrite(l2xx, HIGH);   // turn the LED on (HIGH is the voltage level)
  digitalWrite(l3xx, HIGH);   // turn the LED on (HIGH is the voltage level)
  
  int dot =  3;
  
  
  // This whole routine should take less than 1/25 seconds.
  for (int layer = 0; layer < 3 ;layer++){
    // Activates the layer
    digitalWrite(layer2pin[layer], LOW);   // turn the LED on (HIGH is the voltage level)
    

    // Activates the dots within the layer
    dot = 3;// The initial position on a layer
    for (int i = 0; i < 3; i++){
      for (int j = 0; j < 3; j++){    
        // Activates the dots
        //digitalWrite(dots[layer][i][j], HIGH);
        if(dots[layer][i][j] == 1){
          digitalWrite(dot, HIGH);// Actiates the dot        
        }
       // Moves on to the next 
       dot++;
      }
    }
 
    // 10 msec per layer, so that we can go through the whole cube in 30ms > 25fps 
    delay(5);
    
    // Deactiavtes all the dots
    dot = 3;// The initial position on a layer
    for (int i = 0; i < 3; i++){
      for (int j = 0; j < 3; j++){    
       digitalWrite(dot, LOW);// Deactiates the dot       
       // Moves on to the next 
       dot++;
      }
    }
   
   // Deactivates the layer
   digitalWrite(layer2pin[layer], HIGH);   // turn the LED on (HIGH is the voltage level)
  }  
  return 0;
}

/*
  SerialEvent occurs whenever a new data comes in the
 hardware serial RX.  This routine is run between each
 time loop() runs, so using delay inside loop can delay
 response.  Multiple bytes of data may be available.
 */
void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read(); 
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    //if (inChar == '\n') {
    if(inputString.length() >= 3){
      stringComplete = true;
    } 
  }
}

