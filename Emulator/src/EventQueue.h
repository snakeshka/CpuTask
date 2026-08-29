#pragma once

#include <cstdint>
#include <vector>
#include <algorithm>

namespace Emulator {

class Event {
public:
	virtual ~Event() = default;
	virtual uint32_t OnEvent(uint32_t currentTick) = 0;
};

struct ScheduledEvent {
	uint32_t TargetTick;
	Event* TargetEvent;
};

class EventQueue
{
public:
	void ScheduleDelta(uint32_t delayTicks, Event* event) 
	{
		Schedule(m_CurrentTick + delayTicks, event);
	}

	void Schedule(uint32_t targetTick, Event* event) 
	{
		auto it = std::lower_bound(m_Events.begin(), m_Events.end(), targetTick, [](const ScheduledEvent& e, uint32_t tick) {
			return e.TargetTick > tick;
		});
		m_Events.insert(it, ScheduledEvent{targetTick, event});
	}

	void ProcessEvents(uint32_t currentTick)
	{
		m_CurrentTick = currentTick;
		while (!m_Events.empty() && m_Events.back().TargetTick <= currentTick) {
			auto scheduled = m_Events.back();
			m_Events.pop_back();
			uint32_t delta = scheduled.TargetEvent->OnEvent(currentTick);
			if (delta > 0) {
				Schedule(currentTick + delta, scheduled.TargetEvent);
			}
		}
	}

	bool Empty() const
	{
		return m_Events.empty();
	}

private:
	std::vector<ScheduledEvent> m_Events;
	uint32_t m_CurrentTick = 0;
};

}