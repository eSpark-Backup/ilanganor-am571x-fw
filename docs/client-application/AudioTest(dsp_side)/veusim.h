#ifndef _VEUSIM_H
#define _VEUSIM_H
/******************************************************************************
 * FILE PURPOSE: Header file VEU unit-test. 
 ******************************************************************************
 * FILE NAME: veusim.h
 *
 * DESCRIPTION: Header file for VEU unit-test.
 *
 *        Copyright (c) 2007 – 2013 Texas Instruments Incorporated                
 *                                                                                
 *              All rights reserved not granted herein.                           
 *                                                                                
 *                         Limited License.                                       
 *                                                                                
 *  Texas Instruments Incorporated grants a world-wide, royalty-free,             
 *  non-exclusive license under copyrights and patents it now or hereafter owns   
 *  or controls to make, have made, use, import, offer to sell and sell           
 *  ("Utilize") this software subject to the terms herein.  With respect to the   
 *  foregoing patent license, such license is granted solely to the extent that   
 *  any such patent is necessary to Utilize the software alone.  The patent       
 *  license shall not apply to any combinations which include this software,      
 *  other than combinations with devices manufactured by or for TI (“TI           
 *  Devices”).  No hardware patent is licensed hereunder.                         
 *                                                                                
 *  Redistributions must preserve existing copyright notices and reproduce this   
 *  license (including the above copyright notice and the disclaimer and (if      
 *  applicable) source code license limitations below) in the documentation       
 *  and/or other materials provided with the distribution                         
 *                                                                                
 *  Redistribution and use in binary form, without modification, are permitted    
 *  provided that the following conditions are met:                               
 *                                                                                
 *    *  No reverse engineering, decompilation, or disassembly of this software   
 *  is permitted with respect to any software provided in binary form.            
 *                                                                                
 *    *  any redistribution and use are licensed by TI for use only with TI       
 *  Devices.                                                                      
 *                                                                                
 *    *  Nothing shall obligate TI to provide you with source code for the        
 *  software licensed and provided to you in object code.                         
 *                                                                                
 *  If software source code is provided to you, modification and redistribution   
 *  of the source code are permitted provided that the following conditions are   
 *  met:                                                                          
 *                                                                                
 *    *  any redistribution and use of the source code, including any resulting   
 *  derivative works, are licensed by TI for use only with TI Devices.            
 *                                                                                
 *    *  any redistribution and use of any object code compiled from the source   
 *  code and any resulting derivative works, are licensed by TI for use only      
 *  with TI Devices.                                                              
 *                                                                                
 *  Neither the name of Texas Instruments Incorporated nor the names of its       
 *  suppliers may be used to endorse or promote products derived from this        
 *  software without specific prior written permission.                           
 *                                                                                
 *  DISCLAIMER.                                                                   
 *                                                                                
 *  THIS SOFTWARE IS PROVIDED BY TI AND TI’S LICENSORS "AS IS" AND ANY EXPRESS    
 *  OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED             
 *  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE        
 *  DISCLAIMED. IN NO EVENT SHALL TI AND TI’S LICENSORS BE LIABLE FOR ANY         
 *  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES    
 *  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR            
 *  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER    
 *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT            
 *  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY     
 *  OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH   
 *  DAMAGE.                                                                       
 *****************************************************************************/
#include <types.h>

#define NUMBER_OF_FRAMES       1       /* # of frames to save */
#define VEUSIM_HEAP_SIZE		2048

/******************************************************************************
 * FUNCTION PURPOSE: Initialize test vectors settings 
 ******************************************************************************
 * DESCRIPTION: 
 *
 * CALLING FORMAT:
 *   veusim_init_testvector (tint file_id)
 *
 * RETURN VALUE:
 *   None
 * 
 *****************************************************************************/
void veusim_init_testvector ();

/******************************************************************************
 * FUNCTION PURPOSE: release test vectors / close file handles 
 ******************************************************************************
 * DESCRIPTION: 
 *
 * CALLING FORMAT:
 *   veusim_release_testvector()
 *
 * RETURN VALUE:
 *   None
 * 
 *****************************************************************************/
void veusim_release_testvector ();

/******************************************************************************
 * FUNCTION PURPOSE: Read a Tx frame 
 ******************************************************************************
 * DESCRIPTION: 
 *
 * CALLING FORMAT:
 *   tint veusim_read_frame (tint *inBuf, FILE *inFile, tint frmSize)
 *
 * RETURN VALUE:
 *   flag:   0 - end of file (error)
 *           1 - read successfully
 * 
 *****************************************************************************/
tint veusim_read_frame (tint *inBuf, FILE *inFile, tint frmSize);

/******************************************************************************
 * FUNCTION PURPOSE: create a veu instance
 ******************************************************************************
 * DESCRIPTION: 
 *
 * CALLING FORMAT:
 *   void veusim_new_veu ()
 *
 * RETURN VALUE:
 *   None
 * 
 *****************************************************************************/
void veusim_new_veu ();


/******************************************************************************
 * FUNCTION PURPOSE: create a veu instance
 ******************************************************************************
 * DESCRIPTION: 
 *
 * CALLING FORMAT:
 *   void veusim_open_veu ()
 *
 * RETURN VALUE:
 *   None
 * 
 *****************************************************************************/
void veusim_open_veu (tint frame_size);

/******************************************************************************
 * FUNCTION PURPOSE: create a veu instance
 ******************************************************************************
 * DESCRIPTION: 
 *
 * CALLING FORMAT:
 *   void veusim_open_veu ()
 *
 * RETURN VALUE:
 *   None
 * 
 *****************************************************************************/
void veusim_control_veu ();

/******************************************************************************
 * FUNCTION PURPOSE: setup the parameters of the veu instance for different tests
 ******************************************************************************
 * DESCRIPTION: 
 *
 * CALLING FORMAT:
 *   void veusim_setup_veu ()
 *
 * RETURN VALUE:
 *   None
 * 
 *****************************************************************************/
void veusim_param_veu ();



//void VeuProcess (char * rxBuff, char * txBuff, unsigned int size);

void veuInit();

void ReadFrame(short * _audioBuff);
#endif /* _VEUSIM_H */
/* nothing after this point */
