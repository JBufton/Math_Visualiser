#ifndef TASK_H__
#define TASK_H__

#include <functional>

class Task
{
	public:
		Task() = default;
		Task( std::function<void(void)> _Function, int _ID );
		~Task() = default;

		void Run();
		int GetID(){ return m_ID; }

	private:
		std::function<void()> m_Function;
		int m_ID;
};


#endif
