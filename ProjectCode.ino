#include <Arduino.h>
#include <IRremote.h>
#define LEDARRAY_D 9
#define LEDARRAY_C 8
#define LEDARRAY_B 7
#define LEDARRAY_A 6
#define LEDARRAY_G 5
#define LEDARRAY_DI 4
#define LEDARRAY_CLK 3
#define LEDARRAY_LAT 2

unsigned int i = 0;
int voltage[12];
IRrecv irrecv(13);
decode_results results;
unsigned long key_value = 0;
unsigned char Display_Buffer[2];
int currentPosition[2] = {1, 7};
byte win = false;
int newPosition[2];
byte FirstPuzzle[1][32] =
{
  //   A1         A3         A2         A4         A5         A7         A6         A8         C1         C3         C2         C4         C5         C7         C6         C8
  B11101111, B10111110, B10101000, B00000010, B11111110, B10111011, B10100000, B10001010, B10111110, B10111011, B10100000, B00001110, B11101010, B10111011, B10101010, B00000000,

  //   B1         B3         B2         B4         B5         B7         B6         B8         D1         D3         D2         D4         D5        D7           D6        D8
  B10111110, B10111110, B10001000, B00101010, B11101010, B10101010, B10101010, B00001010, B11101010, B10101010, B10101010, B00101010, B11101010, B10111110, B00100010, B10000000, /*"C:\Users\Lin\Desktop\心.BMP",0*/

};

byte maze1[1][32] = {
  B11101111, B10111110, // A1, B1 = A1, A2  1
  B10101000, B10001000, // A2, B2 = A3, A4  2
  B10111110, B10111110, // A3, B3 = A5, A6  3
  B00000010, B00101010, // A4, B4 = A7, A8  4
  B11111110, B11101010, // A5, B5 = C1, C2  5
  B10100000, B10101010, // A6, B6 = C3, C4  6
  B10111011, B10101010, // A7, B7 = C5, C6  7 B10111011
  B10001010, B00001010, // A8, B8 = C7, C8  8 B10001010
  B10111110, B11101010, // C1, D1 = B1, B2  1
  B10100000, B10101010, // C2, D2 = B3, B4  2
  B10111011, B10101010, // C3, D3 = B5, B6  3
  B00001010, B00101010, // C4, D4 = B7, B8  4
  B11101010, B11101010, // C5, D5 = D1, D2  5
  B10101010, B00100010, // C6, D6 = D3, D4  6 B10111011
  B10111011, B10111110, // C7, D7 = D5, D6  7
  B00000000, B10000000  // C8, D8 = D7, D8  8
};

byte endScreen[1][32] = {
  B11111111, B11111111,
  B11111111, B11111111,
  B11111011, B11111111,
  B11110111, B10011111,
  B11110111, B10011111,
  B11110111, B11111111,
  B11110111, B11111111,
  B11110111, B11111111,
  B11110111, B11111111,
  B11110111, B11111111,
  B11110111, B11111111,
  B11110111, B10011111,
  B11110111, B10011111,
  B11111011, B11111111,
  B11111111, B11111111,
  B11111111, B11111111
};

byte puzzle[1][32];
byte puzzlePlayer[1][32];
byte prePuzzle[1][32];

// 1                                   00010000, 01000001, // A1, B1 = A1, A2
// 2                                   01010111, 01110111, // A2, B2 = A3, A4
// 3                                   01000001, 01000001, // A3, B3 = A5, A6
// 4                                   11111101, 11010101, // A4, B4 = A7, A8
// 5                                   00000001, 00010101, // A5, B5 = C1, C2
// 6                                   01011111, 01010101, // A6, B6 = C3, C4
// 7                                   01000100, 01010101, // A7, B7 = C5, C6
// 8                                   01110101, 11110101, // A8, B8 = C7, C8
// 9                                   01000001, 00010101, // C1, D1 = B1, B2
// 10                                  01011111, 01010101, // C2, D2 = B3, B4
// 11                                  01000100, 01010101, // C3, D3 = B5, B6
// 12                                  11110001, 11010101, // C4, D4 = B7, B8
// 13                                  00010101, 00010101, // C5, D5 = D1, D2
// 14                                  01010101, 11011101, // C6, D6 = D3, D4
// 15                                  01000100, 01000001, // C7, D7 = D5, D6
// 16                                  11111111, 01111111  // C8, D8 = D7, D8


byte test[1][32] =                 {B00000000, B00000000,
                                    B00011000, B00111000,
                                    B00111000, B01101100,
                                    B00011000, B00000100,
                                    B00011000, B00001100,
                                    B00011000, B00111000,
                                    B01111110, B01111110,
                                    B00000000, B00000000,
                                    B00000000, B00000000,
                                    B01111110, B00011100,
                                    B00000100, B00100100,
                                    B00001000, B01100110,
                                    B00000100, B01111100,
                                    B01000010, B00000100,
                                    B00111100, B00000100,
                                    B00000000, B00000000
                                   };
void setup()
{
  Serial.begin(9600);

  for (int i = 0; i < 32; i++) {
    for (int j = 7; j >= 0; j--) {
      Serial.print(bitRead(maze1[0][i], j));
    }
    if (i % 2 > 0) {
      Serial.println();
    }
  }
  irrecv.enableIRIn();
  for (int i = 0; i < 32; i++) {
    puzzle[0][i] = maze1[0][i];
    puzzlePlayer[0][i] = maze1[0][i];
    prePuzzle[0][i] = maze1[0][i];
  }
  bitWrite(puzzlePlayer[0][1], 7, 0);
  makeArrayPrintable(puzzle, 1);
  makeArrayPrintable(puzzlePlayer, 0);
  makeArrayPrintable(endScreen, 2);
  pinMode(LEDARRAY_D, OUTPUT);
  pinMode(LEDARRAY_C, OUTPUT);
  pinMode(LEDARRAY_B, OUTPUT);
  pinMode(LEDARRAY_A, OUTPUT);
  pinMode(LEDARRAY_G, OUTPUT);
  pinMode(LEDARRAY_DI, OUTPUT);
  pinMode(LEDARRAY_CLK, OUTPUT);
  pinMode(LEDARRAY_LAT, OUTPUT);
}

void loop()
{
  int puzzleCounter = 10, playerCounter = 10;
  winMethod();
  TranslateIR();
  if (!win) {
    while (puzzleCounter) {
      Display(puzzle);
      puzzleCounter--;
    }
    delay(1);
    while (playerCounter) {
      Display(puzzlePlayer);
      playerCounter--;
    }
    delay(1);
  } else {
    Display(endScreen);
  }
  delay(1);
}

//************************************************************
//num means the number of word  dat[][32] is the name of font
//*************************************************************
void Display(byte dat[][32])
{
  byte i;

  for ( i = 0 ; i < 16 ; i++ )
  {
    digitalWrite(LEDARRAY_G, HIGH);   //When updating the data,please close the display. After updating the data, open the 138 display line. Avoiding ghosting.

    Display_Buffer[0] = dat[0][i];
    Display_Buffer[1] = dat[0][i + 16];

    Send(Display_Buffer[1]);
    Send(Display_Buffer[0]);

    digitalWrite(LEDARRAY_LAT, HIGH);         //storage data
    delayMicroseconds(1);

    digitalWrite(LEDARRAY_LAT, LOW);
    delayMicroseconds(1);

    Scan_Line(i);             //Choose the line i

    digitalWrite(LEDARRAY_G, LOW);

    delayMicroseconds(100);;      // Delay for a period of time to let the LED light up.
  }
}

//****************************************************
//Scanning line
//****************************************************
void Scan_Line(byte m)
{
  switch (m)
  {
    case 0:
      digitalWrite(LEDARRAY_D, LOW); digitalWrite(LEDARRAY_C, LOW); digitalWrite(LEDARRAY_B, LOW); digitalWrite(LEDARRAY_A, LOW);
      break;
    case 1:
      digitalWrite(LEDARRAY_D, LOW); digitalWrite(LEDARRAY_C, LOW); digitalWrite(LEDARRAY_B, LOW); digitalWrite(LEDARRAY_A, HIGH);
      break;
    case 2:
      digitalWrite(LEDARRAY_D, LOW); digitalWrite(LEDARRAY_C, LOW); digitalWrite(LEDARRAY_B, HIGH); digitalWrite(LEDARRAY_A, LOW);
      break;
    case 3:
      digitalWrite(LEDARRAY_D, LOW); digitalWrite(LEDARRAY_C, LOW); digitalWrite(LEDARRAY_B, HIGH); digitalWrite(LEDARRAY_A, HIGH);
      break;
    case 4:
      digitalWrite(LEDARRAY_D, LOW); digitalWrite(LEDARRAY_C, HIGH); digitalWrite(LEDARRAY_B, LOW); digitalWrite(LEDARRAY_A, LOW);
      break;
    case 5:
      digitalWrite(LEDARRAY_D, LOW); digitalWrite(LEDARRAY_C, HIGH); digitalWrite(LEDARRAY_B, LOW); digitalWrite(LEDARRAY_A, HIGH);
      break;
    case 6:
      digitalWrite(LEDARRAY_D, LOW); digitalWrite(LEDARRAY_C, HIGH); digitalWrite(LEDARRAY_B, HIGH); digitalWrite(LEDARRAY_A, LOW);
      break;
    case 7:
      digitalWrite(LEDARRAY_D, LOW); digitalWrite(LEDARRAY_C, HIGH); digitalWrite(LEDARRAY_B, HIGH); digitalWrite(LEDARRAY_A, HIGH);
      break;
    case 8:
      digitalWrite(LEDARRAY_D, HIGH); digitalWrite(LEDARRAY_C, LOW); digitalWrite(LEDARRAY_B, LOW); digitalWrite(LEDARRAY_A, LOW);
      break;
    case 9:
      digitalWrite(LEDARRAY_D, HIGH); digitalWrite(LEDARRAY_C, LOW); digitalWrite(LEDARRAY_B, LOW); digitalWrite(LEDARRAY_A, HIGH);
      break;
    case 10:
      digitalWrite(LEDARRAY_D, HIGH); digitalWrite(LEDARRAY_C, LOW); digitalWrite(LEDARRAY_B, HIGH); digitalWrite(LEDARRAY_A, LOW);
      break;
    case 11:
      digitalWrite(LEDARRAY_D, HIGH); digitalWrite(LEDARRAY_C, LOW); digitalWrite(LEDARRAY_B, HIGH); digitalWrite(LEDARRAY_A, HIGH);
      break;
    case 12:
      digitalWrite(LEDARRAY_D, HIGH); digitalWrite(LEDARRAY_C, HIGH); digitalWrite(LEDARRAY_B, LOW); digitalWrite(LEDARRAY_A, LOW);
      break;
    case 13:
      digitalWrite(LEDARRAY_D, HIGH); digitalWrite(LEDARRAY_C, HIGH); digitalWrite(LEDARRAY_B, LOW); digitalWrite(LEDARRAY_A, HIGH);
      break;
    case 14:
      digitalWrite(LEDARRAY_D, HIGH); digitalWrite(LEDARRAY_C, HIGH); digitalWrite(LEDARRAY_B, HIGH); digitalWrite(LEDARRAY_A, LOW);
      break;
    case 15:
      digitalWrite(LEDARRAY_D, HIGH); digitalWrite(LEDARRAY_C, HIGH); digitalWrite(LEDARRAY_B, HIGH); digitalWrite(LEDARRAY_A, HIGH);
      break;
    default : break;
  }
}

//****************************************************
//Send the data
//****************************************************
void Send(byte dat)
{
  byte i;
  digitalWrite(LEDARRAY_CLK, LOW);
  delayMicroseconds(1);;
  digitalWrite(LEDARRAY_LAT, LOW);
  delayMicroseconds(1);;

  for ( i = 0 ; i < 8 ; i++ )
  {
    if ( dat & 0x01 )
    {
      digitalWrite(LEDARRAY_DI, HIGH);
    }
    else
    {
      digitalWrite(LEDARRAY_DI, LOW);
    }

    delayMicroseconds(1);
    digitalWrite(LEDARRAY_CLK, HIGH);       // Rising edge sends the data
    delayMicroseconds(1);
    digitalWrite(LEDARRAY_CLK, LOW);
    delayMicroseconds(1);
    dat >>= 1;

  }
}
void TranslateIR() {
  if (irrecv.decode(&results)) {

    if (results.value == 0XFFFFFFFF)
      results.value = key_value;

    switch (results.value) {
      case 0xFF629D://vol+ UP
        MoveMethod(4);
        Serial.println("UP");
        break;
      case 0xFFA857://vol- DOWN
        MoveMethod(3);
        Serial.println("DOWN");
        break ;
      case 0xFF22DD://|<< LEFT
        MoveMethod(2);
        Serial.println("LEFT");
        break;
      case 0xFFC23D://>>| RIGHT
        MoveMethod(1);
        Serial.println("RIGHT");
        break ;
      case 0xFF02FD://>|| OKAY
        resetMethod();
        Serial.println("OKAY");
        break ;


    }
    key_value = results.value;
    irrecv.resume();
    Serial.println(key_value);


  }
}

void winMethod() {
  if (puzzlePlayer[0][31] == B00000000) {
    win = true;
  }
}

byte MoveMethod(int cardinal) {
  int oldPosition[2] = {currentPosition[0], currentPosition[1]};
  newPosition[0] = currentPosition[0];
  newPosition[1] = currentPosition[1];
  switch (cardinal) {
    // UP
    case 4:
      newPosition[1] = newPosition[1] - 1;
      if (legalMove()) {
        flipLED(currentPosition, newPosition);
        currentPosition[0] = newPosition[0];
        currentPosition[1] = newPosition[1];
        return 1;
      } else {
        newPosition[1] = oldPosition[1];
        return 0;
      }
      break;
    case 3:
      // DOWN
      newPosition[1] = newPosition[1] + 1;
      if (legalMove()) {
        flipLED(currentPosition, newPosition);
        currentPosition[0] = newPosition[0];
        currentPosition[1] = newPosition[1];
        return 1;
      } else {
        newPosition[1] = oldPosition[1];
        return 0;
      }
      break;
    case 2:
      // LEFT
      newPosition[0] = newPosition[0] - 2;
      if (legalMove()) {
        flipLED(currentPosition, newPosition);
        currentPosition[0] = newPosition[0];
        return 1;
      } else {
        newPosition[0] = oldPosition[0];
        return 0;
      }
      break;
    case 1:
      //RIGHT
      newPosition[0] = newPosition[0] + 2;
      if (legalMove()) {
        flipLED(currentPosition, newPosition);
        currentPosition[0] = newPosition[0];
        return 1;
      } else {
        newPosition[0] = oldPosition[0];
        return 0;
      }
  }
}
byte legalMove() {
  if (newPosition[0] < 0 || newPosition[0] > 31) {
    return 0;
  }
  if (newPosition[1] < 0) {
    newPosition[0] = newPosition[0] + 1;
    newPosition[1] = 7;
  }
  if (newPosition[1] > 7) {
    newPosition[0] = newPosition[0] - 1;
    newPosition[1] = 0;
  }
  Serial.println(bitRead(prePuzzle[0][newPosition[0]], newPosition[1]));
  return bitRead(prePuzzle[0][newPosition[0]], newPosition[1]);
}

// A1    A3    A2    A4    A5    A7    A6    A8    C1    C3    C2    C4    C5    C7    C6    C8
// B1    B3    B2    B4    B5    B7    B6    B8    D1    D3    D2    D4    D5    D7    D6    D8
//                                                          current   Future
//                                    00000000, 00000000, // A1, B1 = A1, A2
//                                    00011000, 00111000, // A2, B2 = A3, A4
//                                    00111000, 01101100, // A3, B3 = A5, A6
//                                    00011000, 00000100, // A4, B4 = A7, A8
//                                    00011000, 00001100, // A5, B5 = C1, C2
//                                    00011000, 00111000, // A6, B6 = C3, C4
//                                    01111110, 01111110, // A7, B7 = C5, C6
//                                    00000000, 00000000, // A8, B8 = C7, C8
//                                    00000000, 00000000, // C1, D1 = B1, B2
//                                    01111110, 00011100, // C2, D2 = B3, B4
//                                    00000100, 00100100, // C3, D3 = B5, B6
//                                    00001000, 01100110, // C4, D4 = B7, B8
//                                    00000100, 01111100, // C5, D5 = D1, D2
//                                    01000010, 00000100, // C6, D6 = D3, D4
//                                    00111100, 00000100, // C7, D7 = D5, D6
//                                    00000000, 00000000  // C8, D8 = D7, D8
void makeArrayPrintable(byte givenArray[1][32], int switchCase) {
  byte tempValue;
  int tempInt = 0;
  byte tempArray[1][32];

  for (int j = 0; j < 32; j++) {
    if ((j + 2) % 2 == 0) {
      //        Serial.print(tempInt);
      //        Serial.print(": ");
      //        Serial.print(tempArray[1][tempInt]);
      //        Serial.print(" = ");
      //        Serial.println(givenArray[1][j]);
      tempArray[0][tempInt] = givenArray[0][j];
      tempInt++;
    }
  }
  for (int j = 0; j < 32; j++) {
    if ((j + 2) % 2 > 0) {
      //        Serial.print(tempInt);
      //        Serial.print(": ");
      //        Serial.print(tempArray[tempInt][j]);
      //        Serial.print(" = ");
      //        Serial.println(givenArray[1][j]);
      tempArray[0][tempInt] = givenArray[0][j];
      tempInt++;
    }
  }
  for (int i = 0; i < 4; i++) {
    tempInt = 8 * i;
    tempValue = tempArray[0][1 + tempInt];
    tempArray[0][1 + tempInt] = tempArray[0][2 + tempInt];
    tempArray[0][2 + tempInt] = tempValue;
    tempValue = tempArray[0][5 + tempInt];
    tempArray[0][5 + tempInt] = tempArray[0][6 + tempInt];
    tempArray[0][6 + tempInt] = tempValue;
  }

  for (int i = 0; i < 32; i++) {
    if (switchCase == 1) {
      puzzle[0][i] = tempArray[0][i];
    } else if (switchCase == 0){
      puzzlePlayer[0][i] = tempArray[0][i];
    } else {
      endScreen[0][i] = tempArray[0][i];
    }
  }
}
void flipLED(int ledPos1[2], int ledPos2[2]) {
  bitWrite(prePuzzle[0][ledPos1[0]], ledPos1[1], 1);
  bitWrite(prePuzzle[0][ledPos2[0]], ledPos2[1], 0);

  makeArrayPrintable(prePuzzle, 0);
}
void resetMethod() {
  int resetPos[2] = {currentPosition[0], currentPosition[1]};
  currentPosition[0] = 1;
  currentPosition[1] = 7;
  flipLED(resetPos, currentPosition);
  win = false;
}
