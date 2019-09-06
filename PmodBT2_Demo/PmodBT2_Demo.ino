/************************************************************************/
/*				        	                        */
/*	  PmodBT2 Demo Project 		                                */
/*					    	                        */
/************************************************************************/
/*	Author: Eric Marsh					        */
/*	Copyright 2016, Digilent Inc.					*/
/************************************************************************/
/*  File Description: 			             		        */
/*					                        	*/
/* This file implements a simple demo application that demonstrates     */
/* how to setup and use the PmodBT2.				        */
/*									*/
/*	Functionality:							*/
/*									*/
/* In the setup() function, the PmodBT2 is initialized through          */
/* calling the BT2 library.                                             */
/*                                                                      */
/*                                                                      */
/* In the loop() function, the application repeatedly reads input       */
/* from PmodBT2, and sends a string to PmodBT2 using Serial ports and   */
/* the BT2 Library.                                                     */
/*					       	                        */
/*	Required Hardware:		                                */
/*	  1. PIC32 based Microcontroller    	                        */
/*	  2. PmodBT2                                             	*/
/*        3. Bluetooth compatible Bluetooth program to communicate      */
/*           to PmodBT2 with, such as Tera Term for PC or Blueterm      */
/*           for Android.                                               */
/*			                                                */
/************************************************************************/
/*  Revision History:			        			*/
/*					                        	*/
/*	07/13/2016(EricM): Created	       			        */
/*                                                                      */
/*					      	                        */
/************************************************************************/

/* -------------------------------------------------------------------- */
/*		        Include File Definitions                     	*/
/* -------------------------------------------------------------------- */
#include "BT2.h"

/* -------------------------------------------------------------------- */
/*		            Global Variables                     	*/
/* -------------------------------------------------------------------- */
BT2 bt;


void setup()
{
  // Initialize Serial to print to Serial Monitor
  Serial.begin(9600);
  Serial.println("Begin program...");
  
  // Initialize PmodBT2. Serial1 is the serial port used to commuinicate with PmodBT2
  bt.begin(Serial3, 9600); 
}


void loop()
{
  // Read data from PmodBT2, then print it to Serial Monitor
  Serial.println(bt.readSentence(Serial3));
  
  // Send string to PmodBT2
  bt.send(Serial3,"Hi! I was sent using PmodBT2! :)");
  
}
