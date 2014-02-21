#include <fastcgi++/manager.hpp>


Fastcgipp::ManagerPar* Fastcgipp::ManagerPar::instance=0;

Fastcgipp::ManagerPar::ManagerPar(int fd, const boost::function<void(Protocol::FullId, Message)>& sendMessage_, bool doSetupSignals): transceiver(fd, sendMessage_), asleep(false), stopBool(false), terminateBool(false)
{
	if(doSetupSignals) setupSignals();
	instance=this;
}

void Fastcgipp::ManagerPar::terminate()
{
	boost::lock_guard<boost::mutex> terminateLock(terminateMutex);
	boost::lock_guard<boost::mutex> sleepLock(sleepMutex);
	terminateBool=true;
	if(asleep)
	{
		transceiver.wake();
		asleep=false;
	}
}

void Fastcgipp::ManagerPar::stop()
{
	boost::lock_guard<boost::mutex> stopLock(stopMutex);
	boost::lock_guard<boost::mutex> sleepLock(sleepMutex);
	stopBool=true;
	if(asleep)
	{
		transceiver.wake();
		asleep=false;
	}
}

void Fastcgipp::ManagerPar::signalHandler(int signum)
{
	switch(signum)
	{
		case SIGUSR1:
		{
			if(instance) instance->terminate();
			break;
		}
		case SIGTERM:
		{
			if(instance) instance->stop();
			break;
		}
	}
}

void Fastcgipp::ManagerPar::setupSignals()
{
	struct sigaction sigAction;
	sigAction.sa_handler=Fastcgipp::ManagerPar::signalHandler;
	sigemptyset(&sigAction.sa_mask);
	sigAction.sa_flags=0;

	sigaction(SIGPIPE, &sigAction, NULL);
	sigaction(SIGUSR1, &sigAction, NULL);
	sigaction(SIGTERM, &sigAction, NULL);
}

void Fastcgipp::ManagerPar::localHandler(Protocol::FullId id)
{
	using namespace std;
	using namespace Protocol;
	Message message(messages.front());
	messages.pop();
	
	if(!message.type)
	{
		const Header& header=*(Header*)message.data.get(); 
		switch(header.getType())
		{
			case GET_VALUES:
			{
				size_t nameSize;
				size_t valueSize;
				const char* name;
				const char* value;
				processParamHeader(message.data.get()+sizeof(Header), header.getContentLength(), name, nameSize, value, valueSize);
				if(nameSize==14 && !memcmp(name, "FCGI_MAX_CONNS", 14))
				{
					Block buffer(transceiver.requestWrite(sizeof(maxConnsReply)));
					memcpy(buffer.data, (const char*)&maxConnsReply, sizeof(maxConnsReply));
					transceiver.secureWrite(sizeof(maxConnsReply), id, false);
				}
				else if(nameSize==13 && !memcmp(name, "FCGI_MAX_REQS", 13))
				{
					Block buffer(transceiver.requestWrite(sizeof(maxReqsReply)));
					memcpy(buffer.data, (const char*)&maxReqsReply, sizeof(maxReqsReply));
					transceiver.secureWrite(sizeof(maxReqsReply), id, false);
				}
				else if(nameSize==15 && !memcmp(name, "FCGI_MPXS_CONNS", 15))
				{
					Block buffer(transceiver.requestWrite(sizeof(mpxsConnsReply)));
					memcpy(buffer.data, (const char*)&mpxsConnsReply, sizeof(mpxsConnsReply));
					transceiver.secureWrite(sizeof(mpxsConnsReply), id, false);
				}

				break;
			}

			default:
			{
				Block buffer(transceiver.requestWrite(sizeof(Header)+sizeof(UnknownType)));

				Header& sendHeader=*(Header*)buffer.data;
				sendHeader.setVersion(Protocol::version);
				sendHeader.setType(UNKNOWN_TYPE);
				sendHeader.setRequestId(0);
				sendHeader.setContentLength(sizeof(UnknownType));
				sendHeader.setPaddingLength(0);

				UnknownType& sendBody=*(UnknownType*)(buffer.data+sizeof(Header));
				sendBody.setType(header.getType());

				transceiver.secureWrite(sizeof(Header)+sizeof(UnknownType), id, false);

				break;
			}
		}
	}
}
