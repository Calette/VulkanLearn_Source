#pragma once
#include <chrono>
#include "Common/PaletteCommon.h"
#include "Common/Singleton.h"

namespace Palette
{
	class Time : public Singleton<Time>
	{
	public:
		void Initial()
		{
			m_StartTime = std::chrono::high_resolution_clock::now();
			m_PreTime = 0.0f;
		}

		void Update() 
		{
			static auto startTime = std::chrono::high_resolution_clock::now();
			auto currentTime = std::chrono::high_resolution_clock::now();
			m_Time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
			m_DeltaTime = m_Time - m_PreTime;
			m_PreTime = m_Time;
		}

		GET(float, Time)
		GET(float, DeltaTime)

	private:
		std::chrono::steady_clock::time_point m_StartTime;
		float m_PreTime;
	};
}