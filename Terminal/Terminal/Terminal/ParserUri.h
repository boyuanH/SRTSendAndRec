#pragma once

#include <string>

using namespace std;

class ParserUri
{
public:

	enum UriType
	{
		UNKNOWN, SRT
	};
public:
	ParserUri() : m_uriType(UNKNOWN) {};
	ParserUri(const string& strUrl);
	~ParserUri();

	string uri();
	string host();
	string port();
	string proto();
	unsigned short int portno();
	string hostport();
	UriType type();

private:
	UriType m_uriType;
	string m_origUri;
	string m_proto;
	string m_host;
	string m_port;

private:
	void Parse(const string& strUrl);

};

