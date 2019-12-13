#include "serialcomm.h"


CSerialComm::CSerialComm(){}
CSerialComm::~CSerialComm(){}


int CSerialComm::connect(char* portNum)
{
	if(!serial.OpenPort(portNum))
		return RETURN_FAIL;

	serial.ConfigurePort(CBR_115200, 8, FALSE, NOPARITY, ONESTOPBIT); 
	serial.SetCommunicationTimeouts(0, 0, 0, 0, 0); 

	return RETURN_SUCCESS;
}


int CSerialComm::sendCommand(unsigned char pos)
{
	if(serial.WriteByte(pos))
		return RETURN_SUCCESS;
	else
		return RETURN_FAIL;
}

void CSerialComm::disconnect()
{
	serial.ClosePort();
}