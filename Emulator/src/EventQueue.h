#pragma once

#include <cstdint>
#include <functional>
#include <vector>
#include <algorithm>

namespace Emulator {

struct Event
{
	uint32_t TargetTick;
	std::function<uint32_t(uint32_t currentTick)> Handler;
};

class EventQueue
{
public:
	void Schedule(uint32_t targetTick, std::function<uint32_t(uint32_t currentTick)> handler)
	{
		m_Events.push_back(Event{targetTick, std::move(handler)});
		std::sort(m_Events.begin(), m_Events.end(), [](const Event& a, const Event& b) {
			return a.TargetTick < b.TargetTick;
		});
	}

	void ProcessEvents(uint32_t currentTick)
	{
		while (!m_Events.empty() && m_Events.front().TargetTick <= currentTick) {
			auto event = std::move(m_Events.front());
			m_Events.erase(m_Events.begin());

			uint32_t delta = event.Handler(currentTick);
			if (delta > 0) {
				Schedule(currentTick + delta, std::move(event.Handler));
			}
		}
	}

	bool Empty() const
	{
		return m_Events.empty();
	}

private:
	std::vector<Event> m_Events;
};

}