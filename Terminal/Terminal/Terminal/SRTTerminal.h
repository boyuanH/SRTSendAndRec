#pragma once
#include <string>
#include <memory>
#include <vector>
#include "srt.h"
#include "udt.h"
#include "ParserUri.h"

using namespace std;

class SRTTerminal
{
public:
	SRTTerminal();
	SRTTerminal(string host, int port);
	~SRTTerminal();

	static unique_ptr<SRTTerminal> CreateSRTSource(const std::string& url);

private:
	SRTSOCKET m_sock = SRT_INVALID_SOCK;
	unsigned long transmit_chunk_size = 1316;
	int  m_timeout;
	string m_mode;

public:
	SRTSOCKET GetSRTSocket() { return m_sock; }
	ParserUri uri;
	bool Read(size_t chunk, vector<char>& data);
	bool IsOpen();
	bool End();
	void Close();
	void Init(string host, int port);

private:
	int ConfigurePre(SRTSOCKET sock);
	void OpenClient(string host, int port);
	void PrepareClient();
	void ConnectClient(string host, int port);
	int ConfigurePost(SRTSOCKET sock);
	sockaddr_in CreateAddrInet(const std::string& name, int port);
};

