/*
 * Blue print watch firmware - Performs fall detection
 * 
 * Copyright (C) 2014 by Julien Beaudaux for the Blue print project
 * 
 *                        All Rights Reserved
 * Permission to use, copy, modify, and distribute this software and
 * its documentation for any purpose is forbidden unless specific
 * prior written permission is granted by its original owners;
 *
 * Neither the name of the Blue print project nor the names of its
 * contributors may be used to endorse or promote products derived
 * from this software without specific prior written permission.
 *
 * The authors and the Blue print project disclaims all warranties
 * with regard to this software, including all implied warranties
 * of merchantability and fitness. In no wvent shall the authors and
 * the Blue print project be liable for any special, indirect or
 * consequencial damages or any damages whatsoever.
*/

// *************************************************************************************************
//
//	Copyright (C) 2009 Texas Instruments Incorporated - http://www.ti.com/ 
//	 
//	 
//	  Redistribution and use in source and binary forms, with or without 
//	  modification, are permitted provided that the following conditions 
//	  are met:
//	
//	    Redistributions of source code must retain the above copyright 
//	    notice, this list of conditions and the following disclaimer.
//	 
//	    Redistributions in binary form must reproduce the above copyright
//	    notice, this list of conditions and the following disclaimer in the 
//	    documentation and/or other materials provided with the   
//	    distribution.
//	 
//	    Neither the name of Texas Instruments Incorporated nor the names of
//	    its contributors may be used to endorse or promote products derived
//	    from this software without specific prior written permission.
//	
//	  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
//	  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
//	  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
//	  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
//	  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
//	  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
//	  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
//	  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
//	  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
//	  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
//	  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//

#include "project.h"

#ifdef CONFIG_BLUE

// driver
#include "display.h"
#include "vti_ps.h"
#include "ports.h"
#include "timer.h"

#include "stopwatch.h"

// logic
#include "user.h"
#include "blue.h"

#include "menu.h"

struct bluetimer sbluetimer;

#define BLUE_STR " LUEPRINT LUEPRINT"

#define AGGREG 50	//uses the mean of AGGREG values to calculate SSE

static int dataAgg[AGGREG];
static u8 position = 0;
static u8 *str = BLUE_STR;
static int SSE = 0;

void blue_tick()
{
  sbluetimer.pos = (sbluetimer.pos+1) % (sizeof(BLUE_STR)-7);
  display_blue(0, 0);
}

u8 is_blue(void)
{
  return (sbluetimer.state == BLUE_RUN &&  (ptrMenu_L2 == &menu_L2_Blue));
}


uint8 sqrti(uint8 x)
{
	uint8 a,b;
	b = x;
	a = x = 0x3f;
	x = b/x;
	a = x = (x+a)>>1;
	x = b/x;
	a = x = (x+a)>>1;
	x = b/x;
	x = (x+a)>>1;
	return x;
}


// Updates the SSE to discriminate patterns
uint8 update_SSE(uint8 val)
{
	//return SSE;
}

// Takes axial values and assess current behavior
uint8 assess_behavior(uint8 xax, uint8 yax, uint8 zax)
{
	uint8 norm;
	uint8 stdv;
	
	norm = sqrti((xax*xax)+(yax*yax)+(zax*zax));
	
	// Get lower (lie) and upper (fall) bounds
	stdv = update_SSE(norm);
	stdv = sqrti(stdv/iter);
	
	if(norm > FALLTHRESH*stdv)
	{
		return 2;	// Potential fall
	}
	else if(norm < stdv/LIETHRESH)
	{
		return 1;	// Abnormally low activity
	}
	
	return 0;	// Normal behavior
}

void update_blue_timer()
{
  /* TA0CCR2 = TA0CCR2 + STOPWATCH_1HZ_TICK; */
}

void start_blue()
{

  sbluetimer.state = BLUE_RUN;
  /* // Init CCR register with current time */
  /* TA0CCR2 = TA0R; */
		
  /* // Load CCR register with next capture time */
  /* update_blue_timer(); */

  /* // Reset IRQ flag     */
  /* TA0CCTL2 &= ~CCIFG;  */
	          
  /* // Enable timer interrupt     */
  /* TA0CCTL2 |= CCIE;  */
	
  display_symbol(LCD_ICON_RECORD, SEG_ON);
}

void stop_blue()
{
  /* // Clear timer interrupt enable    */
  /* TA0CCTL2 &= ~CCIE;  */

  sbluetimer.state = BLUE_STOP;
	
  display_symbol(LCD_ICON_RECORD, SEG_OFF);

  // Call draw routine immediately
  display_blue(LINE2, DISPLAY_LINE_UPDATE_FULL);
}

void sx_blue(u8 line)
{
  if (button.flag.down)
    {
      if (sbluetimer.state == BLUE_STOP){
        start_blue();
      } else {
        stop_blue();
      }
    }
}

void mx_blue(u8 line)
{
}

void display_blue(u8 line, u8 update)
{
  u8 cur[7];
  memcpy(cur, str + sbluetimer.pos, 6);
  cur[6] = 0;
  
  display_chars(LCD_SEG_L2_5_0, cur, SEG_ON);
}

void reset_blue(void)
{
  sbluetimer.pos = 0;
  sbluetimer.state = BLUE_STOP;
}

#endif /* CONFIG_BLUE */
