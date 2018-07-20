#include "SRTTerminal.h"

unsigned long transmit_chunk_size = SRT_LIVE_DEF_PLSIZE;

SRTTerminal::SRTTerminal()
{
}

SRTTerminal::SRTTerminal(string host, int port)
{
	Init(host, port);
}


SRTTerminal::~SRTTerminal()
{
	Close();
}

unique_ptr<SRTTerminal> SRTTerminal::CreateSRTSource(const std::string & url)
{
	unique_ptr<SRTTerminal> ptr;
	ParserUri u(url);
	switch (u.type())
	{
	default:
		cout << "Unknown URL type, only srt support" << endl;
		break;
	case ParserUri::SRT:
	{
		if (u.portno() <= 1024)
		{
			cout << "Port value invalid must be >1024 " << endl;
			throw invalid_argument("Invalid port number");
		}
		ptr.reset(new SRTTerminal(u.host(), u.portno()));
		break;
	}
	}
	if (ptr.get())
	{
		ptr->uri = move(u);
	}

	return ptr;
}

bool SRTTerminal::Read(size_t chunk, vector<char>& data)
{
	static unsigned long counter = 1;
	if (data.size() < chunk)
		data.resize(chunk);
	bool ready = true;
	int stat;
	bool receiveddata = false;

	stat = srt_recvmsg(m_sock, data.data(), chunk);

	if (stat == SRT_ERROR)
	{
		if (srt_getlasterror(NULL) == SRT_EASYNCRCV)
		{
			cout << "SRT_ERROR   ";
			data.clear();
			return false;
		}
	}

	if (stat == 0)
	{
		cout << "EOF" << endl;
	}

	if (stat > 0)
	{
		chunk = size_t(stat);
		if (chunk < data.size())
			data.resize(chunk);
		cout << "Get Data stat:" << stat << endl;
		receiveddata = true;
	}
	return true;
}

bool SRTTerminal::IsOpen()
{
	SRT_SOCKSTATUS status = srt_getsockstate(m_sock);
	return status > SRTS_INIT && status < SRTS_BROKEN;
}

bool SRTTerminal::End()
{
	return srt_getsockstate(m_sock) > SRTS_CONNECTED;
}

void SRTTerminal::Close()
{
	cout << "DESTROYING CONNECTION, closing sockets (rt%" << m_sock << ")" << endl;
	if (m_sock != SRT_INVALID_SOCK)
	{
		srt_close(m_sock);
		m_sock = SRT_INVALID_SOCK;
	}
}

void SRTTerminal::Init(string host, int port)
{
	cout << "Try to connect to host: " << host << " and the port is : " << port << endl;
	OpenClient(host, port);
}

int SRTTerminal::ConfigurePre(SRTSOCKET sock)
{
	int result = 0;
	bool no = false;
	result = srt_setsockopt(sock, 0, SRTO_TSBPDMODE, &no, sizeof no);
	if (result == -1)
	{
		return result;
	}
	return result;
}

void SRTTerminal::OpenClient(string host, int port)
{
	PrepareClient();
	ConnectClient(host, port);
}

void SRTTerminal::PrepareClient()
{
	m_sock = srt_socket(AF_INET, SOCK_DGRAM, 0);
	if (m_sock == SRT_ERROR)
	{
		UDT::ERRORINFO info = UDT::getlasterror();
	}
	int stat = ConfigurePre(m_sock);
	if (stat == SRT_ERROR)
	{
		UDT::ERRORINFO info = UDT::getlasterror();
	}
}

void SRTTerminal::ConnectClient(string host, int port)
{
	sockaddr_in sa = CreateAddrInet(host, port);
	sockaddr* psa = (sockaddr*)&sa;
	cout << "Connecting to " << host << ":" << port << endl;
	int stat = srt_connect(m_sock, psa, sizeof sa);
	if (stat == SRT_ERROR)
	{
		srt_close(m_sock);
		UDT::ERRORINFO info = UDT::getlasterror();
	}
	stat = ConfigurePost(m_sock);
	if (stat == SRT_ERROR)
		UDT::ERRORINFO info = UDT::getlasterror();
}

int SRTTerminal::ConfigurePost(SRTSOCKET sock)
{
	bool no = false;
	int result = 0;
	result = srt_setsockopt(sock, 0, SRTO_RCVSYN, &no, sizeof no);
	if (result == -1)
		return result;

	if (m_timeout)
		return srt_setsockopt(sock, 0, SRTO_RCVTIMEO, &m_timeout, sizeof m_timeout);

}

sockaddr_in SRTTerminal::CreateAddrInet(const std::string & name, int port)
{
	sockaddr_in sa;
	memset(&sa, 0, sizeof sa);
	sa.sin_family = AF_INET;
	sa.sin_port = htons(port);

	if (name != "")
	{
		if (inet_pton(AF_INET, name.c_str(), &sa.sin_addr) == 1)
			return sa;

		// XXX RACY!!! Use getaddrinfo() instead. Check portability.
		// Windows/Linux declare it.
		// See:
		//  http://www.winsocketdotnetworkprogramming.com/winsock2programming/winsock2advancedInternet3b.html
		//hostent* he = gethostbyname(name.c_str());
		hostent* he = gethostbyname(name.c_str());
		if (!he || he->h_addrtype != AF_INET)
			throw std::invalid_argument("SrtSource: host not found: " + name);
		sa.sin_addr = *(in_addr*)he->h_addr_list[0];
	}
	return sa;
}
