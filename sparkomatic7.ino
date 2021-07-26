// sparkomatic 7 - ignitor replacement test : Only process/use N/N+/N- , IGT, and IGF
//
// by Mike (2021)
//
// released under a ('the') MIT license
//
// Purpose: To act as a substitute for an ignitor circuit used in a 4AGE engine (used in classic Toyota MR2 (and other) cars.
//
// Rationalle factors:  Ignitors are getting more scarce
//                      No need to recycle a classic car just because an ignitor went
//                      Anecdotal reports (enthusiast message boards) suggest electrical failure of ignitor and ecu can occur for cryptic reasons during engine work (this came about due to a rebuild)
//                      Might be in a hurry 
//                      Might want to learn about microcontrollers and how they integrate into automotive solutions
//                      Might want more control over componets (factory ignitor is a black box and cannot be modified, and cannot generate any logs)
//
//      NOTE: The scope is limited to the igniter. If you want a complete solution, please consider COP (coil on plugs) together with a megasquirt or (even better) a https://speeduino.com/home/ ecu
//
// MCU used in prototype: Arduino Nano Every.  
//
// 
//
// 1 - brute force method
// 2 - add IGF
//      change pulse durations to those seen on internet plots: IGT: 8-10 ms@5V every 44ms or so...   (default = low)
//                                                              IGF: 6 ms @0V 2~4 ms after IGT start  (default = high)
//* work in progress
//  3 --> using brute-force method (delay)
//  4 --> like 3, using millis instead of delay
//  5 --> using interrupts (driven through GPIO inputs for N (cam) and G (crank); POC working...
//  6 --> incorporate functions (IGT, IGF, injectors' activation)
//  7 --> ingitor replacement test
//        produce N (to ecu) from cam sensor input (N+-) --> note needs to be CMOS out (vcc=12V)
//        produce/process IGT (if any) from ECU
//        produce IGF in response to IGT
//        Diag outputs used with LEDs. 
//
//  Note: Tested working on 4age engine. (But not at high RPMs)
//
// declare variables and constants
// 
#define IGTout  8                                                 // used for IGT - Trigger   - output to ignitor
#define IGTin   19                                                // used for IGT - Trigger   - input from ECU
#define IGF     20                                                // used for IGF - Feedback  - output to ECU
#define N       4                                                 // used for N - (N+, N-) cam signals (FaradayZLaw--> Schmidt trigger (LM358)--> GPIO input)
#define NE      21                                                // used for NE output from MCU (this) to ECU
#define NEchk   14                                                // ne out checking led
#define IGFchk  15                                                // igf checking led
#define IGTchk  16                                                // igt checking led
//
// timing trackers
// 
unsigned long interval2 = 44;
unsigned long previous = interval2;                               // will store last time LED was updated
unsigned long current = 0;                                        // current time counter
unsigned long IGTdur = 0;                                         // IGT duration time counter
unsigned long IGFdel = 0;                                         // IGF delay time counter
unsigned long IGT_peak  = 10;                                     // expectation of IGT to last 10 ms 
unsigned long IGF_peak  = 6;                                      // expectation of IGF to last 6 ms 
unsigned long IGF_delay = 3;                                      // expectation of IGT to lag behind IGT by 2 ms 
unsigned int n_count = 0;                                         // counter for the number of rotations of the N (cam) input
volatile int igt_state = 0;                                       // state of igt - for diagnostic purposes
volatile int n_state = 0;                                         // state of NE - for diagnostic purposes
volatile int igf_state = 0;                                       // state of IGF - for diagnostic purposes
//
//  
//
void setup() {
  pinMode(IGTout, OUTPUT);                                        // IGT signal (output to igniter)
  pinMode(IGTin, INPUT_PULLUP);                                   // IGT signal (input from ECU)
  pinMode(IGF, OUTPUT);                                           // IGF signal (output to ECU)
  pinMode(N, INPUT_PULLUP);                                       // N (cam position) signal input ( from distributor sensor )
  pinMode(NE, OUTPUT);                                            // NE signal (output to ECU)
  pinMode(NEchk, OUTPUT);                                         // diag (output to ECU)
  pinMode(IGTchk, OUTPUT);                                        // diag (output to igniter)
  pinMode(IGFchk, OUTPUT);                                        // diag (output to ECU)
  attachInterrupt(digitalPinToInterrupt(N), cam_irq, CHANGE);     // interrupt for the cam 
  attachInterrupt(digitalPinToInterrupt(IGTin), igt_irq, CHANGE); // interrupt for the IGT (spark trigger) pulse 
}
//
// track the camshaft
//
void cam_irq(void) {                                              // track the camshaft movements -- echo them to the ECU
  //consider delay to trap glitches...
  //delayMicroseconds(100);                                       // pauses for 100 microseconds -- arbitrarly chosen value
  if ( digitalRead(N) == LOW ) {
    digitalWrite(NE, HIGH);                                       // inverted due to 2n3094 used to pull up vout to +12V max (CMOS)
    n_state = 0;
    }
  if ( digitalRead(N) == HIGH ) {
    digitalWrite(NE, LOW);                                        // inverted due to 2n3094 used to pull up vout to +12V max (CMOS)
    n_state = 1;
    }
}
//
// service the IGT pulse
//
void igt_irq(void) {                                              // track the camshaft movements -- echo them to the ECU
  //consider delay to trap glitches...
  //delayMicroseconds(100);                                       // pauses for 100 microseconds -- arbitrarly chosen value
  if ( digitalRead(IGTin) == LOW ) {
    igt_state = 0;
    digitalWrite(IGF, LOW);
    igf_state = 0;
    }
  if ( digitalRead(IGTin) == HIGH ) 
    igt_state = 1;
}
//
//  main routine below to control the unit based on the fuel/spark expectations
//
void loop() {
  if ( ( igt_state == 0 ) and (digitalRead(IGTin) == LOW ) ) {
    digitalWrite(IGTout, LOW);
    digitalWrite(IGF, LOW);
    igf_state = 0;
    }
  if ( ( igt_state == 1 ) and ( digitalRead(IGTin) == HIGH ) ) {
    digitalWrite(IGTout, HIGH);
    delayMicroseconds(2000);                                      // delay 2000 us  = 2ms for feedback pulse
    digitalWrite(IGF, HIGH);
    igf_state = 1;
    }
  if ( n_state == 0)                                              // trouble and diag lights follow
    digitalWrite(NEchk, LOW);
  if ( n_state == 1) 
    digitalWrite(NEchk, HIGH);
  if ( igt_state == 0) 
    digitalWrite(IGTchk, LOW);
  if ( igt_state == 1) 
    digitalWrite(IGTchk, HIGH);
  if ( igf_state == 0) 
    digitalWrite(IGFchk, LOW);
  if ( igf_state == 1) 
    digitalWrite(IGFchk, HIGH);
  //
  // trust in the interrupts to handle the signals... for the mostpart
}
