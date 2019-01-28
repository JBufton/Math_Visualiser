#include "../include/Task.h"

Task::Task( std::function<void(void)> _Function, int _ID)
{
	m_Function = _Function;
	m_ID = _ID;
}

void Task::Run()
{

	m_Function();

}
