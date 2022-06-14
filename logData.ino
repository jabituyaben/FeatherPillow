#include <SPI.h>
#include <SD.h>

#define ADC_GAIN_1  0x00
#define VBATPIN A7
#define ADC_PRESCALER_DIV64         0x04
#define ADC_PRESCALER_DIV128        0x05
#define ADC_PRESCALER_DIV256        0x06
#define ADC_PRESCALER_DIV512        0x07 // Arduino default

File root;
File logfile;
String stat = "";
int counter = 0;

static __inline__ void syncADC() __attribute__((always_inline, unused));
static void syncADC() {
  while(ADC->STATUS.bit.SYNCBUSY == 1);
}

void analogPrescaler(uint8_t val) {
  syncADC();
  ADC->CTRLB.bit.PRESCALER = val;
  syncADC();
}

//sets the gain of the ADC. See page 868. All values defined above. 
void analogGain(uint8_t gain) {
  syncADC();
  ADC->INPUTCTRL.bit.GAIN = gain;
  syncADC();
}

void CalibrateFeather(){
  syncADC();
  ADC->AVGCTRL.reg = ADC_AVGCTRL_SAMPLENUM_64 | ADC_AVGCTRL_ADJRES(4);
  ADC->CTRLB.reg |= ADC_CTRLB_RESSEL_16BIT;
  ADC->SAMPCTRL.reg = ADC_SAMPCTRL_SAMPLEN(2);
  syncADC();
}

void analogReferenceCompensation(uint8_t val) {
  if(val>0) val = 1; 
  syncADC();
  ADC->REFCTRL.bit.REFCOMP = val;
  syncADC();
}

void analogReference2(uint8_t ref) {
  syncADC();
  ADC->REFCTRL.bit.REFSEL = ref;
  syncADC();
}

struct datastore {
    //uint16_t timer;
    uint16_t eeg;
};

void error(uint8_t errno) {
  while(1) {
    uint8_t i;
    for (i=0; i<errno; i++) {
      digitalWrite(13, HIGH);
      delay(100);
      digitalWrite(13, LOW);
      delay(100);
    }
    for (i=errno; i<10; i++) {
      delay(200);
    }
  }
}

void reconnect(){
  logfile = SD.open("new.dat", FILE_WRITE);
  if( ! logfile ) {
    Serial.print("Couldnt create file"); 
  }
}

void printBatteryInfo(){
  float measuredvbat = analogRead(VBATPIN);
  measuredvbat *= 2;    // we divided by 2, so multiply back
  measuredvbat *= 3.3;  // Multiply by 3.3V, our reference voltage
  measuredvbat /= 1024; // convert to voltage
  Serial.print("VBat: " ); Serial.println(measuredvbat);
}

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);

  analogReadResolution(12);

  analogGain(ADC_GAIN_1);
  analogReferenceCompensation(1);
  analogReference2(ADC_REFCTRL_REFSEL_INTVCC0_Val);
  analogPrescaler(ADC_PRESCALER_DIV128);
  CalibrateFeather();
  
  printBatteryInfo();

  Serial.print("Initializing SD card...");

  if (!SD.begin(4)) {
    Serial.println("initialization failed!");
    stat = "failed";
    while (1);
  }
  else{
    Serial.println("passed");
  }
  char filename[15];
  strcpy(filename, "/ANALOG00.dat");
  
  for (uint8_t i = 0; i < 100; i++) {
    filename[7] = '0' + i/10;
    filename[8] = '0' + i%10;
    // create if does not exist, do not open existing, write, sync after write
    if (! SD.exists(filename)) {
      break;
    }
  }
  
  logfile = SD.open(filename, FILE_WRITE);
  if( ! logfile ) {
    Serial.print("Couldnt create "); 
    Serial.println(filename);
    error(3);
  }
  pinMode(13, OUTPUT);
  pinMode(8, OUTPUT);
}

void loop() {
    counter += 1;
    //digitalWrite(8, HIGH);
    struct datastore myData;
    //myData.timer = millis();
     myData.eeg = analogRead(2);
     Serial.println(myData.eeg);
     if (logfile) {
      if(!logfile.write((const uint8_t *)&myData, sizeof(myData))){
          Serial.println("error writing");
        }
     }
     //Serial.println(logfile.size());
     //Serial.println(logfile.name());
     delay(4);
     if(counter == 3000){
        //Serial.println(myData.eeg);
        logfile.flush();
        counter = 0;
     }
}
