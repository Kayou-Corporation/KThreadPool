#include "ThreadPool.hpp"

#include <ranges>
#include <iostream>

namespace Kayou
{
	ThreadPool::~ThreadPool()
	{
		m_threadManagers.clear();
	}

	void ThreadPool::InitQueue(const char* queueName, uint8_t numThreads)
	{
		m_threadManagers.try_emplace(queueName, std::make_unique<ThreadManager>(queueName, numThreads));
	}

	void ThreadPool::EnqueueTask(const char* queueName, std::move_only_function<void()> task, Priority priority)
	{
		if (m_threadManagers.find(queueName) == m_threadManagers.end()) [[unlikely]]
		{
#if defined (PERMISSIVE_EXCEPTIONS)
			task();
#endif

			if (!m_tooManyErrors)
				std::cerr << "\033[1;31m[KThreads error] Wrong queue name when enqueueing: " << queueName << "\033[0m\n";
			else
				return;

			++m_nbErrors;
			if (m_nbErrors >= m_maxErrorsCount)
			{
				std::cerr << "\033[1;33mToo many errors displayed, following errors for this issue won't be displayed\033[0m\n";
				m_tooManyErrors = true;
			}
			return;
		}

		if (m_tooManyErrors) [[unlikely]]
		{
			m_nbErrors = 0u;
			m_tooManyErrors = false;
		}

		m_threadManagers.at(queueName)->Enqueue(std::move(task), priority);
	}

	void ThreadPool::WaitUntilQueueFinished(const char* queueName) const
	{
		if (m_threadManagers.find(queueName) == m_threadManagers.end()) [[unlikely]]
		{
			std::cerr << "\033[1;31m[KThreads error] Wrong queue name when checking for finished, expect undefined behavior: " << queueName << "\033[0m\n";
			return;
		}

		m_threadManagers.at(queueName)->WaitUntilFinished();
	}

	void ThreadPool::WaitUntilAllFinished() const
	{
		for (const std::unique_ptr<ThreadManager>& manager : m_threadManagers | std::views::values)
		{
			manager->WaitUntilFinished();
		}
	}

	void ThreadPool::ReleaseQueue(const char* queueName)
	{
		m_threadManagers.erase(queueName);
	}
}
