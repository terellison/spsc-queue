#pragma once
#include <array>
#include <atomic>

template <typename T, size_t Capacity>
class SpscQueue
{
private:
	std::array<T, Capacity> mBuffer;
	std::atomic<std::size_t> mReadIndex{ 0 };
	std::atomic<std::size_t> mWriteIndex{0};
public:
	bool TryEnqueue(const T& data)
	{	
		const auto write = mWriteIndex.load(std::memory_order_relaxed); // we're the only one touching this
		std::size_t next = (write + 1) % Capacity;

		if (next == mReadIndex.load(std::memory_order_acquire))
			return false; // we're full

		mBuffer[write] = data;
		mWriteIndex.store(next, std::memory_order_release);
		return true;
	}

	bool TryDequeue(T& out)
	{
		const auto read = mReadIndex.load(std::memory_order_relaxed); // we're the only one touching this
		if (read == mWriteIndex.load(std::memory_order_acquire))
			return false; // we're empty

		out = mBuffer[read];
		mReadIndex.store((read + 1) % Capacity, std::memory_order_release);
		return true;
	}
};