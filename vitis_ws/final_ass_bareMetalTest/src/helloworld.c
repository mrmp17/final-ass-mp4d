/******************************************************************************
*
* Copyright (C) 2009 - 2014 Xilinx, Inc.  All rights reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* Use of the Software is limited solely to applications:
* (a) running on a Xilinx device, or
* (b) that interact with a Xilinx device through a bus or interconnect.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
* XILINX  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
* OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
* Except as contained in this notice, the name of the Xilinx shall not be used
* in advertising or otherwise to promote the sale, use or other dealings in
* this Software without prior written authorization from Xilinx.
*
******************************************************************************/

/*
 * helloworld.c: simple test application
 *
 * This application configures UART 16550 to baud rate 9600.
 * PS7 UART (Zynq) is not initialized by this application, since
 * bootrom/bsp configures it to baud rate 115200
 *
 * ------------------------------------------------
 * | UART TYPE   BAUD RATE                        |
 * ------------------------------------------------
 *   uartns550   9600
 *   uartlite    Configurable only in HW design
 *   ps7_uart    115200 (configured by bootrom/bsp)
 */

#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"
#include "xbram.h"
#include "xparameters.h"
#include <sleep.h>
#include "test_images.h"

#define BRAM(A)     ((volatile u32*)px_config->MemBaseAddress)[A]
#define NUM_INPUTS		100 // number of pixel in input image
#define BYTES_PR_INPUT	4 	// 32 bit float = 4 bytes
#define BASE_ADDR		XPAR_AXI_BRAM_CTRL_0_S_AXI_BASEADDR	 // from xparameters.h

XBram             	x_bram;
XBram_Config    	*px_config;

uint8_t 		ucAXIInit();


int main()
{
    init_platform();

    ucAXIInit();

    printf("Hello World\n\r");
    printf("Successfully ran Hello World application\n\r");


    printf("start BRAM test\n\r");
    BRAM(1024) = 0xFA;
    usleep(10000);
    u32 valback = BRAM(1024);
    printf("(should be oxFA) got back from BRAM: %X \n\r", valback);

    printf("BRAM test finished! \n\r");

    printf("start filling up bram with a picture... \n\r");

    //u32 imNumber = 12;

    for(u32 n=0 ; n<20 ; n++){
		u32 imgToLoad[400];
		for(u32 i = 0 ; i<400 ; i++){
			imgToLoad[i] = *(u32 *)(&images_array[n][i]);
		}

		for(u32 i = 0 ; i<400 ; i++){
			BRAM(i) = imgToLoad[i];
		}
		//printf("just check random bram location: %d \n\r", BRAM(200));
		//printf("picture loaded up in bram...wait for inference \n\r");
		usleep(10000);
		printf("inference result: %d \n\r", BRAM(512));
		usleep(1000);
    }
    printf("ok byeee... \n\r");


    cleanup_platform();
    return 0;
}






uint8_t 	ucAXIInit(){
	/*************************
	*  BRAM initialization   *
	*************************/
	xil_printf("\n\rBRAM init...\n");
	px_config = XBram_LookupConfig(XPAR_BRAM_0_DEVICE_ID);
	if (px_config == (XBram_Config *) NULL) {
		xil_printf("\n\rBRAM init failed\n");
		return XST_FAILURE;
	}
	int x_status 	= 	XBram_CfgInitialize(&x_bram, px_config,
			px_config->CtrlBaseAddress);
	if (x_status != XST_SUCCESS) {
		xil_printf("\n\rBRAM init failed\n");
		return XST_FAILURE;
	}
	xil_printf("\n\rBRAM init success\n");
	return XST_SUCCESS;
}
