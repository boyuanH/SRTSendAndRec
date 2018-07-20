#pragma once
#include <list>
#include <memory>
#include <vector>
//#include <mutex>
#include <iostream>

using namespace std;

class AudioDataBuffer
{
public:
	AudioDataBuffer();
	~AudioDataBuffer();

private:
	list<shared_ptr<vector<char>>> dataqueue;
	//mutex mtx;
	//timed_mutex mtx;

public:
	int WriteIntoBuffer(vector<char>& data);
	int ReadFromBuffer(vector<char>& data);
};

