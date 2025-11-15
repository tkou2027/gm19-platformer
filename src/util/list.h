#pragma once
#include "platform/log.h"
#include "platform/assert.h"
namespace foo
{
	class List {
	public:

		void Initialize(int initial_capacity = 4)
		{
			m_capacity = initial_capacity;
			m_count = 0;
			delete[]m_data; // can initialize multiple times
			m_data = new void* [m_capacity];
			for (int i = 0; i < m_capacity; i++)
			{
				m_data[i] = nullptr;
			}
		}
		void Finalize()
		{
			delete[]m_data;
			m_data = nullptr;
			m_capacity = 0;
			m_count = 0;
		}
		void Add(void* element)
		{
			FOO_ASSERT(m_count <= m_capacity - 1, "List capacity overflow! Max: %d", m_capacity);
			m_data[m_count] = element;
			m_count++;
		}
		void Remove(int index)
		{
			// ! will not preserve order
			if (index >= m_count)
			{
				FOO_LOG("Remove index overflow!");
				return;
			}
			// move last element to current index
			// decrease count by 1
			m_data[index] = m_data[m_count - 1];
			m_data[m_count - 1] = nullptr;
			--m_count;
		}
		void RemoveOrderedPend(int index)
		{
			// preserve order
			// some pointer will be nullptr until calling RemoveOrderedApply
			if (index >= m_count)
			{
				FOO_LOG("Remove index overflow!");
				return;
			}
			m_data[index] = nullptr;
		}
		void RemoveOrderedApply()
		{
			// remove nullptr
			int tail{ 0 }, previous_count{ m_count };
			for (int i = 0; i < previous_count; i++)
			{
				if (!m_data[i])
				{
					--m_count;
					continue;
				}
				m_data[tail++] = m_data[i];
			}
			for (int i = m_count; i < previous_count; i++)
			{
				m_data[i] = nullptr;
			}
		}
		void Clear()
		{
			for (int i = 0; i < m_count; i++)
			{
				m_data[i] = nullptr;
			}
			m_count = 0;
		}
		void ClearDelete()
		{
			for (int i = 0; i < m_count; i++)
			{
				delete m_data[i];
				m_data[i] = nullptr;
			}
			m_count = 0;
		}
		~List()
		{
			Finalize();
		}
		int GetCount() const
		{
			return m_count;
		}
		void* operator[](size_t i)
		{
			if (i >= m_count)
			{
				return nullptr;
			}
			return m_data[i];
		}
		const void* operator[](size_t i) const
		{
			if (i >= m_count)
			{
				return nullptr;
			}
			return m_data[i];
		}
		bool Empty() const
		{
			return m_count == 0;
		}
		bool Full() const
		{
			return m_count >= m_capacity - 1;
		}
	private:
		void** m_data{ nullptr };
		int m_capacity{ 0 };
		int m_count{ 0 };
	};
}