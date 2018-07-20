#include "AudioDataBuffer.h"

int MAXLENGTH4BUFFER = 256;

AudioDataBuffer::AudioDataBuffer()
{
}


AudioDataBuffer::~AudioDataBuffer()
{
}

int AudioDataBuffer::WriteIntoBuffer(vector<char>& data)
{
	
// 	while (!mtx.try_lock_for(chrono::milliseconds(5)))
// 	{
// 		printf("WriteIntoBuffer  lock\n"); fflush(stdout);
// 		continue;
// 	}
	if (dataqueue.size()>=MAXLENGTH4BUFFER)
	{
		cout << "Mic Buffer is full, data lost " << (*dataqueue.front()).size()<< endl;
		dataqueue.pop_front();
	}
	shared_ptr<vector<char>> pdata (new vector<char>(data.size()));
	pdata->assign(data.begin(), data.end());
	dataqueue.push_back(pdata);
	//mtx.unlock();
	return data.size();
}

int AudioDataBuffer::ReadFromBuffer(vector<char>& data)
{		
// 	while (!mtx.try_lock_for(chrono::milliseconds(5)))
// 	{
// 		printf("ReadFromBuffer  lock\n"); fflush(stdout);
// 		continue;
// 	}
	if (dataqueue.size() == 0)
	{
		return -1;
	}
	data.resize((*dataqueue.front()).size());
	data.clear();
	data.assign((*dataqueue.front()).begin(), (*dataqueue.front()).end());
	dataqueue.pop_front();
	//mtx.unlock();
	return data.size();
}
