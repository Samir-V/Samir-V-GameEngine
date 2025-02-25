#include "CacheTrasherComponent.h"

#include <algorithm>
#include <chrono>
#include <numeric>
#include <vector>

#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl2.h"

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
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();
	ShowEx1UI();
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void dae::CacheTrasherComponent::SetLocalPosition(float x, float y)
{
	m_LocalTransform.SetPosition(x, y, 0.0f);
}

void dae::CacheTrasherComponent::ShowEx1UI() const
{
	static int sampleAmount = 10;
	static std::vector<float> cachedMeasurements;
	static bool calculationIsActive;

	ImGui::Begin("Ex1");

	ImGui::PushItemWidth(150);
	ImGui::InputInt("##samples", &sampleAmount, 0, 0, ImGuiInputTextFlags_CharsDecimal);
	ImGui::PopItemWidth();
	ImGui::SameLine();
	if (ImGui::SmallButton("-"))
	{
		sampleAmount = std::max(1, --sampleAmount);
	}
	ImGui::SameLine();
	if (ImGui::SmallButton("+"))
	{
		++sampleAmount;
	}
	ImGui::SameLine();
	ImGui::Text("# samples");

	if (ImGui::Button("Trash the cache"))
	{
		calculationIsActive = CalculateEx1(sampleAmount, cachedMeasurements);
	}

	if (calculationIsActive)
	{
		ImGui::PlotLines("Ex1 Graph", cachedMeasurements.data(), static_cast<int>(cachedMeasurements.size()), 0, nullptr, FLT_MAX, FLT_MAX, ImVec2(200, 200));
	}

	ImGui::End();
}


bool dae::CacheTrasherComponent::CalculateEx1(int samples, std::vector<float>& measurements) const
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

