#include <iostream>
#include <thread>

#include "KThreadPool.hpp"

std::mutex coutMutex;

int main()
{
	for (int i = 0; i < 100; ++i)
	{
		KTP::KThreadPool threadPool;
		threadPool.InitQueue("Worker", 16);
		for (int i = 0; i < 10; ++i)
		{
			threadPool.EnqueueTask("Worker", [i]() {
				{
					std::lock_guard<std::mutex> lock(coutMutex);
					std::cout << "Task " << i << " is running on thread " << std::this_thread::get_id() << '\n';
				}
				});
		}
		threadPool.WaitUntilQueueFinished("Worker");
		threadPool.ReleaseQueue("Worker");
	}

	return 0;
}