#include "ParserUri.h"
#include <map>

map<string, ParserUri::UriType> types;

struct UriParserInit
{
	UriParserInit()
	{
		types["srt"] = ParserUri::SRT;
		types[""] = ParserUri::UNKNOWN;
	}
} g_uriparser_init;


ParserUri::ParserUri(const string & strUrl)
{
	Parse(strUrl);
}

ParserUri::~ParserUri()
{
}

string ParserUri::uri()
{
	return m_origUri;
}

string ParserUri::host()
{
	return m_host;
}

string ParserUri::port()
{
	return m_port;
}

string ParserUri::proto()
{
	return m_proto;
}

unsigned short int ParserUri::portno()
{
	try
	{
		int i = atoi(m_port.c_str());
		if (i <= 0 || i > 65535) {
			return 0;
		}
		return i;
	}
	catch (...)
	{
		return 0;
	}
}

string ParserUri::hostport()
{
	return host() + ":" + port();;
}

ParserUri::UriType ParserUri::type()
{
	return m_uriType;
}

void ParserUri::Parse(const string & strUrl)
{
	int iQueryStart = -1;
	size_t idx = strUrl.find("?");
	if (idx != string::npos)
	{
		m_host = strUrl.substr(0, idx);
		iQueryStart = idx + 1;
	}
	else
	{
		m_host = strUrl;
	}

	idx = m_host.find("://");
	if (idx != string::npos)
	{
		m_proto = m_host.substr(0, idx);
		m_host = m_host.substr(idx + 3, m_host.size() - (idx + 3));
	}

	idx = m_host.find("/");
	if (idx != string::npos)
	{
		m_host = m_host.substr(0, idx);
	}

	idx = m_host.find(":");
	if (idx != string::npos)
	{
		m_port = m_host.substr(idx + 1, m_host.size() - (idx + 1));
		m_host = m_host.substr(0, idx);
	}

	if (m_port == "" && m_host != "")
	{
		const char* beg = m_host.c_str();
		const char* end = m_host.c_str() + m_host.size();
		char* eos = 0;
		long val = strtol(beg, &eos, 10);
		if (val > 0 && eos == end)
		{
			m_port = m_host;
			m_host = "";
		}
	}

	if (m_host == "")
	{
		m_host = "127.0.0.1";
	}

	m_uriType = types[m_proto];
	m_origUri = strUrl;
}
