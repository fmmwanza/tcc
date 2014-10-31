/** @file
Simple Bratrack UDP Class to receive data 
Author Jeronimo Grandi
modified from Franscisco Pinto
*/

#ifndef _BRATRACK_UDP
#define _BRATRACK_UDP

#include <vector>
#include <cstring>
#include <exception>
#include <stdlib.h>
#include <stdio.h>
#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>

namespace BraTrackUDP{

	class type_artifact{
	public:
		static const unsigned int transform_size = 12;
		std::string id; ///< the id of the artifact.
		double transform[transform_size]; ///< the pose of the artifact: four 3D vectors (Rx, Ry, Rz, T).
	};


	class classInputProtocol{

		struct sockaddr_in myaddr;	/* our address */
		struct sockaddr_in remaddr;	/* remote address */
		socklen_t addrlen = sizeof(remaddr);		/* length of addresses */
		int recvlen;			/* # bytes received */
		int fd;				/* our socket */

		void *buffer;  ///< a buffer for receiving sequences of bytes representing data about frames from BraTrack.
		unsigned int buffer_size;  ///< the size of the buffer, i. e., the number of bytes.

		void read_frame_contents(unsigned long long int & time_stamp, std::vector<type_artifact> & list_of_artifacts, unsigned int package_size);  ///< reads the contents of the buffer used to receive frame data and extracts information about the frame.

	public:

		classInputProtocol();  ///< basic constructor.
		classInputProtocol(unsigned short int port_number, unsigned int buffsize);  		
		~classInputProtocol();  ///< destructor.

		int init(unsigned short int port_number, unsigned int buffsize);
		void set_package_limits(unsigned int max_number_of_artifacts);  ///< defines the expected maximum number of points in frames to be received.
		bool receive_frame_not_blocking(unsigned long long int & time_stamp, std::vector<type_artifact> & list_of_artifacts);  ///< receives a frame not waiting for its arrival.


		class InputProtocolException : public std::exception{};

		class InvalidFrameException : public InputProtocolException{
		public:
			const char *what() const throw();
		};

	};

}

#endif