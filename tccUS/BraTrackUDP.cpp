/** @file
**************************************************************************************************************************
BraTrackUDP.cpp
Francisco Pinto, 18/12/2007, creation of this file.
Modified by Jeronimo Grandi
**************************************************************************************************************************
*/
#include <iostream>

#include "BraTrackUDP.h"

using namespace std;

BraTrackUDP::classInputProtocol::classInputProtocol()
{
	buffer = NULL;
	buffer_size = 0;
}

BraTrackUDP::classInputProtocol::classInputProtocol(unsigned short int port_number, unsigned int buffsize)
{
	init(port_number,buffsize);
}

BraTrackUDP::classInputProtocol::~classInputProtocol()
{
	if(buffer != NULL)
		delete buffer;
}

int BraTrackUDP::classInputProtocol::init(unsigned short int port_number, unsigned int buffsize)
{

	if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
	perror("cannot create socket\n");
	return 1;
	}

	memset((char *)&myaddr, 0, sizeof(myaddr));
	myaddr.sin_family = AF_INET;
	myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	myaddr.sin_port = htons(port_number);

	if (bind(fd, (struct sockaddr *)&myaddr, sizeof(myaddr)) < 0) {
		perror("bind failed");
		return 1;
	}

	buffer_size = buffsize;
	return 0;

}

void BraTrackUDP::classInputProtocol::set_package_limits(unsigned int max_number_of_artifacts)
{
	if(buffer != NULL)
		delete buffer;

	buffer_size = max_number_of_artifacts * (20 + type_artifact::transform_size*sizeof(double));
	buffer = new char[buffer_size];
}

void BraTrackUDP::classInputProtocol::read_frame_contents(unsigned long long int & time_stamp, std::vector<type_artifact> & list_of_artifacts, unsigned int package_size)
{

	recvlen = recvfrom(fd, buffer, buffer_size, 0, (struct sockaddr *)&remaddr, &addrlen);

	void *cursor = buffer;
	unsigned int bytes_checked = 0;
	unsigned int number_of_artifacts = 0;

	bytes_checked += sizeof(unsigned long long int);
	if(package_size < bytes_checked)
		throw InvalidFrameException();
	if(*((unsigned long long int*)cursor) == 0)
		throw InvalidFrameException();
	cursor = (char*)cursor + sizeof(unsigned long long int);

	while(package_size > bytes_checked){
		while(*((char*)cursor) != 0){
			cursor = (char*)cursor + sizeof(char);
			bytes_checked++;
			if(bytes_checked >= package_size)
			{
				printf("A\n");
				throw InvalidFrameException();
			}
		}
		cursor = (char*)cursor + sizeof(char);
		bytes_checked++;

		bytes_checked += type_artifact::transform_size*sizeof(double);
		//if(package_size < bytes_checked)
		//{
		//	printf("B\n");
		//	throw InvalidFrameException();
		//}
		cursor = (char*)cursor + type_artifact::transform_size*sizeof(double);

		number_of_artifacts++;
	}

	list_of_artifacts.resize(number_of_artifacts);
	cursor = buffer;
	unsigned int artifact_counter = 0;
	//cout <<time_stamp<<endl;
	time_stamp = *((unsigned long long int*)cursor);
	cursor = (char*)cursor + sizeof(unsigned long long int);

	std::vector<type_artifact>::iterator it = list_of_artifacts.begin();
	for(unsigned int i=0; i<number_of_artifacts; i++, it++){
		it->id = (char*)cursor;
		cursor = (char*)cursor + (it->id.length()+1)*sizeof(char);
		for(unsigned int i=0; i<type_artifact::transform_size; i++){
			it->transform[i] = *((double*)cursor);
			cursor = (char*)cursor + sizeof(double);
		}
	}
}


bool BraTrackUDP::classInputProtocol::receive_frame_not_blocking(unsigned long long int & time_stamp, std::vector<type_artifact> & list_of_artifacts)
{
	unsigned int package_size = buffer_size;
	read_frame_contents(time_stamp,list_of_artifacts,package_size);
	return true;
}

const char *BraTrackUDP::classInputProtocol::InvalidFrameException::what() const throw()
{
	return "The received frame is invalid.";
}


