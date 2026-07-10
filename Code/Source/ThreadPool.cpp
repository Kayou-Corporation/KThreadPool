#include "ThreadPool.hpp"

#include <ranges>
#include <iostream>
#include <syncstream>

#include <Preprocessor.hpp>

namespace Kayou
{
	ThreadPool::~ThreadPool()
	{
		m_threadManagers.clear();
	}

	void ThreadPool::InitQueue(std::string_view queueName, uint8_t numThreads)
	{
		m_threadManagers.try_emplace(queueName, std::make_unique<ThreadManager>(queueName, numThreads));
	}

	void ThreadPool::EnqueueTask(std::string_view queueName, std::function<void()> task, Priority priority)
	{
		std::unique_lock<std::mutex> lock(m_mutex);
		KUNLIKELY if (!m_threadManagers.contains(queueName))
		{
			lock.unlock();
#if defined (PERMISSIVE_EXCEPTIONS)
			task();
#endif

			if (m_areTooManyErrors)
				return;

			std::cerr << "\033[1;31m[KThreads error] Wrong queue name when enqueueing: " << queueName << "\033[0m\n";

			++m_nbErrors;
			if (m_nbErrors >= m_maxErrorsCount)
			{
				std::cerr << "\033[1;33mToo many errors displayed, following errors for this issue won't be displayed\033[0m\n";
				m_areTooManyErrors = true;
			}
			return;
		}

		KUNLIKELY if (m_areTooManyErrors)
		{
			m_nbErrors = 0u;
			m_areTooManyErrors = false;
		}

		m_threadManagers.at(queueName)->Enqueue(std::move(task), priority);
	}

	void ThreadPool::WaitUntilQueueFinished(std::string_view queueName) const
	{
		KUNLIKELY if (!m_threadManagers.contains(queueName))
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

	void ThreadPool::ReleaseQueue(std::string_view queueName)
	{
		m_threadManagers.erase(queueName);
	}
}
