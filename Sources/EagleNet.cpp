#include "Eagle.h"

//namespace ProjectEagle
//{
	/*// NetworkHandle
	void NetworkHandle::Initialize(unsigned short localListeningPort, unsigned short foreignListeningPort)
	{
	cleanup();

	WSAData wsaData;
	int error;

	error = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if(error == SOCKET_ERROR)
	{
	char errorBuffer[100];

	error = WSAGetLastError();
	if(error == WSAVERNOTSUPPORTED)
	{
	sprintf(errorBuffer, "Network startup error : Winsock version 2.2 is needed");
	WSACleanup();
	}
	else
	{
	sprintf(errorBuffer, "Network startup error : %d", error);
	}

	eagle.error(errorBuffer);
	}

	localListenPort = localListeningPort;
	foreignListenPort = foreignListeningPort;
	bytesTransferred = 0;
	started = 1;
	}

	void NetworkHandle::cleanup()
	{
	if(!started) return;

	started = 0;

	stopListening();
	stopSending();

	WSACleanup();
	}

	void NetworkHandle::startListening()
	{
	if(!started || listening) return;

	listening = 1;

	listenSocket = socket(AF_INET, SOCK_DGRAM, 0);
	if(listenSocket == INVALISOCKET)
	{
	eagle.error("Network error : socket creation problem");
	}

	SOCKADDR_IN localAddress;
	int result;

	memset(&localAddress, 0, sizeof(SOCKADDR_IN));
	localAddress.sin_family = AF_INET;
	localAddress.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	localAddress.sin_port = htons(localListenPort);

	result = bind(listenSocket, (sockaddr *)&localAddress, sizeof(PSOCKADDR_IN));
	if(result == SOCKET_ERROR)
	{
	closesocket(listenSocket);
	eagle.error("Network error : socket creation problem");
	}
	}

	void NetworkHandle::startSending()
	{
	if(!started || sending) return;

	sending = 1;

	sendSocket = socket(AF_INET, SOCK_DGRAM, 0);
	if(listenSocket == INVALISOCKET)
	{
	eagle.error("Network error : socket creation problem");
	}

	SOCKADDR_IN localAddress;
	int result;

	memset(&localAddress, 0, sizeof(SOCKADDR_IN));
	localAddress.sin_family = AF_INET;
	localAddress.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	localAddress.sin_port = htons(0);

	result = bind(sendSocket, (sockaddr *)&localAddress, sizeof(PSOCKADDR_IN));
	if(result == SOCKET_ERROR)
	{
	closesocket(sendSocket);
	eagle.error("Network error : socket creation problem");
	}
	}

	DWORD NetworkHandle::threadFunction()
	{
	if(!listening) return 0;

	char inBuffer[4096];
	timeval waitTimeStr;
	SOCKADDR_IN fromAddress;
	int fromLength;
	unsigned short result;
	FSET set;

	while(listening)
	{
	FZERO(&set);
	FSET(listenSocket, &set);

	waitTimeStr.tv_sec = 0;
	waitTimeStr.tv_usec = 0;

	result = select(FSETSIZE, &set, 0, 0, &waitTimeStr);
	if(!result) continue;

	fromLength = sizeof(SOCKADDR);
	result = recvfrom(listenSocket, inBuffer, 4096, 0, (SOCKADDR *)&fromAddress, &fromLength);
	if(!result) continue;

	processIncomingData(inBuffer, result, ntohl(fromAddress.sin_addr.S_un.S_addr), GetTickCount());
	}

	return listening = 1;
	}

	void NetworkHandle::processIncomingData(char * const data, unsigned short length, DWORD address, DWORD recieveTime)
	{
	Host *host;
	HostList::iterator iter;

	threadMonitor.MutexOn();

	if(iter == hosts.end())
	{
	DWORD hostID;
	hostID = hostCreate(address, foreignListenPort);

	if(!hostID)
	{
	host = hostMap[hostID];
	}
	}
	else
	{
	host = (*iter);
	}

	host->processIncomingData(data, length, recieveTime);

	threadMonitor.MutexOff();
	}

	unsigned short NetworkHandle::GetReliableData(char *buffer, unsigned short maxLength, DWORD *hostID)
	{
	if(!maxLength || !buffer || !hostID) return 0;

	DataPacket *packet = 0;
	HostList::iterator iter;

	threadMonitor.MutexOn();

	for(iter = hosts.begin(); iter != hosts.end(); ++iter)
	{
	packet = (*iter)->getQueueIn().getPacket();
	if(packet) break;
	}

	threadMonitor.MutexOff();

	unsigned short length = 0;

	if(packet)
	{
	length = packet->length > maxLength ? maxLength : packet->length;
	memcpy(&buffer, packet->data, length);

	delete packet;

	*hostID = (*iter)->getAddress();
	}

	return length;
	}

	// QueueIn

	QueueIn::QueueIn() : ThreadMonitor()
	{
	currentPacketID = 0;
	count = 0;
	}

	QueueIn::~QueueIn()
	{
	clear();
	}

	void QueueIn::clear()
	{
	MutexOn();

	unorderedPackets.clear();
	orderedPackets.clear();

	MutexOff();
	}

	void QueueIn::addPacket(DWORD packetID, char *data, unsigned short length, DWORD receiveTime )
	{
	if( data == NULL ||
	length == 0 )
	return;

	if( currentPacketID > packetID )
	return;

	DataPacket *packet;

	packet = new DataPacket();

	packet->Initialize(receiveTime, packetID, length, data);

	MutexOn();

	if( currentPacketID == packet->ID)
	{
	orderedPackets.push_back(packet);
	currentPacketID++;
	count++;

	packet = *unorderedPackets.begin();

	while( unorderedPackets.empty() == 0 && currentPacketID == packet->ID)
	{
	unorderedPackets.pop_front();
	orderedPackets.push_back(packet);
	currentPacketID++;
	packet = *unorderedPackets.begin();
	}
	}
	else
	{
	list<DataPacket *>::iterator iter;
	bool bExists;

	bExists = 0;

	for( iter = unorderedPackets.begin(); iter != unorderedPackets.end(); ++iter )
	{
	if((*iter)->ID == packet->ID)
	{
	bExists = 1;
	break;
	}
	if((*iter)->ID > packet->ID)
	break;
	}

	if( bExists == 1 )
	delete packet;
	else
	{
	unorderedPackets.insert(iter, packet);
	count++;
	}
	}

	MutexOff();
	}


	DataPacket *QueueIn::getPacket()
	{
	if(orderedPackets.empty())
	return NULL;

	DataPacket *packet;

	MutexOn();

	packet = *orderedPackets.begin();
	orderedPackets.pop_front();

	MutexOff();

	return packet;
	}


	bool QueueIn::isUnorderedPacketQueued(DWORD packetID)
	{
	bool bIsQueued;
	list<DataPacket *>::iterator iter;

	bool isQueued = 0;

	MutexOn();

	for(iter = unorderedPackets.begin(); iter != unorderedPackets.end(); ++iter)
	{
	// Already in list - get out now!
	if( (*iter)->ID == packetID )
	{
	bIsQueued = 1;
	break;
	}
	}

	MutexOff();

	return bIsQueued;
	}


	DWORD QueueIn::getHighestID()
	{
	list<DataPacket *>::iterator iter;

	iter = unorderedPackets.end();
	if(iter == unorderedPackets.begin())
	return currentPacketID;
	else
	{
	iter--;
	return (*iter)->ID + 1;
	}
	}

	// Host

	void Host::setAddress(const char * const addr)
	{
	if(!addr) return;

	IN_ADDR *inAddr;
	HOSTENT *HE;
	HE = gethostbyname(addr);
	if(!HE) return;

	inAddr = (IN_ADDR *)HE->h_addr_list[0];
	address = ntohl(inAddr->S_un.S_addr);
	}

	unsigned short Host::processIncomingData(char * const buffer, unsigned short length, DWORD recieveTime)
	{
	DWORD packetID;
	unsigned short length;
	char * readPtr;

	readPtr = buffer;
	memcpy(&packetID, readPtr, sizeof(DWORD));
	readPtr += sizeof(DWORD);
	memcpy(&length, readPtr, sizeof(unsigned short));
	readPtr += sizeof(unsigned short);

	queueIn.addPacket(packetID, (char *)readPtr, length, recieveTime);
	readPtr += length;

	return readPtr - buffer;
	}

	// NetClock

	NetClock::NetClock() : ThreadMonitor()
	{
	Initialize();
	}

	NetClock::~NetClock()
	{

	}

	void NetClock::Initialize()
	{
	initialized = 0;
	ratio = 1.0;

	start.actual = GetTickCount();
	start.clock = 0;
	lastUpdate.actual = start.actual;
	lastUpdate.clock = start.clock;
	}


	void NetClock::Synchronize(DWORD serverTime, DWORD packetSendTime, DWORD packetACKTime, float ping)
	{
	MutexOn();

	DWORD dt;

	dt = packetACKTime - packetSendTime;

	if(dt > 10000)
	{
	MutexOff();
	return;
	}

	if(initialized)
	{
	// if the packet ACK time was too long OR the clock is close enough
	// then do not update the clock.
	if(abs((long)( serverTime + ( dt / 2 ) - GetTime() )) <= 5)
	{
	MutexOff();
	return;
	}

	lastUpdate.actual = packetACKTime;
	lastUpdate.clock = serverTime + (DWORD)( ping / 2 );
	ratio = (float)( lastUpdate.clock - start.clock) / (float)(lastUpdate.actual - start.actual);
	}
	else
	{
	lastUpdate.actual = packetACKTime;
	lastUpdate.clock = serverTime + ( dt / 2 );
	start.actual = lastUpdate.actual;
	start.clock = lastUpdate.clock;
	initialized = 1;
	}

	MutexOff();
	}


	DWORD NetClock::GetTime()
	{
	DWORD serverTime;

	MutexOn();

	serverTime = (DWORD)((float)(GetTickCount() - lastUpdate.actual)) + lastUpdate.clock;

	MutexOff();

	return serverTime;
	}


	DWORD NetClock::TranslateTime( DWORD localTime )
	{
	DWORD serverTime;

	MutexOn();

	serverTime = (DWORD)((float)(localTime - lastUpdate.actual)) + lastUpdate.clock;

	MutexOff();

	return serverTime;
	}

	NetworkClient::NetworkClient()
	{
	WSADATA wsaData;
	connectionSocket = INVALISOCKET;

	struct addrinfo *result = NULL, *ptr = NULL, hints;

	WSAStartup(MAKEWORD(2, 2), &wsaData);

	ZeroMemory(&hints, sizeof(hints));
	hints->sin_family = AF_UNSPEC;

	}*/

	/*CommunicationBase::CommunicationBase()
	{
	}

	CommunicationBase::~CommunicationBase()
	{
	}

	bool CommunicationBase::startWinsock()
	{
		char buffer[128];
		int error;

		winsockVersion = MAKEWORD(2, 0);
		error = WSAStartup(winsockVersion, &winsockData);
		if(error == SOCKET_ERROR)
		{
			if(error == WSAVERNOTSUPPORTED)
			{
				eagle.error("Winsock version 2.2 required", "Network Error");
				WSACleanup();
			}
			else
			{
				eagle.error("Winsock startup error", "Network Error");
			}

			return 1;
		}

		return 0;
	}

	bool CommunicationBase::endWinsock()
	{
		disconnect();
		WSACleanup();

		return 0;
	}

	HANDLE CommunicationBase::getHost(char *address, unsigned short port)
	{
		hWnd = eagle.getWindowHandle();

		addressInfo.sin_addr.S_un.S_addr = inet_addr(address);
		addressInfo.sin_port = htons(port);
		addressInfo.sin_family = AF_INET;

		if(addressInfo.sin_addr.S_un.S_addr == INADDR_NONE)
		{
			hostHandle = WSAAsyncGetHostByName(hWnd, SM_GETHOST, address, hostBuffer, MAXGETHOSTSTRUCT);
		}
		else
		{
			hostHandle = WSAAsyncGetHostByAddr(hWnd, SM_GETHOST, (const char *)&addressInfo.sin_addr.S_un.S_addr, sizeof(IN_ADDR), AF_INET, hostBuffer, MAXGETHOSTSTRUCT);
		}

		return hostHandle;
	}

	int CommunicationBase::onGetHost(WPARAM wParam, LPARAM lParam)
	{
		if((HANDLE)wParam != hostHandle) return 1;

		if(WSAGETASYNCERROR(lParam))
		{
			disconnect();
			return 2;
		}

		LPHOSTENT hostent;
		hostent = (LPHOSTENT)hostBuffer;
		strcpy(IP, inet_ntoa(*(LPIN_ADDR)hostent->h_addrtype));

		if(changeWindow(eagle.getWindowHandle()) == SOCKET_ERROR)
		{
			disconnect();
		}

		if(addressHost())
		{
			disconnect();
		}

		if(attemptToConnect())
		{
			disconnect();
		}

		return 0;
	}

	SOCKET CommunicationBase::createSocket()
	{
		if(netSocket) return netSocket;

		netSocket = socket(AF_INET, SOCK_STREAM, 0);

		return netSocket;
	}

	bool CommunicationBase::disconnect()
	{
		shutdown(netSocket, 0);

		closesocket(netSocket);

		return 0;
	}

	bool CommunicationBase::addressHost()
	{
		LPHOSTENT hostent;
		hostent = (LPHOSTENT)hostBuffer;

		if(isServer) addressInfo.sin_addr.S_un.S_addr = INADDR_ANY;
		else addressInfo.sin_addr = *(LPIN_ADDR)hostent->h_addrtype;

		return 0;
	}

	int CommunicationBase::changeWindow(HWND hWndd)
	{
		hWnd = hWndd;

		return WSAAsyncSelect(netSocket, hWnd, SM_ASYNC, socketFlags);
	}

	int CommunicationBase::attemptToConnect()
	{
		if(isServer)
		{
			if(bind(netSocket, (LPSOCKADDR)&addressInfo, sizeof(SOCKADDR)) == SOCKET_ERROR)
			{
				closesocket(netSocket);
				return 1;
			}

			if(listen(netSocket, SOMAXCONN) == SOCKET_ERROR)
			{
				closesocket(netSocket);
				return 2;
			}
		}
		else
		{
			return connect(netSocket, (LPSOCKADDR)&addressInfo, sizeof(SOCKADDR_IN));
		}
	}

	LRESULT CommunicationBase::onAsync(WPARAM wParam, LPARAM lParam)
	{
		int event, error;
		event = WSAGETSELECTEVENT(lParam);
		error = WSAGETSELECTERROR(lParam);

		switch(event)
		{
		case FD_ACCEPT:
			if(!isServer) break;

			SOCKET newSocket;
			PSOCKADDR_IN addr;
			int size;

			size = sizeof(SOCKADDR);
			newSocket = accept(netSocket, (LPSOCKADDR)&addr, &size);
			if(newSocket == INVALISOCKET)
			{
				return 1;
			}
			break;

		case FD_CONNECT:
			break;

		case FD_READ:
			break;

		case FD_WRITE:
			break;

		case FD_CLOSE:
			break;
		}
	}*/

	/*CommunicationBase::CommunicationBase()
	{
		initialize();
	}

	CommunicationBase::~CommunicationBase()
	{
		shutdownNetwork();
	}

	int CommunicationBase::initialize()
	{
		winsockVersion = MAKEWORD( WINSOCK_VERSIONMAJOR, WINSOCK_VERSIONMINOR );
		state = STATE_DISCONNECTED;
		serverPort = DEFAULT_SERVERPORT;
		strcpy(sessionName, "[ Session name will appear here ]");
		strcpy(serverName, "[ Server name will appear here ]");
		IP[0] = 0;
		netSocket = INVALID_SOCKET;
		hWnd = 0;

		return 0;
	}

	int CommunicationBase::startup(int runFlagg, HWND hwnd)
	{
		char  buffer[80];
		int   error;

		hWnd = hwnd;
		runFlag = runFlagg;
		socketFlags = FD_READ | FD_WRITE | FD_CONNECT | FD_CLOSE;

		if(runFlag) // Server
		{
#if !defined( UDP_PROTOCOL )
			socketFlags |= FD_ACCEPT;
#endif
			socketFlags |= FD_OOB;
		}

		error = WSAStartup(winsockVersion, &winsockData);
		if(error == SOCKET_ERROR)
		{
			if(error == WSAVERNOTSUPPORTED)
			{
				sprintf(buffer, "WSAStartup error.\nRequested Winsock v%d.%d, found v%d.%d.", WINSOCK_VERSIONMAJOR, WINSOCK_VERSIONMINOR, 
					LOBYTE(winsockData.wVersion), HIBYTE(winsockData.wVersion));
				eagle.error(buffer, "Network Error");
				WSACleanup();
			}
			else
			{
				sprintf(buffer, "WSAStartup error (%d)", WSAGetLastError());
				eagle.error(buffer, "Network Error");
			}

			return 1;
		}

		gethostname(serverName, sizeof(serverName));
		serverPort = DEFAULT_SERVERPORT;

		return 0;
	}

	int CommunicationBase::shutdownNetwork()
	{
		disconnect();
		WSACleanup();
		return 0;
	}

	HANDLE CommunicationBase::getHost(char *address, int port)
	{
		if(state != STATE_DISCONNECTED) return 0;

		state = STATE_CONNECTING;

		if(port) serverPort = port;
		if(address) strcpy(serverName, address);

		if(!runFlag) addressInfo.sin_addr.S_un.S_addr = inet_addr(serverName);
		else addressInfo.sin_addr.S_un.S_addr = INADDR_ANY;
		addressInfo.sin_port = htons(serverPort);
		addressInfo.sin_family = AF_INET;

		if(addressInfo.sin_addr.S_un.S_addr == INADDR_NONE)
		{
			hostHandle = WSAAsyncGetHostByName(hWnd, SM_GETHOST, serverName, hostBuffer, MAXGETHOSTSTRUCT);
		}
		else
		{
			hostHandle = WSAAsyncGetHostByAddr(hWnd, SM_GETHOST, (const char *)&addressInfo.sin_addr.s_addr, sizeof(IN_ADDR), AF_INET, hostBuffer, MAXGETHOSTSTRUCT);
		}

		return hostHandle;
	}

	int CommunicationBase::onGetHost(WPARAM wparam, LPARAM lparam)
	{
		if((HANDLE)wparam != hostHandle) return 1;
		if(WSAGETASYNCERROR(lparam))
		{
			disconnect();
			return 2;
		}

		if(createSocket() == INVALID_SOCKET)
		{
			disconnect();
			return 3;
		}

		if(changeWindow(hWnd) == SOCKET_ERROR)
		{
			disconnect();
			return 4;
		}

		if(addressHost())
		{
			disconnect();
			return 5;
		}

		return 0;
	}

	SOCKET CommunicationBase::createSocket()
	{
		if(netSocket != INVALID_SOCKET) return netSocket;  // already exists

#if defined(UDP_PROTOCOL)
		netSocket = socket(AF_INET, SOCK_DGRAM, 0);
#else
		netSocket = socket(AF_INET, SOCK_STREAM, 0);
#endif

		return netSocket;
	}

	int CommunicationBase::changeWindow(HWND hwnd)
	{
		hWnd = hwnd;

		return WSAAsyncSelect(netSocket, hWnd, SM_ASYNC, socketFlags);
	}

	int CommunicationBase::addressHost()
	{
		if(!runFlag) // Client
		{
			/*LPHOSTENT lphostent;

			lphostent = (LPHOSTENT)hostBuffer;
			strcpy(IP, inet_ntoa(*(LPIN_ADDR)lphostent->h_addr_list[0]));

			addressInfo.sin_addr = *((LPIN_ADDR)lphostent->h_addr_list[0]);

			WSAAsyncSelect(netSocket, eagle.getWindowHandle(), WM_SOCKET, FD_CONNECT | FD_READ | FD_WRITE | FD_CLOSE);

			attemptToConnect();
		}
		else // Server
		{
			//addressInfo.sin_addr.S_un.S_addr = INADDR_ANY;
			if(bind(netSocket, (sockaddr*)&addressInfo, sizeof(SOCKADDR)) == SOCKET_ERROR)
			{
				closesocket(netSocket);
				return 1;
			}

#if !defined(UDP_PROTOCOL)
			if(listen(netSocket, SOMAXCONN) == SOCKET_ERROR)
			{
				closesocket(netSocket);
				return 2;
			}
#endif
		}

		return 0;
	}

	int CommunicationBase::attemptToConnect() // Client
	{
		return connect(netSocket, (LPSOCKADDR)&addressInfo, sizeof(SOCKADDR_IN));
	}

	int CommunicationBase::setAsConnected(int errmsg)
	{
		state = STATE_CONNECTED;

		return 0;
	}

	int CommunicationBase::disconnect()
	{
		switch(state)
		{
		case STATE_CONNECTED:
			state = STATE_DISCONNECTING;
			return shutdown(netSocket, 0);

		case STATE_CONNECTING:
			closesocket(netSocket);
			netSocket = INVALID_SOCKET;
			state = STATE_DISCONNECTED;
			break;

		case STATE_DISCONNECTING:
			closesocket(netSocket);
			netSocket = INVALID_SOCKET;
			state = STATE_DISCONNECTED;
			break;
		default:
			break;
		}

		return 0;
	}

	int CommunicationBase::onAsync( WPARAM wparam, LPARAM lparam )
	{
		return 1;
	}

	NetworkServerBase::NetworkServerBase()
	{
		numClients = 0;

		for(int i = 0; i < 1024; ++i) clientSockets[i] = INVALID_SOCKET;
	}

	int NetworkServerBase::awaitConnection()
	{
		int size = sizeof(clientAddress[numClients]);
		clientSockets[numClients] = accept(netSocket, (sockaddr*)(&clientAddress[numClients]), &size);
		clientIdleTimer[numClients].reset();
		numClients++;
		//eagle.message("Connected");

		return 0;
	}

	int NetworkServerBase::sendData(char *data, unsigned int dataSize, unsigned int clientIndex)
	{
		if(!dataSize || clientIndex > numClients - 1) return 0;
		if(send(clientSockets[clientIndex], data, dataSize, 0) == SOCKET_ERROR) return 1;
		return 0;
	}

	int NetworkServerBase::recieveData(char *buffer, unsigned int maxLength, unsigned int clientIndex)
	{
		if(!maxLength || clientIndex > numClients) return 0;
		int len;
		if(len = recv(clientSockets[clientIndex], buffer, maxLength, 0) == SOCKET_ERROR) return 1;
		if(len > 0) clientIdleTimer[clientIndex].reset();
		return 0;
	}

	int NetworkClientBase::awaitConnection()
	{
		return attemptToConnect();
	}

	int NetworkClientBase::getRecievedData(char* buffer, unsigned int maxBufferLength)
	{
		//buffer = "";
		//memcpy(buffer, readBuffer, maxBufferLength);
		if(maxBufferLength == 0 || !canRead) return 0;
		canRead = 0;
		return recv(netSocket, buffer, maxBufferLength, 0);
	}

	int NetworkClientBase::sendData(char *data, unsigned int dataLength)
	{
		//memcpy(sendBuffer, data, dataLength);
		//sendLength = dataLength;
		//if(dataLength == 0 || !canWrite) return;
		if(dataLength == 0) return 0;
		canWrite = 0;
		return send(netSocket, data, dataLength, 0);
	}

	void NetworkClientBase::rd()
	{
		if(readLength == 0) return;
		recv(netSocket, readBuffer, readLength, 0);
		readLength = 0;
	}

	void NetworkClientBase::sd()
	{
		if(sendLength == 0) return;
		send(netSocket, sendBuffer, sendLength, 0);
		sendLength = 0;
		sendBuffer = "";
	}

	NetworkServerThread::NetworkServerThread()
	{
		updateTimer.reset();
		updateTick = 50;
	}

	NetworkServerThread::~NetworkServerThread()
	{
	}

	DWORD NetworkServerThread::threadFunction()
	{
		while(1)
		{
			if(!updateTimer.stopWatch(updateTick)) continue;
			updateTimer.reset();

			mutex.MutexOn();
			if(server->awaitConnection() == SOCKET_ERROR) eagle.error("S" + toString(WSAGetLastError()));
			mutex.MutexOff();
		}
	}

	void NetworkServerBase::removeInactiveClients()
	{
		bool d[1024];
		int num = numClients;
		for(int i = 0; i < numClients; ++i)
		{
			d[i] = 0;
			if(clientIdleTimer[i].stopWatch(IDLE_TIMEOUT))
			{
				d[i] = 1;
				num--;
			}
		}

		int i2 = 0;
		for(int i = 0; i < num; ++i)
		{
			while(d[i2])
			{
				i2++;
				if(i2 == numClients) break;
				eagle.message("while");
			}
			if(i2 == numClients) break;

			clientSockets[i] = clientSockets[i2];
			clientAddress[i] = clientAddress[i2];

			i2++;
		}

		numClients = num;
	}

	NetworkClientThread::NetworkClientThread()
	{
		updateTimer.reset();
		updateTick = 50;
	}

	NetworkClientThread::~NetworkClientThread()
	{
	}

	DWORD NetworkClientThread::threadFunction()
	{
		while(1)
		{
			if(!updateTimer.stopWatch(updateTick))
			{
				Sleep(1);
				continue;
			}
			updateTimer.reset();

			mutex.MutexOn();
			/*if(client->getState() != STATE_CONNECTED)
			{
				if(client->awaitConnection())
				{
					int err = WSAGetLastError();
					if(err == WSAEISCONN)
					{
						client->setAsConnected(0);
					}
					else if(err == WSAEWOULDBLOCK)
					{
						// Do nothing
					}
					else
					{
						eagle.error("Server Error : " + toString(WSAGetLastError()), "Network Error");
					}
				}
			}
			else
			{
				if(eagle.isNetworkMessageAvailable())
				{
					WindowsNetworkMessage wnm = eagle.getLastNetworkMessage();

					if(wnm.message == WM_SOCKET)
					{
						switch(wnm.lParam)
						{
						case FD_CONNECT:
							client->attemptToConnect();
							client->setAsConnected(0);
							eagle.message("Connect");

							break;

						case FD_READ:
							//client->rd();
							//eagle.message("Read");
							client->setCanRead(1);

							break;

						case FD_WRITE:
							//client->sd();
							//eagle.message("Write");
							client->setCanWrite(1);

							break;
						}
					}
				}
			}
			mutex.MutexOff();
		}
	}*/
//};

#include "EagleNet.h"

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
			base = (DWORD)eagle.getTimer()->getPassedTimeMilliseconds() & 0xffff0000;
			initialized = 1;
		}

		currentTime = eagle.getTimer()->getPassedTimeMilliseconds() - base;

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