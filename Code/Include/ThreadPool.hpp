#pragma once
#include <functional>
#include <memory>
#include <unordered_map>
#include <string_view>

#include "ThreadManager.hpp"

namespace Kayou
{
	class ThreadPool
	{
	public:
		ThreadPool() = default;
		~ThreadPool();

		void InitQueue(std::string_view queueName, uint8_t numThreads);
		void EnqueueTask(std::string_view queueName, std::function<void()> task, Priority priority = Priority::High);
		void WaitUntilQueueFinished(std::string_view queueName) const;
		void WaitUntilAllFinished() const;
		void ReleaseQueue(std::string_view queueName);

	private:
		std::mutex m_mutex;
		std::unordered_map<std::string_view, std::unique_ptr<ThreadManager>> m_threadManagers{};
		uint32_t m_maxErrorsCount = 30u;
		uint32_t m_nbErrors = 0u;
		bool m_areTooManyErrors = false;
	};
}