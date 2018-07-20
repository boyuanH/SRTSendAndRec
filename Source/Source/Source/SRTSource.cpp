#include "SRTSource.h"




SRTSource::SRTSource(string host, int port)
{
	Init(host, port);
}

SRTSource::~SRTSource()
{
}

unique_ptr<SRTSource> SRTSource::CreateSRTSource(const std::string & url)
{
	unique_ptr<SRTSource> ptr;
	int iport = 0;
	ParserUri u(url);
	iport = u.portno();
	switch (u.type())
	{
	case ParserUri::SRT:
		if (iport <= 1024)
		{
			cerr << "Port value invalid: " << iport << " - must be >1024\n";
			throw invalid_argument("Invalid port number");
		}
		ptr.reset(new SRTSource(u.host(), u.portno()));
		break;
	default:
		cout << "Unknown URL type, only srt support" << endl;
		break;
	}
	if (ptr.get())
	{
		ptr->uri = move(u);
	}

	return ptr;
}

void SRTSource::PrepareListener(string host, int port, int backlog)
{
	m_bindsock = srt_socket(AF_INET, SOCK_DGRAM, 0);
	if (m_bindsock == SRT_ERROR)
	{
		cout << "Error on bind srt_socket for" << UDT::getlasterror().getErrorMessage() << endl;
	}
	int stat = ConfigurePre(m_bindsock);
	if (stat == SRT_ERROR)
	{
		cout << "Error on ConfigurePre for" << UDT::getlasterror().getErrorMessage() << endl;
	}
	sockaddr_in sa = CreateAddrInet(host, port);
	sockaddr* psa = (sockaddr*)&sa;
	cout << "Binding a server on " << host << ":" << port << " ..." << endl;;
	stat = srt_bind(m_bindsock, psa, sizeof sa);
	if (stat == SRT_ERROR)
	{
		srt_close(m_bindsock);
		cout << "Error on srt_bind for" << UDT::getlasterror().getErrorMessage() << endl;
	}
	cout << "Listen..." << endl;

	stat = srt_listen(m_bindsock, backlog);
	if (stat == SRT_ERROR)
	{
		srt_close(m_bindsock);
		cout << "Error on srt_listen for" << UDT::getlasterror().getErrorMessage() << endl;
	}


}

bool SRTSource::AcceptNewClient()
{
	sockaddr_in scl;
	int sclen = sizeof scl;

	m_sock = srt_accept(m_bindsock, (sockaddr*)&scl, &sclen);
	if (m_sock == SRT_INVALID_SOCK)
	{
		srt_close(m_bindsock);
		m_bindsock = SRT_INVALID_SOCK;
		cout << "Error on srt_accept for" << UDT::getlasterror().getErrorMessage() << endl;
	}
	srt_close(m_bindsock);
	m_bindsock = SRT_INVALID_SOCK;
	int stat = ConfigurePost(m_sock);
	if (stat == SRT_ERROR)
	{
		cout << "Error on ConfigurePost for" << UDT::getlasterror().getErrorMessage() << endl;
	}
	return true;
}

SRTSOCKET SRTSource::Listener()
{
	return m_bindsock;
}

void SRTSource::Close()
{
	if (m_sock != SRT_INVALID_SOCK)
	{
		srt_close(m_sock);
		m_sock = SRT_INVALID_SOCK;
	}

	if (m_bindsock != SRT_INVALID_SOCK)
	{
		srt_close(m_bindsock);
		m_bindsock = SRT_INVALID_SOCK;
	}
}

void SRTSource::Init(string host, int port)
{
	m_adapter = host;
	cout << "Opening SRT as server on " << host << ":" << port << endl;;
	OpenServer(host, port);
}

bool SRTSource::Write(const vector<char>& data)
{
	int stat = srt_sendmsg2(m_sock, data.data(), data.size(), nullptr);
	if (stat == SRT_ERROR)
	{
		return false;
	}
	return true;
}

bool SRTSource::IsOpen()
{
	SRT_SOCKSTATUS st = srt_getsockstate(m_sock); 
	return st > SRTS_INIT && st < SRTS_BROKEN;
}

SRTSOCKET SRTSource::GetSRTSocket()
{	
		SRTSOCKET socket = m_sock;
		if (socket == SRT_INVALID_SOCK)
			socket = Listener();
		return socket;
	
}

sockaddr_in SRTSource::CreateAddrInet(const std::string & name, unsigned short port)
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
		hostent* he = gethostbyname(name.c_str());
		if (!he || he->h_addrtype != AF_INET)
			throw std::invalid_argument("SrtSource: host not found: " + name);

		sa.sin_addr = *(in_addr*)he->h_addr_list[0];
	}

	return sa;
}

int SRTSource::ConfigurePost(SRTSOCKET sock)
{
	bool no = false;
	int result = 0;
	result = srt_setsockopt(sock, 0, SRTO_SNDSYN, &no, sizeof no);
	if (result == -1)
		return result;
	return 0;
}

int SRTSource::ConfigurePre(SRTSOCKET sock)
{
	int result = 0;
// 	bool no = false;
// 	result = srt_setsockopt(sock, 0, SRTO_RCVSYN, &no, sizeof no);
// 	if (result == -1)
// 		return result;
	int yes = 1;
	result = srt_setsockopt(sock, 0, SRTO_SENDER, &yes, sizeof yes);
	if (result == -1)
		return result;

	return 0;
}

void SRTSource::SetupAdapter(const std::string & host, int port)
{
	sockaddr_in localsa = CreateAddrInet(host, port);
	sockaddr* psa = (sockaddr*)&localsa;
	int stat = srt_bind(m_sock, psa, sizeof localsa);
}

void SRTSource::ConnectClient(string host, int port)
{
	PrepareClient();

	if (m_outgoing_port)
	{
		SetupAdapter("", m_outgoing_port);
	}

	ConnectClient(host, port);
}

void SRTSource::PrepareClient()
{
	m_sock = srt_socket(AF_INET, SOCK_DGRAM, 0);
	if (m_sock == SRT_ERROR)
		cout<<"PrepareClient error on srt_socket"<< UDT::getlasterror().getErrorMessage() << endl;

	int stat = ConfigurePre(m_sock);
	if (stat == SRT_ERROR)
		cout << "PrepareClient error on ConfigurePre" << UDT::getlasterror().getErrorMessage() << endl;
}

void SRTSource::OpenServer(string host, int port)
{
	PrepareListener(host, port, 1);
}
