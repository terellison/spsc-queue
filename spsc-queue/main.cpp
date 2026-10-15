#include <iostream>
#include <thread>
#include <vector>
#include <atomic>
#include <cstdlib>
#include "SpscQueue.hpp"

using spsc = SpscQueue<int, 10>;
constexpr int kItemCount = 1'000'000;
void writer(spsc& queue);
void reader(spsc& queue);

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		std::cout << "Usage: " << argv[0] << " <writer|reader>" << std::endl;
		return 1;
	}

	spsc queue;
	int value = 0;
	int numThreads = std::atoi(argv[1]);
	std::vector<std::thread> threads;
	std::cout << "Demo 1: Single Producer, Single Consumer - all reads should arrive in order" << std::endl;
	
	// Enqueue some values
	auto writeThread = std::thread(writer, std::ref(queue));
	
	// Dequeue some values
	auto readThread = std::thread(reader, std::ref(queue));
	
	writeThread.join();
	readThread.join();
	
	std::cout << "Demo 2: Multiple Producers, Single Consumer - all reads aren't guaranteed to arrive in order" << std::endl;

	for (int i = 0; i < numThreads; ++i)
	{
		threads.emplace_back(writer, std::ref(queue));
	}
	for (int i = 0; i < numThreads; ++i)
	{
		threads.emplace_back(reader, std::ref(queue));
	}

	for (auto& t : threads)
	{
		t.join();
	}

	return 0;
}

void writer(spsc& queue)
{
	for (int i = 0; i < kItemCount; ++i)
	{
		while (!queue.TryEnqueue(i))
		{
			// Busy wait
		}
	}
}

void reader(spsc& queue)
{
	int value;
	for (int expected = 0; expected < kItemCount; ++expected)
	{
		while (!queue.TryDequeue(value))
		{
			// Busy wait
		}
		if (value != expected)
		{
			std::cerr << "Error: expected " << expected << " but got " << value << std::endl;
			std::abort();
		}
	}
	std::cout << "OK: " << kItemCount << " values arrived in order" << std::endl;
}