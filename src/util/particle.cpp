#include "particle.h"
#include "platform//assert.h"

namespace foo
{
	void ParticleList::Add(const Particle& particle)
	{
		FOO_ASSERT(m_count < MAX_PARTICLES - 1, "Particle overflow");
		m_data[m_count] = particle;
		m_count++;
	}

	void ParticleList::Remove(int index)
	{
		// ! will not preserve order
		// move last element to current index
		// decrease count by 1
		m_data[index] = m_data[m_count - 1];
		--m_count;
	}

	int ParticleList::GetCount() const
	{
		return m_count;
	}
}