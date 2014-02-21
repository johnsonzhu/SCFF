//! \file transceiver.cpp Defines member functions for Fastcgipp::Transceiver
/***************************************************************************
* Copyright (C) 2007 Eddie Carle [eddie@erctech.org]                       *
*                                                                          *
* This file is part of fastcgi++.                                          *
*                                                                          *
* fastcgi++ is free software: you can redistribute it and/or modify it     *
* under the terms of the GNU Lesser General Public License as  published   *
* by the Free Software Foundation, either version 3 of the License, or (at *
* your option) any later version.                                          *
*                                                                          *
* fastcgi++ is distributed in the hope that it will be useful, but WITHOUT *
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or    *
* FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public     *
* License for more details.                                                *
*                                                                          *
* You should have received a copy of the GNU Lesser General Public License *
* along with fastcgi++.  If not, see <http://www.gnu.org/licenses/>.       *
****************************************************************************/


#include <fastcgi++/transceiver.hpp>

int Fastcgipp::Transceiver::transmit()
{
	while(1)
	{{
		Buffer::SendBlock sendBlock(buffer.requestRead());
		if(sendBlock.size)
		{
			ssize_t sent = write(sendBlock.fd, sendBlock.data, sendBlock.size);
			if(sent<0)
			{
				if(errno==EPIPE || errno==EBADF)
				{
					freeFd(sendBlock.fd);
					sent=sendBlock.size;
				}
				else if(errno!=EAGAIN) throw Exceptions::SocketWrite(sendBlock.fd, errno);
			}

			buffer.freeRead(sent);
			if(sent!=(ssize_t)sendBlock.size)
				break;
		}
		else
			break;
	}}

	return buffer.empty();
}

void Fastcgipp::Transceiver::Buffer::secureWrite(size_t size, Protocol::FullId id, bool kill)
{
	writeIt->end+=size;
	if(minBlockSize>(writeIt->data.get()+Chunk::size-writeIt->end) && ++writeIt==chunks.end())
	{
		chunks.push_back(Chunk());
		--writeIt;
	}
	frames.push(Frame(size, kill, id));
}

bool Fastcgipp::Transceiver::handler()
{
	using namespace std;
	using namespace Protocol;

	bool transmitEmpty=transmit();

	int retVal=poll(&pollFds.front(), pollFds.size(), 0);
	if(retVal==0)
	{
		if(transmitEmpty) return true;
		else return false;
	}
	if(retVal<0) throw Exceptions::SocketPoll(errno);
	
	std::vector<pollfd>::iterator pollFd = find_if(pollFds.begin(), pollFds.end(), reventsZero);

	if(pollFd->revents & (POLLHUP|POLLERR|POLLNVAL) )
	{
		fdBuffers.erase(pollFd->fd);
		pollFds.erase(pollFd);
		return false;
	}
	
	int fd=pollFd->fd;
	if(fd==socket)
	{
		sockaddr_un addr;
		socklen_t addrlen=sizeof(sockaddr_un);
		fd=accept(fd, (sockaddr*)&addr, &addrlen);
		fcntl(fd, F_SETFL, (fcntl(fd, F_GETFL)|O_NONBLOCK)^O_NONBLOCK);
		
		pollFds.push_back(pollfd());
		pollFds.back().fd = fd;
		pollFds.back().events = POLLIN|POLLHUP|POLLERR|POLLNVAL;

		Message& messageBuffer=fdBuffers[fd].messageBuffer;
		messageBuffer.size=0;
		messageBuffer.type=0;
	}
	else if(fd==wakeUpFdIn)
	{
		char x;
		read(wakeUpFdIn, &x, 1);
		return false;
	}
	
	Message& messageBuffer=fdBuffers[fd].messageBuffer;
	Header& headerBuffer=fdBuffers[fd].headerBuffer;

	ssize_t actual;
	// Are we in the process of recieving some part of a frame?
	if(!messageBuffer.data)
	{
		// Are we recieving a partial header or new?
		actual=read(fd, (char*)&headerBuffer+messageBuffer.size, sizeof(Header)-messageBuffer.size);
		if(actual<0 && errno!=EAGAIN) throw Exceptions::SocketRead(fd, errno);
		if(actual>0) messageBuffer.size+=actual;
		
		if( actual == 0 )
		{
			fdBuffers.erase( pollFd->fd );
			pollFds.erase( pollFd );
			return false;
		}

		if(messageBuffer.size!=sizeof(Header))
		{
			if(transmitEmpty) return true;
			else return false;
		}

		messageBuffer.data.reset(new char[sizeof(Header)+headerBuffer.getContentLength()+headerBuffer.getPaddingLength()]);
		memcpy(static_cast<void*>(messageBuffer.data.get()), static_cast<const void*>(&headerBuffer), sizeof(Header));
	}

	const Header& header=*(const Header*)messageBuffer.data.get();
	size_t needed=header.getContentLength()+header.getPaddingLength()+sizeof(Header)-messageBuffer.size;
	actual=read(fd, messageBuffer.data.get()+messageBuffer.size, needed);
	if(actual<0 && errno!=EAGAIN) throw Exceptions::SocketRead(fd, errno);
	if(actual>0) messageBuffer.size+=actual;

	// Did we recieve a full frame?
	if(actual==(ssize_t)needed)
	{		
		sendMessage(FullId(headerBuffer.getRequestId(), fd), messageBuffer);
		messageBuffer.size=0;
		messageBuffer.data.reset();
		return false;
	}
	if(transmitEmpty) return true;
	else return false;
}

void Fastcgipp::Transceiver::Buffer::freeRead(size_t size)
{
	pRead+=size;
	if(pRead>=chunks.begin()->end)
	{
		if(writeIt==chunks.begin())
		{
			pRead=writeIt->data.get();
			writeIt->end=pRead;
		}
		else
		{
			if(writeIt==--chunks.end())
			{
				chunks.begin()->end=chunks.begin()->data.get();
				chunks.splice(chunks.end(), chunks, chunks.begin());
			}
			else
				chunks.pop_front();
			pRead=chunks.begin()->data.get();
		}
	}
	if((frames.front().size-=size)==0)
	{
		if(frames.front().closeFd)
			freeFd(frames.front().id.fd);
		frames.pop();
	}

}

void Fastcgipp::Transceiver::wake()
{
	char x=0;
	write(wakeUpFdOut, &x, 1);
}

Fastcgipp::Transceiver::Transceiver(int fd_, boost::function<void(Protocol::FullId, Message)> sendMessage_)
:buffer(pollFds, fdBuffers), sendMessage(sendMessage_), pollFds(2), socket(fd_)
{
	socket=fd_;
	
	// Let's setup a in/out socket for waking up poll()
	int socPair[2];
	socketpair(AF_UNIX, SOCK_STREAM, 0, socPair);
	wakeUpFdIn=socPair[0];
	fcntl(wakeUpFdIn, F_SETFL, (fcntl(wakeUpFdIn, F_GETFL)|O_NONBLOCK)^O_NONBLOCK);	
	wakeUpFdOut=socPair[1];	
	
	fcntl(socket, F_SETFL, (fcntl(socket, F_GETFL)|O_NONBLOCK)^O_NONBLOCK);
	pollFds[0].events = POLLIN|POLLHUP;
	pollFds[0].fd = socket;
	pollFds[1].events = POLLIN|POLLHUP;
	pollFds[1].fd = wakeUpFdIn;
}

Fastcgipp::Exceptions::SocketWrite::SocketWrite(int fd_, int erno_): Socket(fd_, erno_)
{
	switch(errno)
	{
		case EAGAIN:
			msg = "The file descriptor has been marked non-blocking (O_NONBLOCK) and the write would block.";
			break;

		case EBADF:
			msg = "The file descriptor is not a valid file descriptor or is not open for writing.";
			break;

		case EFAULT:
			msg = "The buffer is outside your accessible address space.";
			break;

		case EFBIG:
			msg = "An attempt was made to write a file that exceeds the implementation-defined maximum file size or the process’s file size limit, or to write at a position past the maximum allowed offset.";
			break;

		case EINTR:
			msg = "The call was interrupted by a signal before any data was written; see signal(7).";
			break;

		case EINVAL:
			msg = "The file descriptor is attached to an object which is unsuitable for writing; or the file was opened with the O_DIRECT flag, and either the address specified for the buffer, the value specified in count, or the current file offset is not suitably aligned.";
			break;

		case EIO:
			msg = "A low-level I/O error occurred while modifying the inode.";
			break;

		case ENOSPC:
			msg = "The device containing the file referred to by the file descriptor has no room for the data.";
			break;

		case EPIPE:
			msg = "The file descriptor is connected to a pipe or socket whose reading end is closed.  When this happens the writing process will also receive a SIGPIPE signal.  (Thus, the write return value is seen only if the program catches, blocks or ignores this signal.)";
			break;
	}
}

Fastcgipp::Exceptions::SocketRead::SocketRead(int fd_, int erno_): Socket(fd_, erno_)
{
	switch(errno)
	{
		case EAGAIN:
			msg = "Non-blocking I/O has been selected using O_NONBLOCK and no data was immediately available for reading.";
			break;

		case EBADF:
			msg = "The file descriptor is not valid or is not open for reading.";
			break;

		case EFAULT:
			msg = "The buffer is outside your accessible address space.";
			break;

		case EINTR:
			msg = "The call was interrupted by a signal before any data was written; see signal(7).";
			break;

		case EINVAL:
			msg = "The file descriptor is attached to an object which is unsuitable for reading; or the file was opened with the O_DIRECT flag, and either the address specified in buf, the value specified in count, or the current file offset is not suitably aligned.";
			break;

		case EIO:
			msg = "I/O error.  This will happen for example when the process is in a background process group, tries to read from its controlling tty, and either it is ignoring or blocking SIGTTIN or its process group is orphaned.  It may also occur when there is a low-level I/O error while reading from a disk or tape.";
			break;

		case EISDIR:
			msg = "The file descriptor refers to a directory.";
			break;
	}
}

Fastcgipp::Exceptions::SocketPoll::SocketPoll(int erno_): CodedException(0, erno_)
{
	switch(errno)
	{
		case EBADF:
			msg = "An invalid file descriptor was given in one of the sets.";
			break;

		case EFAULT:
			msg = "The array given as argument was not contained in the calling program’s address space.";
			break;

		case EINTR:
			msg = "A signal occurred before any requested event; see signal(7).";
			break;

		case EINVAL:
			msg = "The nfds value exceeds the RLIMIT_NOFILE value.";
			break;

		case ENOMEM:
			msg = "There was no space to allocate file descriptor tables.";
			break;
	}
}

void Fastcgipp::Transceiver::freeFd(int fd, std::vector<pollfd>& pollFds, std::map<int, fdBuffer>& fdBuffers)
{
	std::vector<pollfd>::iterator it=std::find_if(pollFds.begin(), pollFds.end(), equalsFd(fd));
	if(it != pollFds.end())
	{
		pollFds.erase(it);
		close(fd);
		fdBuffers.erase(fd);
	}
}
