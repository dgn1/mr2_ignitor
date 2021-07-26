# mr2_ignitor
A DIY alternate ignitor module for the Toyota MR2 (tested on classic models)

In-code docs will have more data...please consider to read through them...

An arduino nano every was programmed and connected to replace a (faulty) ignitor module in a toyota mr2 4age engine . 

The ignitor module performs the following functions: 
-- take the N+ and N- (cam) signals from an inductive sensor with ballanced lines and convert to an un-ballanced output of +12V (with respect to ground). This will be the N signal [from sensor in distributor]
-- convey the N signal to the ECU
-- Produce a high frequency impulse (to initiate spark) [to coil]  when a signal is recieved on the IGT (ignitor trigger) line [from ECU]
-- let the ecu know when the spark was sent to the coil through the IGF (ignition feedback) line [to ecu]

What is needed: a proper ignition output chip to send an impulse to the coil (or use coils on plugs).

