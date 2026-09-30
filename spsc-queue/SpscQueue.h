#pragma once
#include <array>
#include <atomic>

template <typename T, size_t Capacity>
class SpscQueue
{
private:
	std::array<T, Capacity> mBuffer;
	std::atomic<int> mReadIndex(0), mWriteIndex{ 0 };
public:
	SpscQueue::SpscQueue(size_t size);
	bool TryEnqueue(const T& data);
	bool TryDequeue(T& out);
};