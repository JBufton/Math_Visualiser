#ifndef TASKSCHEDULER_H__
#define TASKSCHEDULER_H__

#include <vector>
#include <thread>
#include <mutex>
#include "Task.h"

class TaskScheduler
{
	public:
		TaskScheduler();
		TaskScheduler(int _Threads);
		~TaskScheduler();

		int SubmitTask( std::function<void()> _Function );

		std::string CheckProgress( int _TaskID );

	private:
		std::mutex m_Lock;

		std::vector<std::thread> m_Threads;

		std::vector<Task> m_Tasks;

		std::vector<Task> m_CompletedTasks;

		bool m_Running = false;

		int m_TaskCounter;

		void StartThreads(int _Threads);
		void Running();
		void StopThreads();
};

#endif
