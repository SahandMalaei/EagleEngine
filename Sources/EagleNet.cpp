#include "EagleNet.h"

#include "Timer.h"

static ProjectEagle::Timer timer;

namespace ProjectEagle
{
	bool ENetInitialized;

	// Global Functions

	int ENetInitialize()
	{
		if(ENetInitialized)
			return 0;

		ENetInitialized = 1;

		return ENetInitializeWinsock();
	}

	int ENetInitializeWinsock()
	{
		WORD versionRequested;
		WSADATA wsaData;
		DWORD bufferSize = 0;

		LPWSAPROTOCOL_INFO selectedProtocol;
		int numProtocols;

		versionRequested = MAKEWORD(2, 0);
		int error = WSAStartup(versionRequested, &wsaData);
		if(error)
		{
			return 1;
		}
		else
		{
			if(LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 0)
			{
				WSACleanup();

				return 1;
			}
		}

		numProtocols = WSAEnumProtocols(0, 0, &bufferSize);
		if((numProtocols != SOCKET_ERROR) && (WSAGetLastError() != WSAENOBUFS))
		{
			WSACleanup();

			return 1;
		}

		selectedProtocol = (LPWSAPROTOCOL_INFO)malloc(bufferSize);
		if(!selectedProtocol)
		{
			WSACleanup();

			return 1;
		}

		int *protocols = (int *)calloc(2, sizeof(int));
		protocols[0] = IPPROTO_TCP;
		protocols[1] = IPPROTO_UDP;

		numProtocols = WSAEnumProtocols(protocols, selectedProtocol, &bufferSize);

		free(protocols);
		protocols = 0;

		free(selectedProtocol);
		selectedProtocol = 0;

		if(numProtocols == SOCKET_ERROR)
		{
			WSACleanup();

			return 1;
		}

		return 0;
	}

	void ENetShutdown()
	{
		if(!ENetInitialized) return;
		ENetInitialized = 0;

		WSACleanup();
	}

	SOCKET ENetCreateSocket(int protocol)
	{
		int type;
		int usedProtocol;
		SOCKET netSocket;

		if(protocol == ENETPROTOCOL_TCP)
		{
			type = SOCK_STREAM;
			usedProtocol = IPPROTO_TCP;
		}
		else
		{
			type = SOCK_DGRAM;
			usedProtocol = IPPROTO_UDP;
		}

		if((netSocket = socket(AF_INET, type, usedProtocol)) == SOCKET_ERROR)
		{
			return ENETINVALID_SOCKET;
		}

		return netSocket;
	}

	int ENetSetNonBlocking(SOCKET socket, unsigned long mode)
	{
		unsigned long set = mode;
		return ioctlsocket(socket, FIONBIO, &set);
	}

	int ENetSetBroadcasting(SOCKET socket, int mode)
	{
		if(setsockopt(socket, SOL_SOCKET, SO_BROADCAST, (char *)&mode, sizeof(int)) == SOCKET_ERROR)
		{
			return ENETINVALID_SOCKET;
		}

		return 0;
	}

	int ENetStringToSockaddr(char *addressString, sockaddr *address)
	{
		char copy[128];

		memset(address, 0, sizeof(sockaddr));
		sockaddr_in *addressPointer = (sockaddr_in *)address;
		addressPointer->sin_family = AF_INET;
		addressPointer->sin_port = htons(0);

		strcpy(copy, addressString);
		if(copy[0] >= '0' && copy[0] <= '9')
		{
			*(int *)&addressPointer->sin_addr = inet_addr(copy);
			return 0;
		}
		else
		{
			return 1;
		}
	}

	SOCKET ENetOpenUDPSocket(char *networkInterface, int port)
	{
		SOCKET socket;
		sockaddr_in address;

		socket = ENetCreateSocket(ENETPROTOCOL_UDP);
		if(socket == ENETINVALID_SOCKET) return ENETINVALID_SOCKET;

		ENetSetNonBlocking(socket, 1);
		ENetSetBroadcasting(socket, 1);

		if(!networkInterface || !networkInterface[0] || !strcmp(networkInterface, "localHost"))
		{
			address.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
		}
		else
		{
			ENetStringToSockaddr(networkInterface, (sockaddr *)&address);
		}

		if(!port)
		{
			address.sin_port = 0;
		}
		else
		{
			address.sin_port = htons((short)port);
		}

		address.sin_family = AF_INET;

		bind(socket, (sockaddr *)&address, sizeof(address));

		/*if(bind(socket, (sockaddr *)&address, sizeof(address)) == ENETINVALID_SOCKET)
		{
			return ENETINVALID_SOCKET;
		}*/

		int addressLength = sizeof(address);
		getsockname(socket, (sockaddr *)&address, &addressLength);

		return socket;
	}

	void ENetCloseSocket(SOCKET socket)
	{
		closesocket(socket);
	}

	int ENetGetPacket(SOCKET socket, char *data, sockaddr *fromAddress)
	{
		int ret;
		sockaddr tempFromAddress;
		int fromAddressLength = sizeof(tempFromAddress);

		ret = recvfrom(socket, data, 1024, 0, (sockaddr *)&tempFromAddress, &fromAddressLength);
		
		if(fromAddress)
		{
			*(sockaddr *) fromAddress = tempFromAddress;
		}

		if(ret == ENETINVALID_SOCKET)
		{
			int err = WSAGetLastError();
			if(err == WSAEWOULDBLOCK) return ret;
			if(err == WSAEMSGSIZE) return ret;
			if(err == WSAECONNREFUSED) return ret;

			return ret;
		}

		return ret;
	}

	void ENetSendPacket(SOCKET socket, char *data, int length, sockaddr address)
	{
		int ret = sendto(socket, data, length, 0, &address, sizeof(address));
		if(ret == ENETINVALID_SOCKET)
		{
			int err = WSAGetLastError();
			if(err == WSAEWOULDBLOCK) return;
			if(err == WSAEMSGSIZE) return;
			if(err == WSAECONNREFUSED) return;

			return;
		}
	}

	void ENetBroadcast(SOCKET socket, char *data, int length, int port)
	{
		sockaddr_in serverAddress;
		int len;

		unsigned long inetAddr = inet_addr("255.255.255.255");

		memset(&serverAddress, 0, sizeof(sockaddr_in));
		serverAddress.sin_family = AF_INET;
		serverAddress.sin_port = htons(port);
		serverAddress.sin_addr.S_un.S_addr = inetAddr;

		len = sizeof(serverAddress);

		int ret = sendto(socket, data, length, 0, (sockaddr *)&serverAddress, len);
		if(ret == ENETINVALID_SOCKET)
		{
			int err = WSAGetLastError();
			if(err == WSAEWOULDBLOCK) return;
			if(err == WSAEMSGSIZE) return;
			if(err == WSAECONNREFUSED) return;

			return;
		}
	}

	int ENetGetCurrentSystemTime()
	{
		int currentTime;
		static int base;
		static bool initialized = 0;

		if(!initialized)
		{
			base = (DWORD)timer.getPassedTimeMilliseconds() & 0xffff0000;
			initialized = 1;
		}

		currentTime = timer.getPassedTimeMilliseconds() - base;

		return currentTime;
	}

	// ENetMessage

	void ENetMessage::initialize(char *data, int length)
	{
		this->data = data;
		maxSize = length;
		size = 0;
		readCount = 0;
		overflow = 0;
	}

	void ENetMessage::clear()
	{
		size = 0;
		readCount = 0;
		overflow = 0;
	}

	char *ENetMessage::getNewPoint(int length)
	{
		char *tempData;

		if(size + length > maxSize)
		{
			clear();
			overflow = 1;
		}

		tempData = data + size;
		size += length;

		return tempData;
	}

	void ENetMessage::addSequences(ENetClient *client)
	{
		writeShort(client->getOutgoingSequence());
		writeShort(client->getIncomingSequence());
	}

	void ENetMessage::write(void *data, int length)
	{
		memcpy(getNewPoint(length), data, length);
	}

	void ENetMessage::writeByte(char c)
	{
		char *buffer = getNewPoint(1);
		memcpy(buffer, &c, 1);
	}

	void ENetMessage::writeShort(short s)
	{
		char *buffer = getNewPoint(2);
		memcpy(buffer, &s, 2);
	}

	void ENetMessage::writeLong(long l)
	{
		char *buffer = getNewPoint(4);
		memcpy(buffer, &l, 4);
	}

	void ENetMessage::writeFloat(float f)
	{
		char *buffer = getNewPoint(4);
		memcpy(buffer, &f, 4);
	}

	void ENetMessage::writeString(char *s)
	{
		if(!s) return;

		write(s, strlen(s) + 1);
	}

	void ENetMessage::beginRead()
	{
		readCount = 0;
	}

	void ENetMessage::beginRead(int size)
	{
		this->size = size;
		readCount = 0;
	}

	char *ENetMessage::read(int size)
	{
		static char c[2048];

		if(readCount + size > this->size) return 0;
		else memcpy(&c, &data[readCount], size);

		readCount += size;

		return c;
	}

	char ENetMessage::readByte()
	{
		char c;

		if(readCount + 1 > size) c = -1;
		else memcpy(&c, &data[readCount], 1);

		readCount ++;

		return c;
	}

	short ENetMessage::readShort()
	{
		short s;

		if(readCount + 2 > size) s = -1;
		else memcpy(&s, &data[readCount], 2);

		readCount += 2;

		return s;
	}

	long ENetMessage::readLong()
	{
		long l;

		if(readCount + 4 > size) l = -1;
		else memcpy(&l, &data[readCount], 4);

		readCount += 4;

		return l;
	}

	float ENetMessage::readFloat()
	{
		float f;

		if(readCount + 4 > size) f = -1;
		else memcpy(&f, &data[readCount], 4);

		readCount += 4;

		return f;
	}

	char *ENetMessage::readString()
	{
		static char string[2048];
		int l = 0;
		char c;

		do
		{
			c = readByte();

			if(c == -1 || c == 0) break;

			string[l] = c;
			l++;
		} while(l < sizeof(string) - 1);

		string[l] = 0;

		return string;
	}

	// ENetClient

	ENetClient::ENetClient()
	{
		connectionState = ENETSTATE_DISCONNECTED;

		outgoingSequence = 1;
		incomingSequence = 0;
		incomingAcknowledged = 0;
		droppedPackets = 0;

		initialized = 0;

		serverPort = 0;

		pingSent = 0;
		ping = 0;

		lastMessageTime = 0;

		next = 0;
	}

	ENetClient::~ENetClient()
	{
		ENetCloseSocket(socket);
	}

	int ENetClient::initialize(char *localIP, char *remoteIP, int port)
	{
		ENetInitialize();

		serverPort = port;
		strcpy(serverIP, remoteIP);

		socket = ENetOpenUDPSocket(localIP, 0);

		unsigned long inetAddr = inet_addr(serverIP);
		if(inetAddr == INADDR_NONE)
		{
			return ENETCLIENT_ERROR;
		}

		if(socket == ENETINVALID_SOCKET)
		{
			return ENETCLIENT_ERROR;
		}

		initialized = 1;

		return 0;
	}

	void ENetClient::shutdown()
	{
		ENetCloseSocket(socket);

		reset();

		initialized = 0;
	}

	void ENetClient::reset()
	{
		connectionState = ENETSTATE_DISCONNECTED;

		outgoingSequence = 1;
		incomingSequence = 0;
		incomingAcknowledged = 0;
		droppedPackets = 0;

		initialized = 0;

		serverPort = 0;

		pingSent = 0;
		ping = 0;

		lastMessageTime = 0;

		next = 0;
	}

	void ENetClient::dumpBuffer()
	{
		char data[1024];
		int ret;

		while((ret = ENetGetPacket(socket, data, 0)) > 0)
		{
		}
	}
	
	void ENetClient::sendConnect(char *name)
	{
		dumpBuffer();
		
		connectionState = ENETSTATE_CONNECTING;

		message.initialize(message.outgoingData, sizeof(message.outgoingData));
		message.writeByte(ENETMESSAGE_CONNECT);
		message.writeString(name);

		sendPacket(&message);
	}

	void ENetClient::sendDisconnect()
	{
		message.initialize(message.outgoingData, sizeof(message.outgoingData));
		message.writeByte(ENETMESSAGE_DISCONNECT);

		sendPacket(&message);
		reset();

		connectionState = ENETSTATE_DISCONNECTING;
	}

	void ENetClient::sendPing()
	{
		pingSent = ENetGetCurrentSystemTime();

		message.initialize(message.outgoingData, sizeof(message.outgoingData));
		message.writeByte(ENETMESSAGE_PING);

		sendPacket(&message);
	}

	void ENetClient::parsePacket(ENetMessage *message)
	{
		message->beginRead();
		int type = message->readByte();

		if(type > 0)
		{
			unsigned short sequence = message->readShort();
			unsigned short sequenceAcknowledged = message->readShort();

			if(sequence <= incomingSequence)
			{
			}

			droppedPackets = sequence - incomingSequence + 1;

			incomingSequence = sequence;
			incomingAcknowledged = sequenceAcknowledged;
		}

		switch(type)
		{
		case ENETMESSAGE_CONNECT:
			connectionState = ENETSTATE_CONNECTED;

			break;

		case ENETMESSAGE_DISCONNECT:
			connectionState = ENETSTATE_DISCONNECTED;

			break;

		case ENETMESSAGE_ADDCLIENT:

			break;

		case ENETMESSAGE_REMOVECLIENT:

			break;

		case ENETMESSAGE_PING:
			sendPing();

			break;
		}
	}

	int ENetClient::getPacket(char *data, sockaddr *fromAddress)
	{
		if(!socket) return 0;

		int ret;

		ENetMessage mes;
		mes.initialize(data, sizeof(data));

		ret = ENetGetPacket(socket, mes.data, fromAddress);
		if(ret <= 0) return 0;

		mes.setSize(ret);

		parsePacket(&mes);

		return ret;
	}

	void ENetClient::sendPacket()
	{
		if(!socket || connectionState == ENETSTATE_DISCONNECTED)
		{
			return;
		}

		if(message.getOverflow())
		{
			return;
		}

		if(serverPort) // Client-side
		{
			sockaddr_in sendAddress;
			memset((char *)&sendAddress, 0, sizeof(sendAddress));

			unsigned long inetAddr = inet_addr(serverIP);
			sendAddress.sin_port = htons((unsigned short)serverPort);
			sendAddress.sin_family = AF_INET;
			sendAddress.sin_addr.S_un.S_addr = inetAddr;

			ENetSendPacket(socket, message.data, message.getSize(), *(sockaddr *)&sendAddress);
		}
		else // Server-side
		{
			ENetSendPacket(socket, message.data, message.getSize(), address);
		}

		message.beginRead();

		int type = message.readByte();
		if(type > 0)
		{
			outgoingSequence++;
		}
	}

	void ENetClient::sendPacket(ENetMessage *message)
	{
		if(!socket || connectionState == ENETSTATE_DISCONNECTED)
		{
			return;
		}

		if(message->getOverflow())
		{
			return;
		}

		if(serverPort) // Client-side
		{
			sockaddr_in sendAddress;
			memset((char *)&sendAddress, 0, sizeof(sendAddress));

			unsigned long inetAddr = inet_addr(serverIP);
			sendAddress.sin_port = htons((unsigned short)serverPort);
			sendAddress.sin_family = AF_INET;
			sendAddress.sin_addr.S_un.S_addr = inetAddr;

			ENetSendPacket(socket, message->data, message->getSize(), *(sockaddr *)&sendAddress);
		}
		else // Server-side
		{
			ENetSendPacket(socket, message->data, message->getSize(), address);
		}

		message->beginRead();

		int type = message->readByte();
		if(type > 0)
		{
			outgoingSequence++;
		}
	}

	// ENetServer

	ENetServer::ENetServer()
	{
		initialized = 0;

		port = 0;
		runningIndex = 1;
		socket = 0;
		clientList = 0;
	}

	ENetServer::~ENetServer()
	{
		ENetClient *list = clientList;
		ENetClient *next;

		while(list)
		{
			next = list->next;

			if(list)
			{
				free(list);
			}

			list = next;
		}

		clientList = 0;

		ENetCloseSocket(socket);
	}

	int ENetServer::initialize(char *localIP, int serverPort)
	{
		ENetInitialize();

		port = serverPort;

		socket = ENetOpenUDPSocket(localIP, port);
		if(socket == ENETINVALID_SOCKET)
		{
			return ENETSERVER_ERROR;
		}

		initialized = 1;

		return 0;
	}

	void ENetServer::shutdown()
	{
		ENetCloseSocket(socket);

		initialized = 0;
	}

	void ENetServer::sendAddClient(ENetClient *client)
	{
		client->message.initialize(client->message.outgoingData, sizeof(client->message.outgoingData));
		client->message.writeByte(ENETMESSAGE_CONNECT);
		client->sendPacket();

		ENetClient *cli = clientList;

		// Inform the new client of the other clients

		for(; cli; cli = cli->next)
		{
			client->message.initialize(client->message.outgoingData, sizeof(client->message.outgoingData));
			client->message.writeByte(ENETMESSAGE_ADDCLIENT);

			if(cli == client) // Local client
			{
				client->message.writeByte(1);
				client->message.writeByte(cli->getIndex());
				client->message.writeString(cli->getName());
			}
			else // Remote client
			{
				client->message.writeByte(0);
				client->message.writeByte(cli->getIndex());
				client->message.writeString(cli->getName());
			}

			client->sendPacket();
		}

		// Inform the other clients of the new client

		for(cli = clientList; cli; cli = cli->next)
		{
			if(cli == client) continue;

			cli->message.initialize(cli->message.outgoingData, sizeof(cli->message.outgoingData));
			cli->message.writeByte(ENETMESSAGE_ADDCLIENT);
			cli->message.writeByte(0);
			cli->message.writeByte(client->getIndex());
			cli->message.writeString(client->getName());

			cli->sendPacket();
		}
	}

	void ENetServer::sendRemoveClient(ENetClient *client)
	{
		int index = client->getIndex();

		// Send ENETMESSAGE_REMOVECLIENT to every client

		ENetClient *list = clientList;

		for(; list; list = list->next)
		{
			list->message.initialize(list->message.outgoingData, sizeof(list->message.outgoingData));
			list->message.writeByte(ENETMESSAGE_REMOVECLIENT);
			list->message.writeByte(index);
		}

		sendPackets();

		client->message.initialize(list->message.outgoingData, sizeof(list->message.outgoingData));
		client->message.writeByte(ENETMESSAGE_REMOVECLIENT);
		client->sendPacket();
	}

	void ENetServer::sendPing()
	{
		/// Send ping to every client
		ENetClient *list = clientList;
		for(; list; list = list->next)
		{
			list->sendPing();
		}
	}

	void ENetServer::addClient(sockaddr *address, char *name)
	{
		ENetClient *list = clientList;
		ENetClient *prev;
		ENetClient *newClient;

		if(!clientList) // The first client
		{
			clientList = (ENetClient *)calloc(1, sizeof(ENetClient));

			clientList->setSocket(socket);
			clientList->setSocketAddress(address);

			clientList->setConnectionState(ENETSTATE_CONNECTING);
			clientList->setOutgoingSequence(1);
			clientList->setIncomingSequence(0);
			clientList->setIncomingAcknowledged(0);
			clientList->setIndex(runningIndex);
			clientList->setName(name);
			clientList->next = 0;

			newClient = clientList;
		}
		else // Not the first client
		{
			prev = list;
			list = clientList->next;

			while(list)
			{
				prev = list;
				list = list->next;
			}

			list = (ENetClient *)calloc(1, sizeof(ENetClient));

			list->setSocket(socket);
			list->setSocketAddress(address);

			list->setConnectionState(ENETSTATE_CONNECTING);
			list->setOutgoingSequence(1);
			list->setIncomingSequence(0);
			list->setIncomingAcknowledged(0);
			list->setIndex(runningIndex);
			list->setName(name);
			list->next = 0;

			newClient = list;
		}

		runningIndex++;

		sendAddClient(newClient);
	}

	void ENetServer::removeClient(ENetClient *client)
	{
		ENetClient *list = 0;
		ENetClient *prev = 0;
		ENetClient *next = 0;

		int index = client->getIndex();

		sendRemoveClient(client);

		for(list = clientList; list; list = list->next)
		{
			if(client == list)
			{
				if(prev)
				{
					prev->next = client->next;
				}

				break;
			}

			prev = list;
		}

		if(client == clientList)
		{
			if(list) next = list->next;
			if(client) free(client);
			client = 0;
			clientList = next;
		}
		else
		{
			if(list) next = list->next;
			if(client) free(client);
			client = next;
		}
	}

	void ENetServer::parsePacket(ENetMessage *message, sockaddr *address)
	{
		message->beginRead();
		int type = message->readByte();

		ENetClient *cList = clientList;

		if(cList)
		{
			for(; cList; cList = cList->next)
			{
				if(memcmp(cList->getSocketAddress(), address, sizeof(address)))
				{
					break;
				}
			}

			if(cList)
			{
				cList->setLastMessageTime(ENetGetCurrentSystemTime());

				if(type > 0)
				{
					unsigned short sequence = message->readShort();
					unsigned short sequenceAcknowledged = message->readShort();

					if(sequence <= cList->getIncomingSequence())
					{
					}

					cList->setDroppedPackets(sequence - cList->getIncomingSequence() - 1);

					cList->setIncomingSequence(sequence);
					cList->setIncomingAcknowledged(sequenceAcknowledged);
				}

				if(cList->getConnectionState() == ENETSTATE_CONNECTING) cList->setConnectionState(ENETSTATE_CONNECTED);
			}
		}

		switch(type)
		{
		case ENETMESSAGE_CONNECT:
			addClient(address, message->readString());

			break;

		case ENETMESSAGE_DISCONNECT:
			if(!cList) break;

			removeClient(cList);

			break;

		case ENETMESSAGE_PING:
			cList->setPing(ENetGetCurrentSystemTime() - cList->getPingSent());

			break;
		}
	}

	int ENetServer::checkForTimeout(char *data, sockaddr *fromAddress)
	{
		int currentTime = ENetGetCurrentSystemTime();

		ENetClient *cList = clientList;
		ENetClient *next;

		for(; cList; )
		{
			next = cList->next;

			if(cList->getConnectionState() == ENETSTATE_CONNECTING)
			{
				cList = next;

				continue;
			}

			if(currentTime - cList->getLastMessageTime() > ENETCLIENT_TIMEOUT)
			{
				ENetMessage message;
				message.initialize(data, sizeof(data));
				message.writeByte(ENETMESSAGE_DISCONNECT);

				*(sockaddr *) fromAddress = *cList->getSocketAddress();

				removeClient(cList);

				return message.getSize();
			}

			cList = next;
		}

		return 0;
	}

	int ENetServer::getPacket(char *data, sockaddr *fromAddress)
	{
		if(!socket) return 0;

		int timeout = checkForTimeout(data, fromAddress);
		if(timeout) return timeout;

		int max = socket;
		fd_set allSet;
		timeval waitTime;

		waitTime.tv_sec = 10 / 1000;
		waitTime.tv_usec = (10 % 1000) * 1000;

		FD_ZERO(&allSet);
		FD_SET(socket, &allSet);

		fd_set reading = allSet;

		int ready = select(max + 1, &reading, 0, 0, &waitTime);
		if(!ready) return 0;

		int ret = 0;

		ENetMessage message;
		message.initialize(data, sizeof(data));

		ret = ENetGetPacket(socket, message.data, fromAddress);
		if(ret <= 0) return 0;

		message.setSize(ret);

		parsePacket(&message, fromAddress);

		return ret;
	}

	void ENetServer::sendPackets()
	{
		if(!socket) return;

		ENetClient *cList = clientList;
		for(; cList; cList = cList->next)
		{
			if(!cList->message.getSize()) continue;

			cList->sendPacket();
		}
	}
};