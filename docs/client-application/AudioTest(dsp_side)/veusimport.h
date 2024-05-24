/******************************************************************************
 * FILE PURPOSE: VEU simulation prot file.
 ******************************************************************************
 * FILE NAME:   veusimport.h
 *
 * TABS: NONE
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
#ifndef _VEUSIMPORT_H
#define _VEUSIMPORT_H


#ifdef ti_targets_C55_large
#define _VEUSIMPORT_C55L 1
#else
#define _VEUSIMPORT_C55L 0
#endif

#ifdef ti_targets_C64
#define _VEUSIMPORT_C64 1
#else
#define _VEUSIMPORT_C64 0
#endif

#ifdef ti_targets_C64_big_endian
#define _VEUSIMPORT_C64_BIG_ENDIAN 1
#else
#define _VEUSIMPORT_C64_BIG_ENDIAN 0
#endif

#ifdef ti_targets_C64P
#define _VEUSIMPORT_C64P 1
#else
#define _VEUSIMPORT_C64P 0
#endif

#ifdef ti_targets_C64P_big_endian
#define _VEUSIMPORT_C64P_BIG_ENDIAN 1
#else
#define _VEUSIMPORT_C64P_BIG_ENDIAN 0
#endif

#ifdef ti_targets_elf_C64P 
#define _VEUSIMPORT_ELF_C64P 1
#else
#define _VEUSIMPORT_ELF_C64P 0
#endif

#ifdef ti_targets_elf_C64P_big_endian
#define _VEUSIMPORT_ELF_C64P_BIG_ENDIAN 1
#else
#define _VEUSIMPORT_ELF_C64P_BIG_ENDIAN 0
#endif

#ifdef ti_targets_C66
#define _VEUSIMPORT_C66 1
#else
#define _VEUSIMPORT_C66 0
#endif

#ifdef ti_targets_elf_C66
#define _VEUSIMPORT_ELF_C66 1
#else
#define _VEUSIMPORT_ELF_C66 0
#endif

#ifdef ti_targets_C66_big_endian
#define _VEUSIMPORT_C66_BIG_ENDIAN 1
#else
#define _VEUSIMPORT_C66_BIG_ENDIAN 0
#endif

#ifdef ti_targets_elf_C66_big_endian
#define _VEUSIMPORT_ELF_C66_BIG_ENDIAN 1
#else
#define _VEUSIMPORT_ELF_C66_BIG_ENDIAN 0
#endif


#if   (_VEUSIMPORT_C55L)
/* C55x definitions and macros */

static inline tint veu_fileio_read_int (FILE *fp, char *fmt, tint *value)
{
  tint num_read;
  num_read = fscanf (fp, fmt, value);
  return (num_read);
}  

#elif (_VEUSIMPORT_C64       || _VEUSIMPORT_C64_BIG_ENDIAN      || \
       _VEUSIMPORT_C64P      || _VEUSIMPORT_C64P_BIG_ENDIAN     || \
       _VEUSIMPORT_ELF_C64P  || _VEUSIMPORT_ELF_C64P_BIG_ENDIAN || \
       _VEUSIMPORT_C66       || _VEUSIMPORT_C66_BIG_ENDIAN      || \
       _VEUSIMPORT_ELF_C66   || _VEUSIMPORT_ELF_C66_BIG_ENDIAN)
/* C64x/C64x+/C66x definitions and macros */

static inline tint veu_fileio_read_int (FILE *fp, char *fmt, tint *value)
{
  tlong read_value;
  tint num_read;
  num_read = fscanf (fp, fmt, &read_value);
  *value = (tint) read_value;
  return (num_read);
}  
       
#else
#error invalid target
#endif

#endif /* _VEUSIMPORT_H */

/* nothing after this point */
