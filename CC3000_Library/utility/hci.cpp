

//*****************************************************************************
//
//! \addtogroup hci_app
//! @{
//
//*****************************************************************************

#include "cc3000_common.h"
#include "hci.h"
#include "../ccspi.h"
#include "evnt_handler.h"
#include "wlan.h"

#define SL_PATCH_PORTION_SIZE		(1000)


//*****************************************************************************
//
//!  hci_command_send
//!
//!  @param  usOpcode     command operation code
//!  @param  pucBuff      pointer to the command's arguments buffer
//!  @param  ucArgsLength length of the arguments
//!
//!  @return              none
//!
//!  @brief               Initiate an HCI command.
//
//*****************************************************************************
unsigned short 
hci_command_send(unsigned short usOpcode, unsigned char *pucBuff,
                     unsigned char ucArgsLength)
{ 
	unsigned char *stream;
	
	stream = (pucBuff + SPI_HEADER_SIZE);
	
	UINT8_TO_STREAM(stream, HCI_TYPE_CMND);
	stream = UINT16_TO_STREAM(stream, usOpcode);
	UINT8_TO_STREAM(stream, ucArgsLength);
	
	//Update the opcode of the event we will be waiting for
	SpiWrite(pucBuff, ucArgsLength + SIMPLE_LINK_HCI_CMND_HEADER_SIZE);
	
	return(0);
}

//*****************************************************************************
//
//!  hci_data_send
//!
//!  @param  usOpcode        command operation code
//!	 @param  ucArgs					 pointer to the command's arguments buffer
//!  @param  usArgsLength    length of the arguments
//!  @param  ucTail          pointer to the data buffer
//!  @param  usTailLength    buffer length
//!
//!  @return none
//!
//!  @brief              Initiate an HCI data write operation
//
//*****************************************************************************
long
hci_data_send(unsigned char ucOpcode, 
							unsigned char *ucArgs,
							unsigned short usArgsLength, 
							unsigned short usDataLength,
							const unsigned char *ucTail,
							unsigned short usTailLength)
{
	unsigned char *stream;
	
	stream = ((ucArgs) + SPI_HEADER_SIZE);
	
	UINT8_TO_STREAM(stream, HCI_TYPE_DATA);
	UINT8_TO_STREAM(stream, ucOpcode);
	UINT8_TO_STREAM(stream, usArgsLength);
	stream = UINT16_TO_STREAM(stream, usArgsLength + usDataLength + usTailLength);
	
	// Send the packet over the SPI
	SpiWrite(ucArgs, SIMPLE_LINK_HCI_DATA_HEADER_SIZE + usArgsLength + usDataLength + usTailLength);
	
	return(ESUCCESS);
}


//*****************************************************************************
//
//!  hci_data_command_send
//!
//!  @param  usOpcode      command operation code
//!  @param  pucBuff       pointer to the data buffer
//!  @param  ucArgsLength  arguments length
//!  @param  ucDataLength  data length
//!
//!  @return none
//!
//!  @brief              Prepeare HCI header and initiate an HCI data write operation
//
//*****************************************************************************
void hci_data_command_send(unsigned short usOpcode, unsigned char *pucBuff,
                     unsigned char ucArgsLength,unsigned short ucDataLength)
{ 
 	unsigned char *stream = (pucBuff + SPI_HEADER_SIZE);
	
	UINT8_TO_STREAM(stream, HCI_TYPE_DATA);
	UINT8_TO_STREAM(stream, usOpcode);
	UINT8_TO_STREAM(stream, ucArgsLength);
	stream = UINT16_TO_STREAM(stream, ucArgsLength + ucDataLength);
	
	// Send the command over SPI on data channel
	SpiWrite(pucBuff, ucArgsLength + ucDataLength + SIMPLE_LINK_HCI_DATA_CMND_HEADER_SIZE);
	
	return;
}

//*****************************************************************************
//
//!  hci_patch_send
//!
//!  @param  usOpcode      command operation code
//!  @param  pucBuff       pointer to the command's arguments buffer
//!  @param  patch         pointer to patch content buffer 
//!  @param  usDataLength  data length
//!
//!  @return              none
//!
//!  @brief               Prepeare HCI header and initiate an HCI patch write operation
//
//*****************************************************************************
void
hci_patch_send(unsigned char ucOpcode, unsigned char *pucBuff, char *patch, unsigned short usDataLength)
{ 
 	unsigned char *data_ptr = (pucBuff + SPI_HEADER_SIZE);
	unsigned short usTransLength;
	unsigned char *stream = (pucBuff + SPI_HEADER_SIZE);
	
	UINT8_TO_STREAM(stream, HCI_TYPE_PATCH);
	UINT8_TO_STREAM(stream, ucOpcode);
	stream = UINT16_TO_STREAM(stream, usDataLength + SIMPLE_LINK_HCI_PATCH_HEADER_SIZE);
	
	if (usDataLength <= SL_PATCH_PORTION_SIZE)
	{
		UINT16_TO_STREAM(stream, usDataLength);
		stream = UINT16_TO_STREAM(stream, usDataLength);
		memcpy((pucBuff + SPI_HEADER_SIZE) + HCI_PATCH_HEADER_SIZE, patch, usDataLength);
		
		// Update the opcode of the event we will be waiting for
		SpiWrite(pucBuff, usDataLength + HCI_PATCH_HEADER_SIZE);
	}
	else
	{
		
		usTransLength = (usDataLength/SL_PATCH_PORTION_SIZE);
		UINT16_TO_STREAM(stream, usDataLength + SIMPLE_LINK_HCI_PATCH_HEADER_SIZE + usTransLength*SIMPLE_LINK_HCI_PATCH_HEADER_SIZE);
		stream = UINT16_TO_STREAM(stream, SL_PATCH_PORTION_SIZE);
		memcpy(pucBuff + SPI_HEADER_SIZE + HCI_PATCH_HEADER_SIZE, patch, SL_PATCH_PORTION_SIZE);
		usDataLength -= SL_PATCH_PORTION_SIZE;
		patch += SL_PATCH_PORTION_SIZE;
		
		// Update the opcode of the event we will be waiting for
		SpiWrite(pucBuff, SL_PATCH_PORTION_SIZE + HCI_PATCH_HEADER_SIZE);
		
		while (usDataLength)
		{
			cc3k_int_poll();

			if (usDataLength <= SL_PATCH_PORTION_SIZE)
			{
				usTransLength = usDataLength;
				usDataLength = 0;
				
			}
			else
			{
				usTransLength = SL_PATCH_PORTION_SIZE;
				usDataLength -= usTransLength;
			}
			
			*(unsigned short *)data_ptr = usTransLength;
			memcpy(data_ptr + SIMPLE_LINK_HCI_PATCH_HEADER_SIZE, patch, usTransLength);
			patch += usTransLength;
			
			// Update the opcode of the event we will be waiting for
			SpiWrite((unsigned char *)data_ptr, usTransLength + sizeof(usTransLength));
		}
	}
}

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//
//*****************************************************************************
