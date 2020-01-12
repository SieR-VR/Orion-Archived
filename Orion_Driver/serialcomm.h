#include "serialport.h"

#define RETURN_SUCCESS 1
#define RETURN_FAIL 0


class CSerialComm
{
public :
	CSerialComm();
	~CSerialComm();

	CSerialPort		serial;
	int				connect(char* _portNum);
	int				sendCommand(unsigned char pos);
	void			disconnect();
};
