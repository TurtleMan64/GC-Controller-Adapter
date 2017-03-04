#include "Joystick.h"

const int pinIn = 4;
const int pinOut = 2;

volatile uint8_t *REG;

int dat[600];

int currentIndex = 0;

boolean controllerConnected = false;

void setup() 
{
    Joystick.begin(false);
    pinMode(pinIn, INPUT);
    pinMode(pinOut, OUTPUT);
    pinMode(5, INPUT);
    pinMode(6, INPUT);
    pinMode(7, INPUT);
    REG = portInputRegister(digitalPinToPort(4));
    delay(1000);
    noInterrupts();
    sendConnectionRequest();
    interrupts();
    delay(2000);
}

void loop()
{
    noInterrupts();
    
    delayMicroseconds(1000);
    sendPollRequestAndRecordResponse();
    booleanizeData();
    
    interrupts();
    
    scanDataAndSetValues();
    Joystick.sendState();
    delayMicroseconds(3000);
}

void booleanizeData()
{
    int count = 599;
    while(count != 0)
    {
        dat[count] = dat[count] & B00010000;
        count--;
    }
    dat[0] = dat[0] & B00010000;
}

void scanDataAndSetValues()
{
    currentIndex = 0;
    controllerConnected = true;
    getNextBit(); //Padding bit, also used to see if a controller is connected
    if(controllerConnected)
    {
        getNextBit(); //Padding bit
        getNextBit(); //Padding bit
        Joystick.setButton(7, getNextBit()); //Start
        Joystick.setButton(3, getNextBit()); //Y
        Joystick.setButton(1, getNextBit()); //X
        Joystick.setButton(2, getNextBit()); //B
        Joystick.setButton(0, getNextBit()); //A
        getNextBit(); //Padding bit
        Joystick.setButton(4, getNextBit()); //L
        Joystick.setButton(5, getNextBit()); //R
        Joystick.setButton(6, getNextBit()); //Z
        setHat(); //Next 4 bits for hat
        
        //Next 8 bits for Main-Stick X value
        unsigned int xVal[8];
        xVal[0] = getNextBit() << 3;
        xVal[1] = getNextBit() << 2;
        xVal[2] = getNextBit() << 1;
        xVal[3] = getNextBit();
        xVal[4] = getNextBit() >> 1;
        xVal[5] = getNextBit() >> 2;
        xVal[6] = getNextBit() >> 3;
        xVal[7] = getNextBit() >> 4;
        int totalX = xVal[0] | xVal[1] | xVal[2] | xVal[3] | xVal[4] | xVal[5] | xVal[6] | xVal[7];
        //if((totalX-127)*1.2 > 128)
        {
            //Joystick.setXAxis(128);
        }
        //else if((totalX-127)*1.2 < -127)
        {
            //Joystick.setXAxis(-127);
        }
        //else
        {
            Joystick.setXAxis((totalX-127));
        }
    
        //Next 8 bits for Main-Stick Y value
        unsigned int yVal[8];
        yVal[0] = getNextBit() << 3;
        yVal[1] = getNextBit() << 2;
        yVal[2] = getNextBit() << 1;
        yVal[3] = getNextBit();
        yVal[4] = getNextBit() >> 1;
        yVal[5] = getNextBit() >> 2;
        yVal[6] = getNextBit() >> 3;
        yVal[7] = getNextBit() >> 4;
        int totalY = yVal[0] | yVal[1] | yVal[2] | yVal[3] | yVal[4] | yVal[5] | yVal[6] | yVal[7];
        Joystick.setYAxis(128-totalY);//128-totalY
        //if((128-totalY)*1.2 > 128)
        {
            //Joystick.setYAxis(128);
        }
        //else if((128-totalY)*1.2 < -127)
        {
            //Joystick.setYAxis(-127);
        }
        //else
        {
            Joystick.setYAxis((128-totalY));
        }
    
        //Next 8 bits for C-Stick X value
        unsigned int xValC[8];
        xValC[0] = getNextBit() << 3;
        xValC[1] = getNextBit() << 2;
        xValC[2] = getNextBit() << 1;
        xValC[3] = getNextBit();
        xValC[4] = getNextBit() >> 1;
        xValC[5] = getNextBit() >> 2;
        xValC[6] = getNextBit() >> 3;
        xValC[7] = getNextBit() >> 4;
        unsigned int totalXC = xValC[0] | xValC[1] | xValC[2] | xValC[3] | xValC[4] | xValC[5] | xValC[6] | xValC[7];
        Joystick.setXAxisRotation(totalXC);
    
        //Next 8 bits for C-Stick Y value
        unsigned int yValC[8];
        yValC[0] = getNextBit() << 3;
        yValC[1] = getNextBit() << 2;
        yValC[2] = getNextBit() << 1;
        yValC[3] = getNextBit();
        yValC[4] = getNextBit() >> 1;
        yValC[5] = getNextBit() >> 2;
        yValC[6] = getNextBit() >> 3;
        yValC[7] = getNextBit() >> 4;
        unsigned int totalYC = yValC[0] | yValC[1] | yValC[2] | yValC[3] | yValC[4] | yValC[5] | yValC[6] | yValC[7];
        Joystick.setYAxisRotation(255-totalYC);
    
        //Next 8 bits for Left Shoulder value
        unsigned int leftShoulder[8];
        leftShoulder[0] = getNextBit() << 3;
        leftShoulder[1] = getNextBit() << 2;
        leftShoulder[2] = getNextBit() << 1;
        leftShoulder[3] = getNextBit();
        leftShoulder[4] = getNextBit() >> 1;
        leftShoulder[5] = getNextBit() >> 2;
        leftShoulder[6] = getNextBit() >> 3;
        leftShoulder[7] = getNextBit() >> 4;
        unsigned int totalLeftShoulder = leftShoulder[0] | leftShoulder[1] | leftShoulder[2] | leftShoulder[3] | leftShoulder[4] | leftShoulder[5] | leftShoulder[6] | leftShoulder[7];
        Joystick.setThrottle(totalLeftShoulder);
        
        //Next 8 bits for Right Shoulder value
        unsigned int rightShoulder[8];
        rightShoulder[0] = getNextBit() << 3;
        rightShoulder[1] = getNextBit() << 2;
        rightShoulder[2] = getNextBit() << 1;
        rightShoulder[3] = getNextBit();
        rightShoulder[4] = getNextBit() >> 1;
        rightShoulder[5] = getNextBit() >> 2;
        rightShoulder[6] = getNextBit() >> 3;
        rightShoulder[7] = getNextBit() >> 4;
        unsigned int totalRightShoulder = rightShoulder[0] | rightShoulder[1] | rightShoulder[2] | rightShoulder[3] | rightShoulder[4] | rightShoulder[5] | rightShoulder[6] | rightShoulder[7];
        Joystick.setRudder(totalRightShoulder);
    }
    else
    {
        Joystick.setButton(0, 0);
        Joystick.setButton(1, 0);
        Joystick.setButton(2, 0);
        Joystick.setButton(3, 0);
        Joystick.setButton(4, 0);
        Joystick.setButton(5, 0);
        Joystick.setButton(6, 0);
        Joystick.setButton(7, 0);
        Joystick.setXAxis(0);
        Joystick.setYAxis(0);
        Joystick.setXAxisRotation(127);
        Joystick.setYAxisRotation(127);
        Joystick.setRudder(0);
        Joystick.setThrottle(0);
        Joystick.setHatSwitch(0, -1);
    }
}

//gets the next bit
unsigned int getNextBit()
{
    unsigned int prev = dat[currentIndex];
    unsigned int curr = prev;
    while(currentIndex < 600)
    {
        curr = dat[currentIndex];
        if(prev && !curr)
        {
            currentIndex+=4;
            return dat[currentIndex];
        }
        prev = curr;
        currentIndex++;
    }
    controllerConnected = false;
    return 0; //error, or no controller connected
}

void setHat()
{
    int up = getNextBit();
    int down = getNextBit();
    int right = getNextBit();
    int left = getNextBit();
    int dir = -1;
    if(up)
    {
        if(left)
        {
            dir = 315;
        }
        else if (right)
        {
            dir = 45;
        }
        else
        {
            dir = 0;
        }
    }
    else if(down)
    {
        if(left)
        {
            dir = 225;
        }
        else if (right)
        {
            dir = 135;
        }
        else
        {
            dir = 180;
        }
    }
    else if(right)
    {
        dir = 90;
    }
    else if(left)
    {
        dir = 270;
    }
    Joystick.setHatSwitch(0, dir);
}

void sendConnectionRequest()
{
    //bit 0
    //Set pin 4 low for 3 microseconds
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    //Set pin 4 high for 1 microsecond
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH


    //bit 1
    //Set pin 4 low for 3 microseconds
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    //Set pin 4 high for 1 microsecond
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH


    //bit 2
    //Set pin 4 low for 3 microseconds
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    //Set pin 4 high for 1 microsecond
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH


    //bit 3
    //Set pin 4 low for 3 microseconds
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    //Set pin 4 high for 1 microsecond
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH


    //bit 4
    //Set pin 4 low for 3 microseconds
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    //Set pin 4 high for 1 microsecond
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH


    //bit 5
    //Set pin 4 low for 3 microseconds
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    //Set pin 4 high for 1 microsecond
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH


    //bit 6
    //Set pin 4 low for 3 microseconds
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    //Set pin 4 high for 1 microsecond
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH


    //bit 7
    //Set pin 4 low for 3 microseconds
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    //Set pin 4 high for 1 microsecond
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH


    //bit 8
    //Set pin 4 low for 1 microseconds
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    //Set pin 4 high for 3 microsecond
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
}

void sendPollRequestAndRecordResponse()
{
    //Sending poll request to controller
    
    //bit 0
    //Set pin 4 low for 3 microseconds
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    //Set pin 4 high for 1 microsecond
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH



    //bit 1
    //Set pin 4 low for 1 microseconds
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    //Set pin 4 high for 3 microsecond
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH





    //bit 2
    //Set pin 4 low for 3 microseconds
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    //Set pin 4 high for 1 microsecond
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH





    //bit 3
    //Set pin 4 low for 3 microseconds
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    //Set pin 4 high for 1 microsecond
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH




    //bit 4
    //Set pin 4 low for 3 microseconds
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    //Set pin 4 high for 1 microsecond
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH





    //bit 5
    //Set pin 4 low for 3 microseconds
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    //Set pin 4 high for 1 microsecond
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH




    //bit 6
    //Set pin 4 low for 3 microseconds
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    //Set pin 4 high for 1 microsecond
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH




    //bit 7
    //Set pin 4 low for 3 microseconds
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    //Set pin 4 high for 1 microsecond
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH



    //bit 8
    //Set pin 4 low for 3 microseconds
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    //Set pin 4 high for 1 microsecond
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH


    //bit 9
    //Set pin 4 low for 3 microseconds
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    //Set pin 4 high for 1 microsecond
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH


    //bit 10
    //Set pin 4 low for 3 microseconds
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    //Set pin 4 high for 1 microsecond
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH


    //bit 11
    //Set pin 4 low for 3 microseconds
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    //Set pin 4 high for 1 microsecond
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH




    //bit 12
    //Set pin 4 low for 3 microseconds
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    //Set pin 4 high for 1 microsecond
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH


    //bit 13
    //Set pin 4 low for 3 microseconds
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    //Set pin 4 high for 1 microsecond
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH



    //bit 14
    //Set pin 4 low for 1 microseconds
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    //Set pin 4 high for 3 microsecond
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH


    //bit 15
    //Set pin 4 low for 1 microseconds
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    //Set pin 4 high for 3 microsecond
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH


    //bit 16
    //Set pin 4 low for 3 microseconds
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    //Set pin 4 high for 1 microsecond
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH


    //bit 17
    //Set pin 4 low for 3 microseconds
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    //Set pin 4 high for 1 microsecond
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH


    //bit 18
    //Set pin 4 low for 3 microseconds
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    //Set pin 4 high for 1 microsecond
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH


    //bit 19
    //Set pin 4 low for 3 microseconds
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    //Set pin 4 high for 1 microsecond
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH


    //bit 20
    //Set pin 4 low for 3 microseconds
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    //Set pin 4 high for 1 microsecond
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH


    //bit 21
    //Set pin 4 low for 3 microseconds
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    //Set pin 4 high for 1 microsecond
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH


    //bit 22
    //Set pin 4 low for 1 microseconds
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    //Set pin 4 high for 3 microsecond
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH


    //bit 23
    //Set pin 4 low for 3 microseconds
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    //Set pin 4 high for 1 microsecond
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH



    //stop bit
    //Set pin 4 low for 1 microseconds
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    PORTD |= _BV(PD1);//turn on, so LOW
    //Set pin 4 high for 3 microsecond
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    PORTD &= ~_BV(PD1);//turn off, so HIGH
    //PORTD &= ~_BV(PD1);//turn off, so HIGH
    //PORTD &= ~_BV(PD1);//turn off, so HIGH
    //PORTD &= ~_BV(PD1);//turn off, so HIGH
    //PORTD &= ~_BV(PD1);//turn off, so HIGH
    //PORTD &= ~_BV(PD1);//turn off, so HIGH
    //PORTD &= ~_BV(PD1);//turn off, so HIGH
    //PORTD &= ~_BV(PD1);//turn off, so HIGH
    //PORTD &= ~_BV(PD1);//turn off, so HIGH
    //PORTD &= ~_BV(PD1);//turn off, so HIGH
    //PORTD &= ~_BV(PD1);//turn off, so HIGH
    //PORTD &= ~_BV(PD1);//turn off, so HIGH
    //PORTD &= ~_BV(PD1);//turn off, so HIGH
    //PORTD &= ~_BV(PD1);//turn off, so HIGH
    //PORTD &= ~_BV(PD1);//turn off, so HIGH
    //PORTD &= ~_BV(PD1);//turn off, so HIGH
    //PORTD &= ~_BV(PD1);//turn off, so HIGH
    
    
    
    
    //Recording Response fom controller
    
    dat[0] = *REG;
    dat[1] = *REG;
    dat[2] = *REG;
    dat[3] = *REG;
    dat[4] = *REG;
    dat[5] = *REG;
    dat[6] = *REG;
    dat[7] = *REG;
    dat[8] = *REG;
    dat[9] = *REG;
    dat[10] = *REG;
    dat[11] = *REG;
    dat[12] = *REG;
    dat[13] = *REG;
    dat[14] = *REG;
    dat[15] = *REG;
    dat[16] = *REG;
    dat[17] = *REG;
    dat[18] = *REG;
    dat[19] = *REG;
    dat[20] = *REG;
    dat[21] = *REG;
    dat[22] = *REG;
    dat[23] = *REG;
    dat[24] = *REG;
    dat[25] = *REG;
    dat[26] = *REG;
    dat[27] = *REG;
    dat[28] = *REG;
    dat[29] = *REG;
    dat[30] = *REG;
    dat[31] = *REG;
    dat[32] = *REG;
    dat[33] = *REG;
    dat[34] = *REG;
    dat[35] = *REG;
    dat[36] = *REG;
    dat[37] = *REG;
    dat[38] = *REG;
    dat[39] = *REG;
    dat[40] = *REG;
    dat[41] = *REG;
    dat[42] = *REG;
    dat[43] = *REG;
    dat[44] = *REG;
    dat[45] = *REG;
    dat[46] = *REG;
    dat[47] = *REG;
    dat[48] = *REG;
    dat[49] = *REG;
    dat[50] = *REG;
    dat[51] = *REG;
    dat[52] = *REG;
    dat[53] = *REG;
    dat[54] = *REG;
    dat[55] = *REG;
    dat[56] = *REG;
    dat[57] = *REG;
    dat[58] = *REG;
    dat[59] = *REG;
    dat[60] = *REG;
    dat[61] = *REG;
    dat[62] = *REG;
    dat[63] = *REG;
    dat[64] = *REG;
    dat[65] = *REG;
    dat[66] = *REG;
    dat[67] = *REG;
    dat[68] = *REG;
    dat[69] = *REG;
    dat[70] = *REG;
    dat[71] = *REG;
    dat[72] = *REG;
    dat[73] = *REG;
    dat[74] = *REG;
    dat[75] = *REG;
    dat[76] = *REG;
    dat[77] = *REG;
    dat[78] = *REG;
    dat[79] = *REG;
    dat[80] = *REG;
    dat[81] = *REG;
    dat[82] = *REG;
    dat[83] = *REG;
    dat[84] = *REG;
    dat[85] = *REG;
    dat[86] = *REG;
    dat[87] = *REG;
    dat[88] = *REG;
    dat[89] = *REG;
    dat[90] = *REG;
    dat[91] = *REG;
    dat[92] = *REG;
    dat[93] = *REG;
    dat[94] = *REG;
    dat[95] = *REG;
    dat[96] = *REG;
    dat[97] = *REG;
    dat[98] = *REG;
    dat[99] = *REG;
    dat[100] = *REG;
    dat[101] = *REG;
    dat[102] = *REG;
    dat[103] = *REG;
    dat[104] = *REG;
    dat[105] = *REG;
    dat[106] = *REG;
    dat[107] = *REG;
    dat[108] = *REG;
    dat[109] = *REG;
    dat[110] = *REG;
    dat[111] = *REG;
    dat[112] = *REG;
    dat[113] = *REG;
    dat[114] = *REG;
    dat[115] = *REG;
    dat[116] = *REG;
    dat[117] = *REG;
    dat[118] = *REG;
    dat[119] = *REG;
    dat[120] = *REG;
    dat[121] = *REG;
    dat[122] = *REG;
    dat[123] = *REG;
    dat[124] = *REG;
    dat[125] = *REG;
    dat[126] = *REG;
    dat[127] = *REG;
    dat[128] = *REG;
    dat[129] = *REG;
    dat[130] = *REG;
    dat[131] = *REG;
    dat[132] = *REG;
    dat[133] = *REG;
    dat[134] = *REG;
    dat[135] = *REG;
    dat[136] = *REG;
    dat[137] = *REG;
    dat[138] = *REG;
    dat[139] = *REG;
    dat[140] = *REG;
    dat[141] = *REG;
    dat[142] = *REG;
    dat[143] = *REG;
    dat[144] = *REG;
    dat[145] = *REG;
    dat[146] = *REG;
    dat[147] = *REG;
    dat[148] = *REG;
    dat[149] = *REG;
    dat[150] = *REG;
    dat[151] = *REG;
    dat[152] = *REG;
    dat[153] = *REG;
    dat[154] = *REG;
    dat[155] = *REG;
    dat[156] = *REG;
    dat[157] = *REG;
    dat[158] = *REG;
    dat[159] = *REG;
    dat[160] = *REG;
    dat[161] = *REG;
    dat[162] = *REG;
    dat[163] = *REG;
    dat[164] = *REG;
    dat[165] = *REG;
    dat[166] = *REG;
    dat[167] = *REG;
    dat[168] = *REG;
    dat[169] = *REG;
    dat[170] = *REG;
    dat[171] = *REG;
    dat[172] = *REG;
    dat[173] = *REG;
    dat[174] = *REG;
    dat[175] = *REG;
    dat[176] = *REG;
    dat[177] = *REG;
    dat[178] = *REG;
    dat[179] = *REG;
    dat[180] = *REG;
    dat[181] = *REG;
    dat[182] = *REG;
    dat[183] = *REG;
    dat[184] = *REG;
    dat[185] = *REG;
    dat[186] = *REG;
    dat[187] = *REG;
    dat[188] = *REG;
    dat[189] = *REG;
    dat[190] = *REG;
    dat[191] = *REG;
    dat[192] = *REG;
    dat[193] = *REG;
    dat[194] = *REG;
    dat[195] = *REG;
    dat[196] = *REG;
    dat[197] = *REG;
    dat[198] = *REG;
    dat[199] = *REG;
    dat[200] = *REG;
    dat[201] = *REG;
    dat[202] = *REG;
    dat[203] = *REG;
    dat[204] = *REG;
    dat[205] = *REG;
    dat[206] = *REG;
    dat[207] = *REG;
    dat[208] = *REG;
    dat[209] = *REG;
    dat[210] = *REG;
    dat[211] = *REG;
    dat[212] = *REG;
    dat[213] = *REG;
    dat[214] = *REG;
    dat[215] = *REG;
    dat[216] = *REG;
    dat[217] = *REG;
    dat[218] = *REG;
    dat[219] = *REG;
    dat[220] = *REG;
    dat[221] = *REG;
    dat[222] = *REG;
    dat[223] = *REG;
    dat[224] = *REG;
    dat[225] = *REG;
    dat[226] = *REG;
    dat[227] = *REG;
    dat[228] = *REG;
    dat[229] = *REG;
    dat[230] = *REG;
    dat[231] = *REG;
    dat[232] = *REG;
    dat[233] = *REG;
    dat[234] = *REG;
    dat[235] = *REG;
    dat[236] = *REG;
    dat[237] = *REG;
    dat[238] = *REG;
    dat[239] = *REG;
    dat[240] = *REG;
    dat[241] = *REG;
    dat[242] = *REG;
    dat[243] = *REG;
    dat[244] = *REG;
    dat[245] = *REG;
    dat[246] = *REG;
    dat[247] = *REG;
    dat[248] = *REG;
    dat[249] = *REG;
    dat[250] = *REG;
    dat[251] = *REG;
    dat[252] = *REG;
    dat[253] = *REG;
    dat[254] = *REG;
    dat[255] = *REG;
    dat[256] = *REG;
    dat[257] = *REG;
    dat[258] = *REG;
    dat[259] = *REG;
    dat[260] = *REG;
    dat[261] = *REG;
    dat[262] = *REG;
    dat[263] = *REG;
    dat[264] = *REG;
    dat[265] = *REG;
    dat[266] = *REG;
    dat[267] = *REG;
    dat[268] = *REG;
    dat[269] = *REG;
    dat[270] = *REG;
    dat[271] = *REG;
    dat[272] = *REG;
    dat[273] = *REG;
    dat[274] = *REG;
    dat[275] = *REG;
    dat[276] = *REG;
    dat[277] = *REG;
    dat[278] = *REG;
    dat[279] = *REG;
    dat[280] = *REG;
    dat[281] = *REG;
    dat[282] = *REG;
    dat[283] = *REG;
    dat[284] = *REG;
    dat[285] = *REG;
    dat[286] = *REG;
    dat[287] = *REG;
    dat[288] = *REG;
    dat[289] = *REG;
    dat[290] = *REG;
    dat[291] = *REG;
    dat[292] = *REG;
    dat[293] = *REG;
    dat[294] = *REG;
    dat[295] = *REG;
    dat[296] = *REG;
    dat[297] = *REG;
    dat[298] = *REG;
    dat[299] = *REG;
    dat[300] = *REG;
    dat[301] = *REG;
    dat[302] = *REG;
    dat[303] = *REG;
    dat[304] = *REG;
    dat[305] = *REG;
    dat[306] = *REG;
    dat[307] = *REG;
    dat[308] = *REG;
    dat[309] = *REG;
    dat[310] = *REG;
    dat[311] = *REG;
    dat[312] = *REG;
    dat[313] = *REG;
    dat[314] = *REG;
    dat[315] = *REG;
    dat[316] = *REG;
    dat[317] = *REG;
    dat[318] = *REG;
    dat[319] = *REG;
    dat[320] = *REG;
    dat[321] = *REG;
    dat[322] = *REG;
    dat[323] = *REG;
    dat[324] = *REG;
    dat[325] = *REG;
    dat[326] = *REG;
    dat[327] = *REG;
    dat[328] = *REG;
    dat[329] = *REG;
    dat[330] = *REG;
    dat[331] = *REG;
    dat[332] = *REG;
    dat[333] = *REG;
    dat[334] = *REG;
    dat[335] = *REG;
    dat[336] = *REG;
    dat[337] = *REG;
    dat[338] = *REG;
    dat[339] = *REG;
    dat[340] = *REG;
    dat[341] = *REG;
    dat[342] = *REG;
    dat[343] = *REG;
    dat[344] = *REG;
    dat[345] = *REG;
    dat[346] = *REG;
    dat[347] = *REG;
    dat[348] = *REG;
    dat[349] = *REG;
    dat[350] = *REG;
    dat[351] = *REG;
    dat[352] = *REG;
    dat[353] = *REG;
    dat[354] = *REG;
    dat[355] = *REG;
    dat[356] = *REG;
    dat[357] = *REG;
    dat[358] = *REG;
    dat[359] = *REG;
    dat[360] = *REG;
    dat[361] = *REG;
    dat[362] = *REG;
    dat[363] = *REG;
    dat[364] = *REG;
    dat[365] = *REG;
    dat[366] = *REG;
    dat[367] = *REG;
    dat[368] = *REG;
    dat[369] = *REG;
    dat[370] = *REG;
    dat[371] = *REG;
    dat[372] = *REG;
    dat[373] = *REG;
    dat[374] = *REG;
    dat[375] = *REG;
    dat[376] = *REG;
    dat[377] = *REG;
    dat[378] = *REG;
    dat[379] = *REG;
    dat[380] = *REG;
    dat[381] = *REG;
    dat[382] = *REG;
    dat[383] = *REG;
    dat[384] = *REG;
    dat[385] = *REG;
    dat[386] = *REG;
    dat[387] = *REG;
    dat[388] = *REG;
    dat[389] = *REG;
    dat[390] = *REG;
    dat[391] = *REG;
    dat[392] = *REG;
    dat[393] = *REG;
    dat[394] = *REG;
    dat[395] = *REG;
    dat[396] = *REG;
    dat[397] = *REG;
    dat[398] = *REG;
    dat[399] = *REG;
    dat[400] = *REG;
    dat[401] = *REG;
    dat[402] = *REG;
    dat[403] = *REG;
    dat[404] = *REG;
    dat[405] = *REG;
    dat[406] = *REG;
    dat[407] = *REG;
    dat[408] = *REG;
    dat[409] = *REG;
    dat[410] = *REG;
    dat[411] = *REG;
    dat[412] = *REG;
    dat[413] = *REG;
    dat[414] = *REG;
    dat[415] = *REG;
    dat[416] = *REG;
    dat[417] = *REG;
    dat[418] = *REG;
    dat[419] = *REG;
    dat[420] = *REG;
    dat[421] = *REG;
    dat[422] = *REG;
    dat[423] = *REG;
    dat[424] = *REG;
    dat[425] = *REG;
    dat[426] = *REG;
    dat[427] = *REG;
    dat[428] = *REG;
    dat[429] = *REG;
    dat[430] = *REG;
    dat[431] = *REG;
    dat[432] = *REG;
    dat[433] = *REG;
    dat[434] = *REG;
    dat[435] = *REG;
    dat[436] = *REG;
    dat[437] = *REG;
    dat[438] = *REG;
    dat[439] = *REG;
    dat[440] = *REG;
    dat[441] = *REG;
    dat[442] = *REG;
    dat[443] = *REG;
    dat[444] = *REG;
    dat[445] = *REG;
    dat[446] = *REG;
    dat[447] = *REG;
    dat[448] = *REG;
    dat[449] = *REG;
    dat[450] = *REG;
    dat[451] = *REG;
    dat[452] = *REG;
    dat[453] = *REG;
    dat[454] = *REG;
    dat[455] = *REG;
    dat[456] = *REG;
    dat[457] = *REG;
    dat[458] = *REG;
    dat[459] = *REG;
    dat[460] = *REG;
    dat[461] = *REG;
    dat[462] = *REG;
    dat[463] = *REG;
    dat[464] = *REG;
    dat[465] = *REG;
    dat[466] = *REG;
    dat[467] = *REG;
    dat[468] = *REG;
    dat[469] = *REG;
    dat[470] = *REG;
    dat[471] = *REG;
    dat[472] = *REG;
    dat[473] = *REG;
    dat[474] = *REG;
    dat[475] = *REG;
    dat[476] = *REG;
    dat[477] = *REG;
    dat[478] = *REG;
    dat[479] = *REG;
    dat[480] = *REG;
    dat[481] = *REG;
    dat[482] = *REG;
    dat[483] = *REG;
    dat[484] = *REG;
    dat[485] = *REG;
    dat[486] = *REG;
    dat[487] = *REG;
    dat[488] = *REG;
    dat[489] = *REG;
    dat[490] = *REG;
    dat[491] = *REG;
    dat[492] = *REG;
    dat[493] = *REG;
    dat[494] = *REG;
    dat[495] = *REG;
    dat[496] = *REG;
    dat[497] = *REG;
    dat[498] = *REG;
    dat[499] = *REG;
    dat[500] = *REG;
    dat[501] = *REG;
    dat[502] = *REG;
    dat[503] = *REG;
    dat[504] = *REG;
    dat[505] = *REG;
    dat[506] = *REG;
    dat[507] = *REG;
    dat[508] = *REG;
    dat[509] = *REG;
    dat[510] = *REG;
    dat[511] = *REG;
    dat[512] = *REG;
    dat[513] = *REG;
    dat[514] = *REG;
    dat[515] = *REG;
    dat[516] = *REG;
    dat[517] = *REG;
    dat[518] = *REG;
    dat[519] = *REG;
    dat[520] = *REG;
    dat[521] = *REG;
    dat[522] = *REG;
    dat[523] = *REG;
    dat[524] = *REG;
    dat[525] = *REG;
    dat[526] = *REG;
    dat[527] = *REG;
    dat[528] = *REG;
    dat[529] = *REG;
    dat[530] = *REG;
    dat[531] = *REG;
    dat[532] = *REG;
    dat[533] = *REG;
    dat[534] = *REG;
    dat[535] = *REG;
    dat[536] = *REG;
    dat[537] = *REG;
    dat[538] = *REG;
    dat[539] = *REG;
    dat[540] = *REG;
    dat[541] = *REG;
    dat[542] = *REG;
    dat[543] = *REG;
    dat[544] = *REG;
    dat[545] = *REG;
    dat[546] = *REG;
    dat[547] = *REG;
    dat[548] = *REG;
    dat[549] = *REG;
    dat[550] = *REG;
    dat[551] = *REG;
    dat[552] = *REG;
    dat[553] = *REG;
    dat[554] = *REG;
    dat[555] = *REG;
    dat[556] = *REG;
    dat[557] = *REG;
    dat[558] = *REG;
    dat[559] = *REG;
    dat[560] = *REG;
    dat[561] = *REG;
    dat[562] = *REG;
    dat[563] = *REG;
    dat[564] = *REG;
    dat[565] = *REG;
    dat[566] = *REG;
    dat[567] = *REG;
    dat[568] = *REG;
    dat[569] = *REG;
    dat[570] = *REG;
    dat[571] = *REG;
    dat[572] = *REG;
    dat[573] = *REG;
    dat[574] = *REG;
    dat[575] = *REG;
    dat[576] = *REG;
    dat[577] = *REG;
    dat[578] = *REG;
    dat[579] = *REG;
    dat[580] = *REG;
    dat[581] = *REG;
    dat[582] = *REG;
    dat[583] = *REG;
    dat[584] = *REG;
    dat[585] = *REG;
    dat[586] = *REG;
    dat[587] = *REG;
    dat[588] = *REG;
    dat[589] = *REG;
    dat[590] = *REG;
    dat[591] = *REG;
    dat[592] = *REG;
    dat[593] = *REG;
    dat[594] = *REG;
    dat[595] = *REG;
    dat[596] = *REG;
    dat[597] = *REG;
    dat[598] = *REG;
    dat[599] = *REG;
}
