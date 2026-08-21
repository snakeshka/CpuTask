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
		auto it = std::lower_bound(m_Events.begin(), m_Events.end(), targetTick, [](const Event& e, uint32_t tick) {
			return e.TargetTick > tick;
		});
		m_Events.insert(it, Event{targetTick, std::move(handler)});
	}

	void ProcessEvents(uint32_t currentTick)
	{
		while (!m_Events.empty() && m_Events.back().TargetTick <= currentTick) {
			auto event = std::move(m_Events.back());
			m_Events.pop_back();
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