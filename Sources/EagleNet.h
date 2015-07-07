//namespace ProjectEagle
//{
	/*struct TimePair
	{
	DWORD actual, clock;
	};

	class NetClock : public ThreadMonitor
	{
	private:
	TimePair start, lastUpdate;
	bool initialized;
	float ratio;

	public:
	NetClock();
	virtual ~NetClock();

	void  Initialize();
	void  Synchronize(DWORD serverTime, DWORD packetSendTime, DWORD packetACKTime, float ping);
	DWORD GetTime();
	DWORD TranslateTime(DWORD time);
	};

	class DataPacket
	{
	public:
	DataPacket();
	virtual ~DataPacket();

	char data[4096];
	unsigned short length, timesSent;
	DWORD ID, firstTime, lastTime;

	void Initialize(DWORD time, DWORD id, unsigned short len, const char * const dataChar);

	DataPacket &operator = (const DataPacket packet);
	};

	class QueueIn : public ThreadMonitor
	{
	protected:
	list<DataPacket *> orderedPackets;
	list<DataPacket *> unorderedPackets;

	DWORD currentPacketID, count;

	public:
	QueueIn();
	~QueueIn();

	void clear();
	void addPacket(DWORD packetID, char *packetData, unsigned short len, DWORD recieveTime);
	DataPacket *getPacket();

	bool isUnorderedPacketQueued(DWORD packetID);
	DWORD getHighestID();

	inline DWORD getCurrentID(){return currentPacketID;}
	inline DWORD getCount(){return count;}
	};

	#define ACK_MAXPERMSG           256
	#define ACK_BUFFERLENGTH        48

	#define PING_RECORDLENGTH       64
	#define PING_DEFAULTVALLINK     150
	#define PING_DEFAULTVALTRANS    200


	class cHost : public cMonitor
	{
	DWORD               d_address;
	unsigned short      d_port;
	cQueueIn            d_inQueue;
	cQueueOut           d_outQueue;
	cUnreliableQueueIn  d_unreliableInQueue;
	cUnreliableQueueOut d_unreliableOutQueue;

	// Ping records
	DWORD d_pingLink[ PING_RECORDLENGTH ],
	d_pingTrans[ PING_RECORDLENGTH ];

	// For clock synchronization
	DWORD d_lastClockTime;
	bool  d_bClockTimeSet;

	// A buffer of the latest ACK message for this host
	char            d_ackBuffer[ ACK_BUFFERLENGTH ];
	unsigned short  d_ackLength;  // ammount of the buffer actually used.


	void ACKPacket( DWORD packetID, DWORD receiveTime );

	public:
	cHost();
	virtual ~cHost();

	float           GetAverageLinkPing( float percent );
	float           GetAverageTransPing( float percent );
	unsigned short  ProcessIncomingACKs( char *pBuffer, unsigned short len, DWORD receiveTime );
	unsigned short  ProcessIncomingReliable( char *pBuffer, unsigned short len, DWORD receiveTime );
	unsigned short  ProcessIncomingUnreliable( char *pBuffer, unsigned short len, DWORD receiveTime );
	unsigned short  AddACKMessage( char *pBuffer, unsigned short maxLen );

	void  SetPort( unsigned short port );
	bool  SetAddress( char *pAddress );
	bool  SetAddress( DWORD address );

	DWORD GetLastClockTime();
	void  SetLastClockTime( DWORD time );


	bool  WasClockTimeSet()
	{
	return d_bClockTimeSet;
	}


	DWORD GetAddress()
	{
	return d_address;
	}


	unsigned short GetPort()
	{
	return d_port;
	}


	QueueIn &GetInQueue()
	{
	return d_inQueue;
	}


	QueueOut &GetOutQueue()
	{
	return d_outQueue;
	}


	UnreliableQueueIn &GetUnreliableInQueue()
	{
	return d_unreliableInQueue;
	}


	UnreliableQueueOut &GetUnreliableOutQueue()
	{
	return d_unreliableOutQueue;
	}
	};

	class Host : public ThreadMonitor
	{
	private:
	DWORD address;
	unsigned short port;
	QueueIn queueIn;

	public:
	Host();
	virtual ~Host();

	unsigned short processIncomingData(char * const buffer, unsigned short length, DWORD recieveTime);

	void setPort(unsigned short p){port = p;}
	unsigned short getPort(){return port;}

	void setAddress(const char * const addr);
	void setAddress(DWORD addr){address = addr;}
	DWORD getAddress(){return address;}

	QueueIn getQueueIn(){return queueIn;}
	};

	class NetworkHandle : public Thread
	{
	private:
	typedef map<DWORD, Host *> HostMap;
	typedef list<Host *> HostList;

	SOCKET listenSocket, sendSocket;
	unsigned short localListenPort, foreignListenPort;
	bool started, listening, sending;
	unsigned int bytesTransferred;

	bool isServer, isClient;

	void resendData(DataPacket *packet, Host *host);
	void processIncomingData(char *data, unsigned short length, DWORD address, DWORD receiveTime);
	unsigned short  addClockData(char *data, unsigned short maxLength, Host *host);
	unsigned short  processIncomingClockData(char *data, unsigned short length, Host *host, DWORD receiveTime);

	//ThreadMonitor threadMonitor;

	protected:
	HostList hosts;
	HostMap hostMap;
	DWORD lastHandleID;
	NetClock clock;

	public:
	NetworkHandle();
	~NetworkHandle();

	void Initialize(unsigned short localListeningPort, unsigned short foreignListeningPort);
	void cleanup();

	void Update();

	void startListening();
	void stopListening();

	void startSending();
	void stopSending();

	DWORD threadFunction();

	DWORD getHostAddress(DWORD hostID);
	unsigned short getHostPort(DWORD hostID);

	void startServer();
	void stopServer();
	void startClient();
	void stopClient();

	unsigned short  GetUnreliableData( char *buffer, unsigned short maxLength, DWORD *hostID);
	unsigned short  GetReliableData( char *buffer, unsigned short maxLength, DWORD *hostID);
	void            UnreliableSendTo( char *str, unsigned short length, DWORD hostID);
	void            ReliableSendTo( char *str, unsigned short length, DWORD hostID);
	void            ResendPackets();

	DWORD hostCreate(const char * const address, unsigned short port);
	DWORD hostCreate(DWORD address, unsigned short port);
	void hostDestroy(DWORD hostID);
	float           HostGetAverageLinkPing(DWORD hostID, float precent);
	float           HostGetAverageTransPing(DWORD hostID, float precent);
	bool            HostGetIsOutEmpty(DWORD hostID);
	unsigned short  HostGetPort(DWORD hostID);
	DWORD           HostGetAddress(DWORD hostID);

	unsigned short  CountBits( DWORD value );

	bool IsServerOn(){return isServer;}
	bool IsClientOn(){return isClient;}

	const NetClock &GetClock(){return clock;}

	DWORD GetBytesTransfered(){return bytesTransferred;}
	void ResetBytesTransfered(){bytesTransferred = 0;}
	};*/

	/*class NetworkServices
	{
	public:
	int sendMessage(SOCKET socket, char *data, unsigned short dataSize){return send(socket, data, dataSize, 0);}
	int recieveMessage(SOCKET socket, char *buffer, unsigned short bufferSize){return recv(socket, buffer, bufferSize, 0);}
	};

	class NetworkClient
	{
	public:
	NetworkClient();
	~NetworkClient();

	SOCKET connectionSocket;
	};*/

/*#define WM_SOCKET WM_USER + 1
#define SM_ASYNC WM_USER + 2
#define SM_GETHOST WM_USER + 3

#define WINSOCK_VERSIONMAJOR 2
#define WINSOCK_VERSIONMINOR 0

#define DEFAULT_SERVERNAME "127.0.0.1"
#define DEFAULT_SERVERPORT 7000
#define DEFAULT_SESSIONNAME "Session"

#define IDLE_TIMEOUT 5000

	enum NetworkSide
	{
		SIDE_CLIENT = 0,
		SIDE_SERVER = 1
	};
	
	enum ConnectionState
	{
		STATE_DISCONNECTED = 0,
		STATE_CONNECTING = 1,
		STATE_CONNECTED = 2,
		STATE_DISCONNECTING = 3,
	};

	class CommunicationBase 
	{
	protected:
		HWND hWnd;
		WSADATA winsockData;
		unsigned short winsockVersion;
		SOCKET netSocket;
		SOCKADDR_IN addressInfo;
		HANDLE hostHandle;
		char hostBuffer[MAXGETHOSTSTRUCT], sessionName[64], serverName[64], IP[64];
		int state, socketFlags, serverPort, runFlag;
		DWORD version;

	public:
		CommunicationBase();
		virtual ~CommunicationBase();

		int initialize();
		int startup(int mode, HWND hwnd);
		int shutdownNetwork();
		HANDLE getHost(char *address = NULL, int port = 0);
		SOCKET createSocket();
		int changeWindow(HWND hwnd);
		int addressHost();
		int attemptToConnect();
		int disconnect();

		int setAsConnected(int errmsg);

		int onAsync(WPARAM wparam, LPARAM lparam);
		int onGetHost(WPARAM wparam, LPARAM lparam);

		virtual long determineSize(int type) {return 0;}

		inline HANDLE getHostHandle() const{return hostHandle;}
		inline SOCKET getSocket() const{return netSocket;}
		inline void setSocket(SOCKET socket ){netSocket = socket;}
		inline void setState(int statee ){state = statee;}
		inline int getState(){return state;}
		inline void setSessionName(char *name){strcpy(sessionName, name);}
		inline char *getSessionName(){return sessionName;}
		inline char *getServerName(){return serverName;}
		inline char *getIP() {return IP;}
		inline void setPort(int port) {serverPort = port;}
		inline int getPort(){return serverPort;}
	};

	class NetworkServerBase : public CommunicationBase
	{
	private:
		SOCKET clientSockets[1024];
		SOCKADDR_IN clientAddress[1024];
		Timer clientIdleTimer[1024];
		int numClients;
		int listenPort;

	public:
		NetworkServerBase();
		~NetworkServerBase(){};

		int awaitConnection();
		int sendData(char *data, unsigned int dataSize, unsigned int clientIndex);
		int recieveData(char *buffer, unsigned int maxLength, unsigned int clientIndex);

		int getNumClients(){return numClients;}

		void removeInactiveClients();
	};

	class NetworkClientBase : public CommunicationBase
	{
	private:
		char *readBuffer;
		char* sendBuffer;
		unsigned int readLength, sendLength;
		int canRead, canWrite;

	public:
		NetworkClientBase(){readBuffer = ""; sendBuffer = ""; readLength = 4096; sendLength = 0; canRead = canWrite = 0;}
		~NetworkClientBase(){}

		int awaitConnection();

		int sendData(char *data, unsigned int dataLength);
		int getRecievedData(char* buffer, unsigned int maxBufferLength);

		void sd();
		void rd();

		void setCanRead(bool c){canRead = c;}
		void setCanWrite(bool c){canWrite = c;}
		bool getCanRead(){return canRead;}
		bool getCanWrite(){return canWrite;}
	};

	class NetworkServerThread : public Thread
	{
	private:
		NetworkServerBase *server;
		ThreadMonitor mutex;
		int updateTick;
		Timer updateTimer;

	public:
		NetworkServerThread();
		~NetworkServerThread();

		DWORD threadFunction();

		void setServer(NetworkServerBase *s){server = s;}

		int getUpdateInterval(){return updateTick;}
		void setUpdateInterval(int interval){updateTick = interval;}
	};

	class NetworkClientThread : public Thread
	{
	private:
		NetworkClientBase *client;
		ThreadMonitor mutex;
		int updateTick;
		Timer updateTimer;

	public:
		NetworkClientThread();
		~NetworkClientThread();

		DWORD threadFunction();

		void setClient(NetworkClientBase *c){client = c;}

		int getUpdateInterval(){return updateTick;}
		void setUpdateInterval(int interval){updateTick = interval;}
	};*/
//};

#ifndef _EAGLENET_H
#define _EAGLENET_H 1

#include "Eagle.h"

using namespace std;

#define SIDE_SERVER 0
#define SIDE_CLIENT 1

#define ENETPROTOCOL_TCP 0
#define ENETPROTOCOL_UDP 1

#define ENETINVALID_SOCKET INVALID_SOCKET

#define ENETSTATE_CONNECTING 0
#define ENETSTATE_CONNECTED 1
#define ENETSTATE_DISCONNECTING 2
#define ENETSTATE_DISCONNECTED 4

#define ENETSERVER_ERROR 1
#define ENETCLIENT_ERROR 2

#define ENETMESSAGE_CONNECT -101
#define ENETMESSAGE_DISCONNECT -102
#define ENETMESSAGE_ADDCLIENT -103
#define ENETMESSAGE_REMOVECLIENT -104
#define ENETMESSAGE_PING -105

#define ENETCLIENT_TIMEOUT 30000

namespace ProjectEagle
{
	class ENetMessage;
	class ENetClient;
	class ENetServer;
	class ENetSocket;

	class ENetMessage
	{
	private:
		bool overflow;
		int maxSize;
		int size;
		int readCount;

		char *getNewPoint(int length);

	public:
		void initialize(char *data, int length);
		void clear();
		void write(void *data, int length);
		void addSequences(ENetClient *client);

		void writeByte(char c);
		void writeShort(short s);
		void writeLong(long l);
		void writeFloat(float f);
		void writeString(char *s);

		void beginRead();
		void beginRead(int size);
		char *read(int size);
		char readByte();
		short readShort();
		long readLong();
		float readFloat();
		char *readString();

		bool getOverflow(){return overflow;}
		int getSize(){return size;}
		void setSize(int s){size = s;}

		char *data;
		char outgoingData[1024];
	};

	class ENetClient
	{
	private:
		void dumpBuffer();
		void parsePacket(ENetMessage *message);

		int connectionState;

		unsigned short outgoingSequence;
		unsigned short incomingSequence;
		unsigned short incomingAcknowledged;

		unsigned short droppedPackets;

		int serverPort;
		char serverIP[32];
		int index;

		char name[32];

		SOCKET socket;
		sockaddr address;

		int pingSent;
		int ping;

		int lastMessageTime;

		bool initialized;

	public:
		ENetClient();
		~ENetClient();

		int initialize(char *localIP, char *remoteIP, int port);
		void shutdown();
		void reset();
		void sendConnect(char *name);
		void sendDisconnect();
		void sendPing();

		void setConnectionState(int state){connectionState = state;}
		int getConnectionState(){return connectionState;}

		int getPacket(char *data, sockaddr *fromAddress);
		void sendPacket();
		void sendPacket(ENetMessage *message);

		unsigned short getOutgoingSequence(){return outgoingSequence;}
		void setOutgoingSequence(unsigned short sequence){outgoingSequence = sequence;}
		void increaseOutgoingSequence(){outgoingSequence++;}

		unsigned short getIncomingSequence(){return incomingSequence;}
		void setIncomingSequence(unsigned short sequence){incomingSequence = sequence;}

		unsigned short getIncomingAcknowledged(){return incomingAcknowledged;}
		void setIncomingAcknowledged(unsigned short sequence){incomingAcknowledged = sequence;}

		unsigned short getDroppedPackets(){return droppedPackets;}
		void setDroppedPackets(unsigned short packets){droppedPackets = packets;}

		bool isInitialized(){return initialized;}

		int getIndex(){return index;}
		void setIndex(int i){index = i;}

		char *getName(){return name;}
		void setName(char *n){strcpy(name, n);}

		SOCKET getSocket(){return socket;}
		void setSocket(SOCKET s){socket = s;}

		sockaddr *getSocketAddress(){return &address;}
		void setSocketAddress(sockaddr *a){memcpy(&address, a, sizeof(sockaddr));}

		int getPingSent(){return pingSent;}
		void setPing(int p){ping = p;}

		int getLastMessageTime(){return lastMessageTime;}
		void setLastMessageTime(int t){lastMessageTime = t;}

		ENetMessage message;
		ENetClient *next;
	};

	class ENetServer
	{
	private:
		void sendAddClient(ENetClient *client);
		void sendRemoveClient(ENetClient *client);
		
		void addClient(sockaddr *address, char *name);
		void removeClient(ENetClient *client);

		void parsePacket(ENetMessage *message, sockaddr *address);

		int checkForTimeout(char* data, sockaddr *fromAddress);

		ENetClient *clientList;

		int port;
		SOCKET socket;
		int runningIndex;

		bool initialized;

	public:
		ENetServer();
		~ENetServer();

		int initialize(char *localIP, int serverPort);
		void shutdown();

		void sendPing();
		
		int getPacket(char *data, sockaddr *fromAddress);
		void sendPackets();

		bool isInitialized(){return initialized;}
		ENetClient *getClientList(){return clientList;}

		int getPort(){return port;}

		int getRunningIndex(){return runningIndex;}
	};

	int ENetInitialize();
	int ENetInitializeWinsock();
	void ENetShutdown();

	SOCKET ENetCreateSocket(int protocol);
	int ENetSetNonBlocking(SOCKET socket, unsigned long mode);
	int ENetSetBroadcasting(SOCKET socket, int mode);

	int ENetStringToSockaddr(char *addressString, sockaddr *address);

	SOCKET ENetOpenUDPSocket(char networkInterface[32], int port);
	void ENetCloseSocket(SOCKET socket);

	int ENetGetPacket(SOCKET socket, char *data, sockaddr *fromAddress);
	void ENetSendPacket(SOCKET socket, char *data, int length, sockaddr address);
	void ENetBroadcast(SOCKET socket, char *data, int length, int port);

	int ENetGetCurrentSystemTime();
};

#endif