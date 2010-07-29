#include <iostream>

#include "tcp_ip/serversock.hpp"
#include "IAPBoard.hpp"

IAPBoard::IAPBoard(const RS232config & config) :
    connected(false),
    serial_interface(STD_TR1::shared_ptr< RS232 >(new sctl_ctb(config)))
{
    /* opens/initializes the serial device */
    serial_interface->open();
}

IAPBoard::~IAPBoard()
{
 if(connected)
     disconnect();
}

//activate the Steppercontrolboard
void IAPBoard::connect()
{
     std::cout << "board: connecting to sctlbrd" << std::endl;

     /* connection commands */

     connected = true;

}


//shut down the Steppercontrolboard to prevent overheating of the steppermotors
void IAPBoard::disconnect()
{
     std::cout << "board: disconnecting from sctlbrd" << std::endl;

     /* disconnection commands */

     connected = false;

}


void IAPBoard::send_command(ServerSock &sock, const std::string & cmd)
{
    char buffer[1024];
 
    serial_interface->rslog(cmd, "#>");

    while(serial_interface->send(cmd.c_str(), cmd.length()) <= 0 ) {
	sleep(1);
    }

    while(serial_interface->receive(buffer, 1024) <= 0 ) {
	sleep(1);
    }

    sock << buffer;

}


void IAPBoard::test(ServerSock &sock)
{
    char buffer[1024];
 
    serial_interface->rslog(" serial test", "#>");

    while(serial_interface->receive(buffer, 1024) <= 0 ) {
	sleep(1);
    }
    sock << buffer;

}

void IAPBoard::reset()
{
 std::cout << "board: reset" << std::endl;

}
