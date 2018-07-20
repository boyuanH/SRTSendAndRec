#pragma once

#include <string>
#include <memory>
#include <vector>
#include "srt.h"
#include "udt.h"
#include "ParserUri.h"

using namespace std;

class SRTSource
{
public:
	SRTSource(string host, int port);
	~SRTSource();
public:
	static unique_ptr<SRTSource> CreateSRTSource(const std::string& url);
public:
	ParserUri uri;
private:
	SRTSOCKET m_sock = SRT_INVALID_SOCK;
	SRTSOCKET m_bindsock = SRT_INVALID_SOCK;
	string m_adapter;
	int m_outgoing_port;

public:
	void PrepareListener(string host, int port, int backlog);
	bool AcceptNewClient();

	SRTSOCKET Listener();

	void Close();
	void Init(string host, int port);
	bool Write(const vector<char>& data);
	bool IsOpen();
	SRTSOCKET GetSRTSocket();

private:
	sockaddr_in CreateAddrInet(const std::string& name, unsigned short port);	
	int ConfigurePost(SRTSOCKET sock);
	int ConfigurePre(SRTSOCKET sock);

	void SetupAdapter(const std::string& host, int port);
	void ConnectClient(string host, int port);
	void PrepareClient();

	void OpenServer(string host, int port);

};

