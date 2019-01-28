#include "../include/TaskScheduler.h"

TaskScheduler::TaskScheduler()
{
	// -1 because of the main thread
	int Threads = std::thread::hardware_concurrency() - 1;

	StartThreads(Threads);
}

TaskScheduler::TaskScheduler(int _Threads)
{
	StartThreads(_Threads);
}

void TaskScheduler::Running()
{
	Task tempTask;

	while( m_Running || m_Tasks.size() > 0 )
	{

		if( m_Tasks.size() > 0 )
		{
			// Lock
			m_Lock.lock();

			// Take top task off of the pile
			tempTask = m_Tasks.front();
			
			m_Tasks.erase( m_Tasks.begin() );;

			// Unlock
			m_Lock.unlock();
			// Run the task
			tempTask.Run();

			// Put the task on the completed tasks pile
			// Not sure this is correct as the vector is a vector of pointers rather than objects
			// May need a Task copy constructor
			m_CompletedTasks.push_back(tempTask);
		}
	}
}

void TaskScheduler::StartThreads(int _Threads)
{
	for(size_t i = 0; i < _Threads; i++)
	{
		m_Threads.push_back( std::thread( &TaskScheduler::Running, this ) );
	}
}



void TaskScheduler::StopThreads()
{
	for( size_t i = 0; i < m_Threads.size(); ++i)
	{
		m_Threads[i].join();
	}
}

TaskScheduler::~TaskScheduler()
{
	StopThreads();
}


int TaskScheduler::SubmitTask( std::function<void()> _Function )
{
	m_Tasks.push_back( Task( _Function, m_TaskCounter) );
	m_TaskCounter++;
}


std::string TaskScheduler::CheckProgress(int _TaskID)
{
	for(size_t i = 0; i < m_Tasks.size(); ++i)
	{
		if(m_Tasks[i].GetID() == _TaskID)
		{
			return "New";
		}
	}

	for(size_t i = 0; i < m_CompletedTasks.size(); ++i)
	{
		if(m_CompletedTasks[i].GetID() == _TaskID)
		{
			return "Completed";
		}
	}

	return "In Progress";
}
