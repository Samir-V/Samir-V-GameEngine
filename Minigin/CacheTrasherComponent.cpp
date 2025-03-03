#include "CacheTrasherComponent.h"

#include <algorithm>
#include <chrono>
#include <numeric>

#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl2.h"
#include "implot.h"

dae::CacheTrasherComponent::CacheTrasherComponent(GameObject* ownerPtr): ComponentBase(ownerPtr)
{
}

void dae::CacheTrasherComponent::Update(float)
{
}

void dae::CacheTrasherComponent::LateUpdate(float)
{
	
}

void dae::CacheTrasherComponent::Render() const
{
}

void dae::CacheTrasherComponent::RenderImGui()
{
	ShowEx1UI();
	ShowEx2UI();
}


void dae::CacheTrasherComponent::SetLocalPosition(float x, float y)
{
	m_LocalTransform.SetPosition(x, y, 0.0f);
}

void dae::CacheTrasherComponent::ShowEx1UI()
{
	ImGui::Begin("Ex1");

	ImGui::PushItemWidth(150);
	ImGui::InputInt("##samples", &m_NrOfSamplesEx1, 0, 0, ImGuiInputTextFlags_CharsDecimal);
	ImGui::PopItemWidth();
	ImGui::SameLine();
	if (ImGui::SmallButton("-"))
	{
		m_NrOfSamplesEx1 = std::max(1, --m_NrOfSamplesEx1);
	}
	ImGui::SameLine();
	if (ImGui::SmallButton("+"))
	{
		++m_NrOfSamplesEx1;
	}
	ImGui::SameLine();
	ImGui::Text("# samples");

	if (ImGui::Button("Trash the cache"))
	{
		m_Ex1Measurements.clear();
		m_IsEx1GraphActive = CalculateEx1(m_NrOfSamplesEx1, m_Ex1Measurements);
	}

	if (m_IsEx1GraphActive && ImPlot::BeginPlot("Ex1 Graph"))
	{
		ImPlot::PlotLine("Time Measurements", m_Ex1Measurements.data(), static_cast<int>(m_Ex1Measurements.size()));
		ImPlot::EndPlot();
	}

	ImGui::End();
}


bool dae::CacheTrasherComponent::CalculateEx1(int samples, std::vector<float>& measurements)
{
	constexpr int bufferSize = 67108864;

	const auto buffer = new int[bufferSize];

	for (int stepsize = 1; stepsize <= 1024; stepsize *= 2)
	{
		std::vector<float> times{};
		times.reserve(samples);

		for (int timeMeasure = 0; timeMeasure < samples; ++timeMeasure)
		{
			auto start = std::chrono::high_resolution_clock::now();
			for (int index = 0; index < bufferSize; index += stepsize)
			{
				buffer[index] *= 2;
			}

			auto end = std::chrono::high_resolution_clock::now();
			auto elapsedTime = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

			times.emplace_back(static_cast<float>(elapsedTime));
		}

		times.erase(std::ranges::max_element(times));
		times.erase(std::ranges::min_element(times));

		const float sumTimes = std::accumulate(times.begin(), times.end(), 0.0f);

		measurements.emplace_back(sumTimes / static_cast<float>(times.size()) / 1000.0f);
	}

	delete[] buffer;

	return true;
}

void dae::CacheTrasherComponent::ShowEx2UI()
{
	ImGui::Begin("Ex2");

	ImGui::PushItemWidth(150);
	ImGui::InputInt("##samples", &m_NrOfSamplesEx2, 0, 0, ImGuiInputTextFlags_CharsDecimal);
	ImGui::PopItemWidth();
	ImGui::SameLine();
	if (ImGui::SmallButton("-"))
	{
		m_NrOfSamplesEx2 = std::max(1, --m_NrOfSamplesEx2);
	}
	ImGui::SameLine();
	if (ImGui::SmallButton("+"))
	{
		++m_NrOfSamplesEx2;
	}
	ImGui::SameLine();
	ImGui::Text("# samples");

	if (ImGui::Button("Trash the cache with GameObject3D"))
	{
		m_Ex2Measurements.clear();
		m_IsEx2GraphActive = CalculateEx2(m_NrOfSamplesEx2, m_Ex2Measurements);
	}

	if (ImGui::Button("Trash the cache with GameObject3DAlt"))
	{
		m_Ex2AltMeasurements.clear();
		m_IsEx2AltGraphActive = CalculateEx2Alt(m_NrOfSamplesEx2, m_Ex2AltMeasurements);
	}

	if (m_IsEx2GraphActive && ImPlot::BeginPlot("Ex2 Graph Regular"))
	{
		ImPlot::PlotLine("Time Measurements", m_Ex2Measurements.data(), static_cast<int>(m_Ex2Measurements.size()));
		ImPlot::EndPlot();
	}

	if (m_IsEx2AltGraphActive && ImPlot::BeginPlot("Ex2 Graph Alt"))
	{
		ImPlot::PlotLine("Time Measurements", m_Ex2AltMeasurements.data(), static_cast<int>(m_Ex2AltMeasurements.size()));
		ImPlot::EndPlot();
	}

	if (m_IsEx2GraphActive && m_IsEx2AltGraphActive && ImPlot::BeginPlot("Ex2 Graph Combined"))
	{
		ImPlot::PlotLine("Time Measurements Reg", m_Ex2Measurements.data(), static_cast<int>(m_Ex2Measurements.size()));
		ImPlot::PlotLine("Time Measurements Alt", m_Ex2AltMeasurements.data(), static_cast<int>(m_Ex2AltMeasurements.size()));
		ImPlot::EndPlot();
	}

	ImGui::End();
}

bool dae::CacheTrasherComponent::CalculateEx2(int samples, std::vector<float>& measurements)
{
	constexpr int bufferSize = 16777216; // 2^24

	const auto buffer = new GameObject3D[bufferSize];

	for (int stepsize = 1; stepsize <= 1024; stepsize *= 2)
	{
		std::vector<float> times{};
		times.reserve(samples);

		for (int timeMeasure = 0; timeMeasure < samples; ++timeMeasure)
		{
			auto start = std::chrono::high_resolution_clock::now();
			for (int index = 0; index < bufferSize; index += stepsize)
			{
				buffer[index].ID *= 2;
			}

			auto end = std::chrono::high_resolution_clock::now();
			auto elapsedTime = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

			times.emplace_back(static_cast<float>(elapsedTime));
		}

		times.erase(std::ranges::max_element(times));
		times.erase(std::ranges::min_element(times));

		const float sumTimes = std::accumulate(times.begin(), times.end(), 0.0f);

		measurements.emplace_back(sumTimes / static_cast<float>(times.size()) / 1000.0f);
	}

	delete[] buffer;

	return true;
}

bool dae::CacheTrasherComponent::CalculateEx2Alt(int samples, std::vector<float>& measurements)
{
	constexpr int bufferSize = 16777216; // 2^24

	const auto buffer = new GameObject3DAlt[bufferSize];

	for (int stepsize = 1; stepsize <= 1024; stepsize *= 2)
	{
		std::vector<float> times{};
		times.reserve(samples);

		for (int timeMeasure = 0; timeMeasure < samples; ++timeMeasure)
		{
			auto start = std::chrono::high_resolution_clock::now();
			for (int index = 0; index < bufferSize; index += stepsize)
			{
				buffer[index].ID *= 2;
			}

			auto end = std::chrono::high_resolution_clock::now();
			auto elapsedTime = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

			times.emplace_back(static_cast<float>(elapsedTime));
		}

		times.erase(std::ranges::max_element(times));
		times.erase(std::ranges::min_element(times));

		const float sumTimes = std::accumulate(times.begin(), times.end(), 0.0f);

		measurements.emplace_back(sumTimes / static_cast<float>(times.size()) / 1000.0f);
	}

	delete[] buffer;

	return true;
}


