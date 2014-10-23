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


#ifndef BLUE_H_
#define BLUE_H_

// menu functions
extern void sx_blue(u8 line);
extern void mx_blue(u8 line);
extern void display_blue(u8 line, u8 update);

extern void reset_blue(void);
extern void blue_tick(void);
extern void update_blue_timer(void);
extern u8 is_blue(void);

#define BLUE_STOP	(0u)
#define BLUE_RUN	(1u)

struct bluetimer
{
  u8 state;
  u8 pos;
  u8 ticks;
  u8 time[8];
};

extern struct bluetimer sbluetimer;
#endif
