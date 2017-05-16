/* ###################################################################
**     This component module is generated by Processor Expert. Do not modify it.
**     Filename    : I2CSPY1.c
**     Project     : FRDM-KL27Z_McuOnEclipseLib
**     Processor   : MKL25Z128VLK4
**     Component   : I2CSpy
**     Version     : Component 01.014, Driver 01.00, CPU db: 3.00.000
**     Repository  : Legacy User Components
**     Compiler    : GNU C Compiler
**     Date/Time   : 2017-05-16, 20:12, # CodeGen: 190
**     Abstract    :
**          This component implements a utility to inspect devices on the I2C bus.
**     Settings    :
**          Component name                                 : I2CSPY1
**          I2C                                            : GI2C1
**          Default Address                                : 0x0
**          Default Address Size                           : 1
**          Default Bytes per Line                         : 8
**          SDK                                            : MCUC1
**          Shell                                          : Enabled
**            Shell                                        : CLS1
**            Utility                                      : UTIL1
**     Contents    :
**         SetDeviceAddress - uint8_t I2CSPY1_SetDeviceAddress(uint8_t addr);
**         GetDeviceAddress - uint8_t I2CSPY1_GetDeviceAddress(void);
**         SetAddressSize   - uint8_t I2CSPY1_SetAddressSize(uint8_t size);
**         SetBytesPerLine  - uint8_t I2CSPY1_SetBytesPerLine(uint8_t nofBytesPerLine);
**         ReadRegData      - uint8_t I2CSPY1_ReadRegData(uint32_t addr, uint8_t *data, uint16_t dataSize);
**         WriteRegData     - uint8_t I2CSPY1_WriteRegData(uint32_t addr, uint8_t *data, uint16_t dataSize);
**         ParseCommand     - uint8_t I2CSPY1_ParseCommand(const unsigned char *cmd, bool *handled, const...
**         Deinit           - void I2CSPY1_Deinit(void);
**         Init             - uint8_t I2CSPY1_Init(void);
**
**     * Copyright (c) 2013-2017, Erich Styger
**      * Web:         https://mcuoneclipse.com
**      * SourceForge: https://sourceforge.net/projects/mcuoneclipse
**      * Git:         https://github.com/ErichStyger/McuOnEclipse_PEx
**      * All rights reserved.
**      *
**      * Redistribution and use in source and binary forms, with or without modification,
**      * are permitted provided that the following conditions are met:
**      *
**      * - Redistributions of source code must retain the above copyright notice, this list
**      *   of conditions and the following disclaimer.
**      *
**      * - Redistributions in binary form must reproduce the above copyright notice, this
**      *   list of conditions and the following disclaimer in the documentation and/or
**      *   other materials provided with the distribution.
**      *
**      * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
**      * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
**      * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
**      * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
**      * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
**      * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
**      * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
**      * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
**      * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
**      * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
** ###################################################################*/
/*!
** @file I2CSPY1.c
** @version 01.00
** @brief
**          This component implements a utility to inspect devices on the I2C bus.
*/         
/*!
**  @addtogroup I2CSPY1_module I2CSPY1 module documentation
**  @{
*/         

/* MODULE I2CSPY1. */

#include "I2CSPY1.h"

#define I2CSPY1_DEFAULT_I2C_ADDR        0
#define I2CSPY1_DEFAULT_ADDR_SIZE       1
#define I2CSPY1_DEFAULT_BYTES_PER_LINE  8

typedef struct {
  uint8_t deviceAddr; /* I2C 7bit device address */
  uint8_t addrSize; /* number of address bytes, e.g. 1, 2, 3 or 4 bytes */
  uint8_t bytesPerLine; /* number of bytes per line for dump */
} I2CSPY1_TDataState;

static I2CSPY1_TDataState I2CSPY1_deviceData;

#define MAX_NOF_BYTES_PER_LINE 32 /* maximum number of bytes per line */

static void strcatAddress(unsigned char *buf, size_t bufSize, uint32_t addr) {
  if (I2CSPY1_deviceData.addrSize==1) {
    UTIL1_strcatNum8Hex(buf, bufSize, (uint8_t)addr);
  } else if (I2CSPY1_deviceData.addrSize==2) {
    UTIL1_strcatNum16Hex(buf, bufSize, (uint16_t)addr);
  } else if (I2CSPY1_deviceData.addrSize==3) {
    UTIL1_strcatNum24Hex(buf, bufSize, addr);
  } else if (I2CSPY1_deviceData.addrSize==4) {
    UTIL1_strcatNum32Hex(buf, bufSize, addr);
  }
}

static uint8_t Dump(unsigned long startAddr, unsigned long endAddr, const CLS1_StdIOType *io) {
  #define NOF_BYTES_PER_LINE 32 /* how many bytes are shown on a line. This defines as well the chunk size we read from memory */
  static uint8_t buf[MAX_NOF_BYTES_PER_LINE]; /* this is the chunk of data we get (per line in output) */
  static uint8_t str[3*MAX_NOF_BYTES_PER_LINE+((MAX_NOF_BYTES_PER_LINE+1)/8)+1]; /* maximum string for output:
                                              - '3*' because each byte is 2 hex digits plus a space
                                              - '(NOF_BYTES_PER_LINE+1)/8' because we add a space between every 8 byte block
                                              - '+1' for the final zero byte */
  unsigned long addr;
  uint8_t res=0, j, bufSize;
  uint8_t ch;

  if (endAddr<startAddr) {
    CLS1_SendStr((unsigned char*)"\r\n*** End address must be larger or equal than start address\r\n", io->stdErr);
    return ERR_RANGE;
  }
  CLS1_SendStatusStr((unsigned char*)"device", (unsigned char*)"0x", io->stdOut);
  str[0]='\0';
  strcatAddress(str, sizeof(str), I2CSPY1_deviceData.deviceAddr);
  UTIL1_strcat(str, sizeof(str), (unsigned char*)"\r\n");
  CLS1_SendStr((unsigned char*)str, io->stdOut);

  CLS1_SendStatusStr((unsigned char*)"start", (unsigned char*)"0x", io->stdOut);
  str[0]='\0';
  strcatAddress(str, sizeof(str), startAddr);
  UTIL1_strcat(str, sizeof(str), (unsigned char*)"\r\n");
  CLS1_SendStr((unsigned char*)str, io->stdOut);

  CLS1_SendStatusStr((unsigned char*)"end", (unsigned char*)"0x", io->stdOut);
  str[0]='\0';
  strcatAddress(str, sizeof(str), endAddr);
  UTIL1_strcat(str, sizeof(str), (unsigned char*)"\r\n");
  CLS1_SendStr((unsigned char*)str, io->stdOut);

  CLS1_SendStatusStr((unsigned char*)"Addr Size", (unsigned char*)"", io->stdOut);
  CLS1_SendNum8u(I2CSPY1_deviceData.addrSize, io->stdOut);
  CLS1_SendStr((unsigned char*)"\r\n", io->stdOut);

  for(addr=startAddr; addr<=endAddr; /* nothing */ ) {
    if (endAddr-addr+1 >= I2CSPY1_deviceData.bytesPerLine) { /* read only part of buffer */
      bufSize = I2CSPY1_deviceData.bytesPerLine; /* read full buffer */
    } else {
      bufSize = (uint8_t)(endAddr-addr+1);
    }
    if (I2CSPY1_ReadRegData(addr, buf, bufSize)!=ERR_OK) {
      CLS1_SendStr((unsigned char*)"\r\n*** Read failed!\r\n", io->stdErr);
      return ERR_FAILED;
    }
    if (res != ERR_OK) {
      CLS1_SendStr((unsigned char*)"\r\n*** Failure reading memory block!\r\n", io->stdErr);
      return ERR_FAULT;
    }
    /* write address */
    UTIL1_strcpy(str, sizeof(str), (unsigned char*)"0x");
    strcatAddress(str, sizeof(str), addr);
    UTIL1_chcat(str, sizeof(str), ':');
    CLS1_SendStr((unsigned char*)str, io->stdOut);
    /* write data in hex */
    str[0] = '\0';
    for (j=0; j<bufSize; j++) {
      if ((j%8)==0) {
        UTIL1_chcat(str, sizeof(str), ' ');
      }
      UTIL1_strcatNum8Hex(str, sizeof(str), buf[j]);
      UTIL1_chcat(str, sizeof(str), ' ');
    }
    for (/*empty*/; j<I2CSPY1_deviceData.bytesPerLine; j++) { /* fill up line */
      UTIL1_strcat(str, sizeof(str), (unsigned char*)"-- ");
    }
    CLS1_SendStr((unsigned char*)str, io->stdOut);
    /* write in ASCII */
    io->stdOut(' ');
    for (j=0; j<bufSize; j++) {
      ch = buf[j];
      if (ch >= ' ' && ch <= 0x7f) {
        io->stdOut(ch);
      } else {
        io->stdOut('.'); /* place holder */
      }
    }
    for (/*empty*/; j<I2CSPY1_deviceData.bytesPerLine; j++) { /* fill up line */
      UTIL1_strcat(str, sizeof(str), (unsigned char*)"-- ");
    }
    CLS1_SendStr((unsigned char*)"\r\n", io->stdOut);
    addr += I2CSPY1_deviceData.bytesPerLine;
  }
  CLS1_SendStr((unsigned char*)"\r\n", io->stdOut);
  return ERR_OK;
}

static uint8_t Read(uint32_t addr, const CLS1_StdIOType *io) {
  uint8_t val;
  uint8_t hexBuf[3];

  if (I2CSPY1_ReadRegData(addr, &val, 1)!=ERR_OK) {
    CLS1_SendStr((unsigned char*)"\r\n*** read failed!\r\n", io->stdErr);
    return ERR_FAILED;
  }
  CLS1_SendStr((unsigned char*)"value: 0x", io->stdOut);
  hexBuf[0] = '\0';
  UTIL1_strcatNum8Hex(hexBuf, sizeof(hexBuf), val);
  CLS1_SendStr(hexBuf, io->stdOut);
  CLS1_SendStr((unsigned char*)"\r\n", io->stdOut);
  return ERR_OK;
}

static void ScanDevices(const CLS1_StdIOType *io) {
  uint8_t addr, oldAddr;
  uint8_t buf[3], res;

  oldAddr = I2CSPY1_GetDeviceAddress();
  for (addr=0; addr<0x7f; addr++) {
    CLS1_SendStr((unsigned char*)"scan of I2C device addr 0x", io->stdOut);
    buf[0] = '\0';
    UTIL1_strcatNum8Hex(buf, sizeof(buf), addr);
    CLS1_SendStr(buf, io->stdOut);
    if (I2CSPY1_SetDeviceAddress(addr)!=ERR_OK) {
      CLS1_SendStr((unsigned char*)": failed selecting device\r\n", io->stdErr);
      break; /* get out of for loop */
    }
    res = GI2C1_ScanDevice(addr);
    if (res==ERR_NOTAVAIL) {
      CLS1_SendStr((unsigned char*)": no ACK\r\n", io->stdErr);
    } else if (res==ERR_OK) {
      CLS1_SendStr((unsigned char*)": Device responded!\r\n", io->stdErr);
    } else {
      CLS1_SendStr((unsigned char*)": *** failed! ***\r\n", io->stdErr);
    }
  } /* for */
  (void)I2CSPY1_SetDeviceAddress(oldAddr); /* restore old device address */
}

static uint8_t PrintStatus(const CLS1_StdIOType *io) {
  unsigned char buf[3];

  CLS1_SendStatusStr((unsigned char*)"I2CSPY1", (unsigned char*)"\r\n", io->stdOut);
  CLS1_SendStatusStr((unsigned char*)"  deviceAddr", (unsigned char*)"0x", io->stdOut);
  buf[0] = '\0';
  UTIL1_strcatNum8Hex(buf, sizeof(buf), I2CSPY1_GetDeviceAddress());
  CLS1_SendStr(buf, io->stdOut);
  CLS1_SendStr((unsigned char*)"\r\n", io->stdOut);
  CLS1_SendStatusStr((unsigned char*)"  addrSize", (unsigned char*)"", io->stdOut);
  CLS1_SendNum8u(I2CSPY1_deviceData.addrSize, io->stdOut);
  CLS1_SendStr((unsigned char*)"\r\n", io->stdOut);
  CLS1_SendStatusStr((unsigned char*)"  bytes/Line", (unsigned char*)"", io->stdOut);
  CLS1_SendNum8u(I2CSPY1_deviceData.bytesPerLine, io->stdOut);
  CLS1_SendStr((unsigned char*)"\r\n", io->stdOut);
  return ERR_OK;
}

static uint8_t PrintHelp(const CLS1_StdIOType *io) {
  CLS1_SendHelpStr((unsigned char*)"I2CSPY1", (unsigned char*)"Group of I2CSPY1 commands\r\n", io->stdOut);
  CLS1_SendHelpStr((unsigned char*)"  help|status", (unsigned char*)"Print help or status information\r\n", io->stdOut);
  CLS1_SendHelpStr((unsigned char*)"  bytesPerLine <number>", (unsigned char*)"Number of bytes per line for dump command (1..32)\r\n", io->stdOut);
  CLS1_SendHelpStr((unsigned char*)"  scan", (unsigned char*)"Scans device addresses\r\n", io->stdOut);
  CLS1_SendHelpStr((unsigned char*)"  device 0x<addr>", (unsigned char*)"Set the I2C device address\r\n", io->stdOut);
  CLS1_SendHelpStr((unsigned char*)"  addrSize <size>", (unsigned char*)"Address size used for device memory map (1, 2, 3 or 4)\r\n", io->stdOut);
  CLS1_SendHelpStr((unsigned char*)"  read 0x<addr>", (unsigned char*)"Read a byte from an address\r\n", io->stdOut);
  CLS1_SendHelpStr((unsigned char*)"  write 0x<addr> 0x<data>", (unsigned char*)"Write a data to an address. 0x<data> can be multiple items.\r\n", io->stdOut);
  CLS1_SendHelpStr((unsigned char*)"  dump 0x<start>..0x<end>", (unsigned char*)"Read data from an address range\r\n", io->stdOut);
  return ERR_OK;
}

/*
** ===================================================================
**     Method      :  I2CSPY1_ParseCommand (component I2CSpy)
**     Description :
**         Shell Command Line parser
**     Parameters  :
**         NAME            - DESCRIPTION
**       * cmd             - Pointer to command string
**       * handled         - Pointer to variable which tells if
**                           the command has been handled or not
**         io              - Pointer to I/O structure
**     Returns     :
**         ---             - Error code
** ===================================================================
*/
uint8_t I2CSPY1_ParseCommand(const unsigned char *cmd, bool *handled, const CLS1_StdIOType *io)
{
  uint8_t addr8, value8;
  uint32_t addr32, end32;
  const unsigned char *p;
  uint8_t cnt;
  uint8_t buf[16];

  if (UTIL1_strcmp((char*)cmd, CLS1_CMD_HELP)==0 || UTIL1_strcmp((char*)cmd, "I2CSPY1 help")==0) {
    *handled = TRUE;
    return PrintHelp(io);
  } else if ((UTIL1_strcmp((char*)cmd, CLS1_CMD_STATUS)==0) || (UTIL1_strcmp((char*)cmd, "I2CSPY1 status")==0)) {
    *handled = TRUE;
    return PrintStatus(io);
  } else if (UTIL1_strncmp((char*)cmd, "I2CSPY1 bytesPerLine ", sizeof("I2CSPY1 bytesPerLine ")-1)==0) {
    p = cmd+sizeof("I2CSPY1 bytesPerLine ")-1;
    if (UTIL1_ScanDecimal8uNumber(&p, &value8)==ERR_OK && value8>=1 && value8<=MAX_NOF_BYTES_PER_LINE) {
      (void)I2CSPY1_SetBytesPerLine(value8);
    } else {
      CLS1_SendStr((unsigned char*)"**** wrong bytes per line, must be 1..32\r\n", io->stdErr);
    }
    *handled = TRUE;
  } else if (UTIL1_strncmp((char*)cmd, "I2CSPY1 device ", sizeof("I2CSPY1 device ")-1)==0) {
    p = cmd+sizeof("I2CSPY1 device ")-1;
    if (UTIL1_ScanHex8uNumber(&p, &addr8)==ERR_OK && addr8<=0x7F) {
      (void)I2CSPY1_SetDeviceAddress(addr8);
    } else {
      CLS1_SendStr((unsigned char*)"**** wrong device address\r\n", io->stdErr);
    }
    *handled = TRUE;
  } else if (UTIL1_strncmp((char*)cmd, "I2CSPY1 read ", sizeof("I2CSPY1 read ")-1)==0) {
    p = cmd+sizeof("I2CSPY1 read ")-1;
    if (UTIL1_ScanHex32uNumber(&p, &addr32)==ERR_OK) {
      (void)Read(addr32, io);
    } else {
      CLS1_SendStr((unsigned char*)"**** wrong address\r\n", io->stdErr);
    }
    *handled = TRUE;
  } else if (UTIL1_strncmp((char*)cmd, "I2CSPY1 addrSize ", sizeof("I2CSPY1 addrSize ")-1)==0) {
    p = cmd+sizeof("I2CSPY1 addrSize ")-1;
    if (UTIL1_ScanDecimal8uNumber(&p, &value8)==ERR_OK && value8>=1 && value8<=4) {
      (void)I2CSPY1_SetAddressSize(value8);
    } else {
      CLS1_SendStr((unsigned char*)"**** addrSize of range (1-4)\r\n", io->stdErr);
    }
    *handled = TRUE;
  } else if (UTIL1_strncmp((char*)cmd, "I2CSPY1 write ", sizeof("I2CSPY1 write ")-1)==0) {
    p = cmd+sizeof("I2CSPY1 write ")-1;
    if (UTIL1_ScanHex32uNumber(&p, &addr32)==ERR_OK) {
      for(cnt=0; cnt<sizeof(buf) && *p!='\0'; cnt++) { /* breaks */
        if (UTIL1_ScanHex8uNumber(&p, &buf[cnt])!=ERR_OK) {
          CLS1_SendStr((unsigned char*)"**** wrong data format\r\n", io->stdErr);
          cnt = 0;
          break;
        }
      } /* for */
      if (cnt>=1) {
        (void)I2CSPY1_WriteRegData(addr32, &buf[0], cnt);
      }
    } /* if */
    *handled = TRUE;
  } else if (UTIL1_strncmp((char*)cmd, "I2CSPY1 dump ", sizeof("I2CSPY1 dump ")-1)==0) {
    p = cmd+sizeof("I2CSPY1 dump ")-1;
    if (UTIL1_ScanHex32uNumber(&p, &addr32)==ERR_OK) {
      if (UTIL1_strncmp((char*)p, "..", sizeof("..")-1)==0) {
        p = p+sizeof("..")-1;
        if (UTIL1_ScanHex32uNumber(&p, &end32)==ERR_OK) {
          (void)Dump(addr32, end32, io);
        } else {
          CLS1_SendStr((unsigned char*)"**** wrong end address\r\n", io->stdErr);
        }
      } else {
        CLS1_SendStr((unsigned char*)"**** wrong format, '..' expected.\r\n", io->stdErr);
      }
    } else {
      CLS1_SendStr((unsigned char*)"**** wrong start address\r\n", io->stdErr);
    }
    *handled = TRUE;
  } else if (UTIL1_strcmp((char*)cmd, "I2CSPY1 scan")==0) {
    ScanDevices(io);
    *handled = TRUE;
  }
  return ERR_OK;
}

/*
** ===================================================================
**     Method      :  I2CSPY1_SetDeviceAddress (component I2CSpy)
**     Description :
**         Sets the I2C Device address to be used
**     Parameters  :
**         NAME            - DESCRIPTION
**         addr            - I2C 7bit address
**     Returns     :
**         ---             - Error code
** ===================================================================
*/
uint8_t I2CSPY1_SetDeviceAddress(uint8_t addr)
{
  I2CSPY1_deviceData.deviceAddr = addr;
  return ERR_OK;
}

/*
** ===================================================================
**     Method      :  I2CSPY1_GetDeviceAddress (component I2CSpy)
**     Description :
**         Returns the current I2C Device address used
**     Parameters  : None
**     Returns     :
**         ---             - 7bit Device Address
** ===================================================================
*/
uint8_t I2CSPY1_GetDeviceAddress(void)
{
  return I2CSPY1_deviceData.deviceAddr;
}

/*
** ===================================================================
**     Method      :  I2CSPY1_SetAddressSize (component I2CSpy)
**     Description :
**         Specifies the address size of the I2C device memory
**     Parameters  :
**         NAME            - DESCRIPTION
**         size            - Either 1 (8bit), 2 (16bit), 3 (24bit),
**                           or 4 (32bit)
**     Returns     :
**         ---             - Error code
** ===================================================================
*/
uint8_t I2CSPY1_SetAddressSize(uint8_t size)
{
  I2CSPY1_deviceData.addrSize = size;
  return ERR_OK;
}

/*
** ===================================================================
**     Method      :  I2CSPY1_SetBytesPerLine (component I2CSpy)
**     Description :
**         Sets the number of bytes per line for the Dump shell command
**     Parameters  :
**         NAME            - DESCRIPTION
**         nofBytesPerLine - Number of bytes
**                           per line
**     Returns     :
**         ---             - Error code
** ===================================================================
*/
uint8_t I2CSPY1_SetBytesPerLine(uint8_t nofBytesPerLine)
{
  I2CSPY1_deviceData.bytesPerLine = nofBytesPerLine;
  return ERR_OK;
}

/*
** ===================================================================
**     Method      :  I2CSPY1_Init (component I2CSpy)
**     Description :
**         Initialization method
**     Parameters  : None
**     Returns     :
**         ---             - Error code
** ===================================================================
*/
uint8_t I2CSPY1_Init(void)
{
  if (I2CSPY1_SetDeviceAddress(I2CSPY1_DEFAULT_I2C_ADDR)!=ERR_OK) {
    return ERR_FAILED;
  }
  if (I2CSPY1_SetBytesPerLine(I2CSPY1_DEFAULT_BYTES_PER_LINE)!=ERR_OK) {
    return ERR_FAILED;
  }
  if (I2CSPY1_SetAddressSize(I2CSPY1_DEFAULT_ADDR_SIZE)!=ERR_OK) {
    return ERR_FAILED;
  }
  return ERR_OK;
}

/*
** ===================================================================
**     Method      :  I2CSPY1_Deinit (component I2CSpy)
**     Description :
**         Deinitialization method
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void I2CSPY1_Deinit(void)
{
  /* nothing to do */
}

/*
** ===================================================================
**     Method      :  I2CSPY1_OnRequestBus (component I2CSpy)
**
**     Description :
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/
void GI2C1_OnRequestBus(void)
{
  /* Write your code here ... */
}

/*
** ===================================================================
**     Method      :  I2CSPY1_OnReleaseBus (component I2CSpy)
**
**     Description :
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/
void GI2C1_OnReleaseBus(void)
{
  /* Write your code here ... */
}

/*
** ===================================================================
**     Method      :  I2CSPY1_ReadRegData (component I2CSpy)
**     Description :
**         Reads data starting from a register address from the current
**         device
**     Parameters  :
**         NAME            - DESCRIPTION
**         addr            - Device address
**       * data            - Pointer to data buffer where read data
**                           is stored
**         dataSize        - size of data buffer in bytes
**     Returns     :
**         ---             - Error code
** ===================================================================
*/
uint8_t I2CSPY1_ReadRegData(uint32_t addr, uint8_t *data, uint16_t dataSize)
{
  uint8_t addrBuf[4];

  addrBuf[3] = (uint8_t)(addr&0xff);
  addrBuf[2] = (uint8_t)((addr&0xff00)>>8);
  addrBuf[1] = (uint8_t)((addr&0xff0000)>>16);
  addrBuf[0] = (uint8_t)((addr&0xff000000)>>24);
  return GI2C1_ReadAddress(I2CSPY1_deviceData.deviceAddr, &addrBuf[4-I2CSPY1_deviceData.addrSize],
     I2CSPY1_deviceData.addrSize, data, dataSize);
}

/*
** ===================================================================
**     Method      :  I2CSPY1_WriteRegData (component I2CSpy)
**     Description :
**         Writes data starting from a register address to the current
**         device
**     Parameters  :
**         NAME            - DESCRIPTION
**         addr            - Address in device
**       * data            - Pointer to data to write
**         dataSize        - Size of data in bytes
**     Returns     :
**         ---             - Error code
** ===================================================================
*/
uint8_t I2CSPY1_WriteRegData(uint32_t addr, uint8_t *data, uint16_t dataSize)
{
  uint8_t addrBuf[4];

  addrBuf[3] = (uint8_t)(addr&0xff);
  addrBuf[2] = (uint8_t)((addr&0xff00)>>8);
  addrBuf[1] = (uint8_t)((addr&0xff0000)>>16);
  addrBuf[0] = (uint8_t)((addr&0xff000000)>>24);
  return GI2C1_WriteAddress(I2CSPY1_deviceData.deviceAddr, &addrBuf[4-I2CSPY1_deviceData.addrSize],
     I2CSPY1_deviceData.addrSize, data, dataSize);
}

/* END I2CSPY1. */

/*!
** @}
*/
/*
** ###################################################################
**
**     This file was created by Processor Expert 10.5 [05.21]
**     for the Freescale Kinetis series of microcontrollers.
**
** ###################################################################
*/
