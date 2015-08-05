#ifndef _EAGLENET_H
#define _EAGLENET_H 1

#include <WinSock2.h>
#include <Windows.h>

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