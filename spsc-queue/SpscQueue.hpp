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
		if (mBuffer.size() == Capacity)
			return false;
		
		const auto write = mWriteIndex.load(std::memory_order_relaxed); // we're the only one touching this
		std::size_t next = (write + 1) % Capacity;

		
		if (next == mReadIndex.load(std::memory_order_acquire))
			return false; // we're full
		mBuffer[write];
		mWriteIndex.store(next, std::memory_order_release);
		return true;
	}

	bool TryDequeue(T& out);
};