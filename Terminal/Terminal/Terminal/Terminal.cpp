#include <iostream>
#include "ParserUri.h"
#include <iostream>
#include <string>
#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <memory>
#include <list>
#include "srt.h"
#include "SRTTerminal.h"
#include <fstream>

using namespace std;

bool SysInitializeNetwork()
{
	WORD wVersionRequested = MAKEWORD(2, 2);
	WSADATA wsaData;
	return WSAStartup(wVersionRequested, &wsaData) == 0;
}

void SysCleanupNetwork()
{
	WSACleanup();
}

int main(int argc, char** argv) 
{
	if (!SysInitializeNetwork())
		throw runtime_error("Can't initialize network!");

	struct NetworkCleanup
	{
		~NetworkCleanup()
		{
			SysCleanupNetwork();
		}
	} cleanupobj;

	string uri = "srt://:9002";
	ParserUri u(uri);

	if (u.type() != ParserUri::SRT) {
		cout << uri << " is unknown type" << endl;
		return 1;
	}

	unsigned long chunk = 1316;
	unsigned long transmit_chunk_size = SRT_LIVE_DEF_PLSIZE;

	volatile bool int_state = false;
	volatile bool timer_state = false;

	unique_ptr<SRTTerminal> src;
	bool srcConnected = false;

	int pollid = srt_epoll_create();
	if (pollid < 0)
	{
		cerr << "Can't initialize epoll";
		return 1;
	}

	size_t receivedBytes = 0;
	size_t wroteBytes = 0;
	size_t lostBytes = 0;
	size_t lastReportedtLostBytes = 0;
	list<shared_ptr<vector<char>>> dataqueue;
	int spacetime = 0;

	try
	{
		while (!int_state && !timer_state)
		{
			if (!src.get()) {
				src = SRTTerminal::CreateSRTSource(uri);
				if (!src.get())
				{
					cout << "Unsupported source type" << endl;
					return 1;
				}

				int events = SRT_EPOLL_ERR | SRT_EPOLL_IN;
				switch (src->uri.type())
				{
				case ParserUri::SRT:
				{
					if (srt_epoll_add_usock(pollid, src->GetSRTSocket(), &events))
					{
						cout << "Failed to add SRT source to poll, " << src->GetSRTSocket() << endl;
						return 1;
					}
				}
				break;
				default:
				{
					cout << "Unknow src uri type" << endl;
				}
				break;
				}
			}
			
			int srtrfdslen = 1;
			SRTSOCKET srtrfds;
			int sysrfdslen = 1;
			SYSSOCKET sysrfds;

			if (srt_epoll_wait(pollid, &srtrfds, &srtrfdslen, 0, 0, 100, &sysrfds, &sysrfdslen, 0, 0) >= 0)
			{
				bool doabort = false;
				SRTSOCKET s = srtrfds;
				SRT_SOCKSTATUS status = srt_getsockstate(s);

				if ((false) && status != SRTS_CONNECTED)
				{
					cout << " status " << status << endl;
				}
				switch (status)
				{

				case SRTS_CONNECTED:
				{
					if (srcConnected)
					{
						cout << "SRT source connected" << endl;
						srcConnected = true;
					}
				}
				break;

				case SRTS_BROKEN:
				case SRTS_CLOSED:
				case SRTS_NONEXIST:
				{
					if (srcConnected)
					{
						cout << "SRT source disconnected" << endl;
						srcConnected = false;
					}

					doabort = true;
				}
				default:
					break;
				}
				if (doabort)
				{
					cout << "doabout" << endl;
					break;
				}
			}

			if (src.get() && (srtrfdslen || sysrfdslen))
			{
				std::shared_ptr<vector<char>> pdata(new vector<char>(chunk));
				if (!src->Read(chunk, *pdata) || (*pdata).empty())
				{
					cout << "No data" << endl;
					if (dataqueue.size() > 0) {
						if (spacetime > 10)
						{
							break;
						}
						else
						{
							spacetime++;
							cout << "Current space is :" << spacetime << endl;
							continue;
						}
						
					}
					else
					{
						continue;
					}
				}
				else
				{
					spacetime = 0;
				}
				dataqueue.push_back(pdata);
				receivedBytes += (*pdata).size();
				cout << "Current datalist size is :" << dataqueue.size() << " and recbytes is :" << receivedBytes << endl;
			}

		}
	}
	catch (exception& e)
	{
		cout << "Error: " << e.what() << endl;
		src->Close();
		if (!src.get())
		{
			src->Close();
		}
		return 255;
	}

	src->Close();


	if (!dataqueue.empty())
	{
		char *tmpPath = "c://tmp//outterminaldata.raw";
		remove(tmpPath);

		ofstream writedata;		
		writedata.open("c://tmp//outterminaldata.raw", ios::out | ios::app | ios::binary );
		while (!dataqueue.empty())
		{
			std::shared_ptr<vector<char>> pdata = dataqueue.front();
			cout << "pdata length " << (*pdata).size() << " and current dataqueue is: " << dataqueue.size() << endl;
			writedata.write(pdata->data(), sizeof(char)*(*pdata).size());
			dataqueue.pop_front();
		}
		writedata.flush();
		writedata.close();
	}

	return 0;
}