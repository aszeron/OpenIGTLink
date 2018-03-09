/*=========================================================================

  Program:   OpenIGTLink -- Example for Tracker Client Program
  Language:  C++

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include <iostream>
#include <math.h>
#include <cstdlib>

#include "igtlOSUtil.h"
#include "igtlPointMessage.h"
#include "igtlClientSocket.h"

int ReceivePoint(igtl::Socket * socket, igtl::MessageHeader * header);

int main(int argc, char* argv[])
{
  //------------------------------------------------------------
  // Parse Arguments

  /*if (argc != 3) // check number of arguments
    {
    // If not correct, print usage
    std::cerr << "Usage: " << argv[0] << " <hostname> <port> <fps>"    << std::endl;
    std::cerr << "    <hostname> : IP or host name"                    << std::endl;
    std::cerr << "    <port>     : Port # (18944 in Slicer default)"   << std::endl;
    exit(0);
    }

  char*  hostname = argv[1];
  int    port     = atoi(argv[2]);*/

	 char*  hostname	= "127.0.0.1";
	 int	port		= 1280;			

  //------------------------------------------------------------
  // Establish Connection

  igtl::ClientSocket::Pointer socket;
  socket = igtl::ClientSocket::New();
  int r = socket->ConnectToServer(hostname, port);

  if (r != 0)
    {
    std::cerr << "Cannot connect to the server." << std::endl;
    exit(0);
    }

  //------------------------------------------------------------
  // Allocate Transform Message Class

  igtl::PointMessage::Pointer pointMsg;
  pointMsg = igtl::PointMessage::New();
  pointMsg->SetDeviceName("PointSender");

  //---------------------------
  // Create 1st point
  igtl::PointElement::Pointer point0;
  point0 = igtl::PointElement::New();
  point0->SetName("POINT_0");
  point0->SetGroupName("GROUP_0");
  point0->SetRGBA(0xFF, 0x00, 0x00, 0xFF);
  point0->SetPosition(10.0, 20.0, 30.0);
  point0->SetRadius(15.0);
  point0->SetOwner("IMAGE_0");
  
  //---------------------------
  // Create 2nd point
  igtl::PointElement::Pointer point1;
  point1 = igtl::PointElement::New();
  point1->SetName("POINT_1");
  point1->SetGroupName("GROUP_0");
  point1->SetRGBA(0x00, 0xFF, 0x00, 0xFF);
  point1->SetPosition(40.0, 50.0, 60.0);
  point1->SetRadius(45.0);
  point1->SetOwner("IMAGE_0");
  
  //---------------------------
  // Create 3rd point
  igtl::PointElement::Pointer point2;
  point2 = igtl::PointElement::New();
  point2->SetName("POINT_2");
  point2->SetGroupName("GROUP_0");
  point2->SetRGBA(0x00, 0x00, 0xFF, 0xFF);
  point2->SetPosition(70.0, 80.0, 90.0);
  point2->SetRadius(75.0);
  point2->SetOwner("IMAGE_0");
  
  //---------------------------
  // Pack into the point message
  pointMsg->AddPointElement(point0);
  pointMsg->AddPointElement(point1);
  pointMsg->AddPointElement(point2);
  pointMsg->Pack();
  
  //------------------------------------------------------------
  // Send
  socket->Send(pointMsg->GetPackPointer(), pointMsg->GetPackSize());
  
  
while (1)
  {
    //------------------------------------------------------------
    // Waiting for Connection
    
      // Create a message buffer to receive header
      igtl::MessageHeader::Pointer headerMsg;
      headerMsg = igtl::MessageHeader::New();

      //------------------------------------------------------------
      // loop
      for (int i = 0; i < 100; i ++)
        {
        // Initialize receive buffer
        headerMsg->InitPack();

        // Receive generic header from the socket
        int r = socket->Receive(headerMsg->GetPackPointer(), headerMsg->GetPackSize());
        if (r == 0)
          {
          socket->CloseSocket();
          }
        if (r != headerMsg->GetPackSize())
          {
          continue;
          }

        // Deserialize the header
        headerMsg->Unpack();
        

	 if (strcmp(headerMsg->GetDeviceType(), "POINT") == 0)
     {
		 ReceivePoint(socket, headerMsg);
	 }
 }
	
  //------------------------------------------------------------
  // Close the socket
  socket->CloseSocket();
  }

}

int ReceivePoint(igtl::Socket * socket, igtl::MessageHeader * header)
{

  std::cerr << "Receiving POINT data type." << std::endl;

  // Create a message buffer to receive transform data
  igtl::PointMessage::Pointer pointMsg;
  pointMsg = igtl::PointMessage::New();
  pointMsg->SetMessageHeader(header);
  pointMsg->AllocatePack();

  // Receive transform data from the socket
  socket->Receive(pointMsg->GetPackBodyPointer(), pointMsg->GetPackBodySize());

  // Deserialize the transform data
  // If you want to skip CRC check, call Unpack() without argument.
  int c = pointMsg->Unpack(1);

  if (c & igtl::MessageHeader::UNPACK_BODY) // if CRC check is OK
    {
    int nElements = pointMsg->GetNumberOfPointElement();
    for (int i = 0; i < nElements; i ++)
      {
      igtl::PointElement::Pointer pointElement;
      pointMsg->GetPointElement(i, pointElement);

      igtlUint8 rgba[4];
      pointElement->GetRGBA(rgba);

      igtlFloat32 pos[3];
      pointElement->GetPosition(pos);

      std::cerr << "========== Element #" << i << " ==========" << std::endl;
      std::cerr << " Name      : " << pointElement->GetName() << std::endl;
      std::cerr << " GroupName : " << pointElement->GetGroupName() << std::endl;
      std::cerr << " RGBA      : ( " << (int)rgba[0] << ", " << (int)rgba[1] << ", " << (int)rgba[2] << ", " << (int)rgba[3] << " )" << std::endl;
      std::cerr << " Position  : ( " << std::fixed << pos[0] << ", " << pos[1] << ", " << pos[2] << " )" << std::endl;
      std::cerr << " Radius    : " << std::fixed << pointElement->GetRadius() << std::endl;
      std::cerr << " Owner     : " << pointElement->GetOwner() << std::endl;
      std::cerr << "================================" << std::endl;
      }
    }

  return 1;
}

