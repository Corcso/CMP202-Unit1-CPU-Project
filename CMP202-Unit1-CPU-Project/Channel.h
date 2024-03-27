#pragma once
#include <queue>
#include <semaphore>
#include <mutex>
#include <atomic>
template<typename T>
class Channel
{
public:
	struct ChannelDataOut {
		T data;
		bool success;
	};

	Channel();

	void addData(T data);
	ChannelDataOut getData();

	void announceEndOfData();
	bool isDataSendOver();

private:
	std::mutex queueMtx;
	std::queue<T> dataQueue;

	std::counting_semaphore<INT32_MAX> dataAvailable;

	std::atomic<bool> dataSendOver;
};

template<typename T>
inline Channel<T>::Channel()
	:dataAvailable(0)
{
}

template<typename T>
inline void Channel<T>::addData(T data)
{
	if (dataSendOver) return;
	queueMtx.lock();
	dataQueue.push(data);
	dataAvailable.release();
	queueMtx.unlock();
}

template<typename T>
inline Channel<T>::ChannelDataOut Channel<T>::getData()
{
	if (dataAvailable.try_acquire()) {
		queueMtx.lock();
		T dataToReturn = dataQueue.front();
		dataQueue.pop();
		queueMtx.unlock();
		return ChannelDataOut{ dataToReturn, true };
	}
	else return ChannelDataOut{ T(), true};
}

template<typename T>
inline void Channel<T>::announceEndOfData()
{
	dataSendOver = true;
}

template<typename T>
inline bool Channel<T>::isDataSendOver()
{
	std::lock_guard<std::mutex> lockGuard(queueMtx);
	if (dataSendOver && dataQueue.empty()) return true;
	return false;
}
