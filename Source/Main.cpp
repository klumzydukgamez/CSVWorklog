/* Copyright (c) 2026 Klumzy Duk Gamez. All rights reserved.
 * This file is apart of CSVWorklog.
 * See LICENSE.md for licnse details. */

#include <SDL3/SDL.h>
#include <nlohmann/json.hpp>
#include <imgui.h>
#include <backends/imgui_impl_sdl3.h>
#include <backends/imgui_impl_sdlrenderer3.h>

#include <iostream>
#include <sstream>
#include <string>
#include <chrono>
#include <optional>
#include <format>
#include <vector>
#include <ctime>

#include <iostream>
#include <string>
#include <optional>
#include <chrono>
#include <ctime>
#include <nlohmann/json.hpp>

void GetLocalTime(std::time_t t, std::tm& ot) {
#if defined(_WIN32)
	localtime_s(&ot, &t);
#else
	localtime_r(&t, &ot);
#endif
}

std::optional<std::chrono::system_clock::time_point> ParseDateTime(const nlohmann::json& j, const std::string& key) {
	if (!j.contains(key) || j[key].is_null())
		return std::nullopt;

	try {
		std::string str = j[key].get<std::string>();

		std::tm tm{};
		if (sscanf_s(str.c_str(), "%d-%d-%d : %d:%d", &tm.tm_year, &tm.tm_mon, &tm.tm_mday, &tm.tm_hour, &tm.tm_min) == 5) {

			tm.tm_year -= 1900;
			tm.tm_mon -= 1;
			tm.tm_isdst = -1;

			std::time_t lt = std::mktime(&tm);
			if (lt != -1) {
				return std::chrono::system_clock::from_time_t(lt);
			}
		}
	} catch (...) {
	}
	return std::nullopt;
}

nlohmann::json DateTimeToJson(const std::optional<std::chrono::system_clock::time_point>& tp) {
	if (!tp.has_value())
		return nullptr;

	std::time_t t = std::chrono::system_clock::to_time_t(*tp);

	std::tm tm{};
	GetLocalTime(t, tm);

	char buf[32];
	if (std::strftime(buf, sizeof(buf), "%Y-%m-%d : %H:%M", &tm) > 0) {
		return std::string(buf);
	}
	return nullptr;
}

std::string DateTimeToOptionalString(const std::optional<std::chrono::system_clock::time_point>& tp) {
	return tp.has_value() ? DateTimeToJson(*tp).get<std::string>() : "null";
}

std::string DateTimeToString(const std::optional<std::chrono::system_clock::time_point>& tp) {
	if (!tp.has_value()) {
		return "NA";
	}

	std::time_t t = std::chrono::system_clock::to_time_t(*tp);
	std::tm tm{};
	GetLocalTime(t, tm);

	char buf[64];
	if (std::strftime(buf, sizeof(buf), "%d/%m/%Y : %H:%M", &tm) > 0) {
		return std::string(buf);
	}
	return "NA";
}

std::string CalculateDurationString(const std::optional<std::chrono::system_clock::time_point>& start, const std::optional<std::chrono::system_clock::time_point>& end) {
	if (!start.has_value() || !end.has_value() || *end < *start) {
		return "00:00";
	}

	auto duration = std::chrono::duration_cast<std::chrono::minutes>(*end - *start);
	auto total_minutes = duration.count();

	int hours = static_cast<int>(total_minutes / 60);
	int minutes = static_cast<int>(total_minutes % 60);

	return std::format("{:02d}:{:02d}", hours, minutes);
}

struct Data {
	std::vector<std::string> descriptions = {"NA"};
	std::string description = "NA";
	std::optional<std::chrono::system_clock::time_point> start = std::nullopt;
	std::optional<std::chrono::system_clock::time_point> end = std::nullopt;

	Data() = default;
	~Data() = default;

	explicit Data(const nlohmann::json& j) {
		Data _default;
		descriptions = j.value("descriptions", _default.descriptions);
		description = j.value("description", _default.description);
		start = ParseDateTime(j, "start");
		end = ParseDateTime(j, "end");
	}

	explicit operator nlohmann::json() const {
		nlohmann::json j;
		j["descriptions"] = descriptions;
		j["description"] = description;
		j["start"] = DateTimeToJson(start);
		j["end"] = DateTimeToJson(end);
		return j;
	}
};

std::string DataToCSV(const Data& data) {
	std::string desc = data.description;
	size_t pos = 0;
	while ((pos = desc.find("\"", pos)) != std::string::npos) {
		desc.insert(pos, "\"");
		pos += 2;
	}

	std::string start = DateTimeToString(data.start);
	std::string end = DateTimeToString(data.end);
	std::string duration = CalculateDurationString(data.start, data.end);

	return std::format("{}\t{}\t\"{}\"\t{}\t{}\n", start, end, duration, desc, "NA");
}

int main(int argc, char* argv[]) {
	SDL_Window* window = nullptr;
	SDL_Renderer* renderer = nullptr;

	SDL_Event event;
	bool running = true;

	SDL_Storage* storage = nullptr;
	Data data;
	const char* DATA_PATH = "data.json";

	if (!SDL_Init(SDL_INIT_VIDEO)) {
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "CSVWorklog", "SDL_Init failed.", nullptr);
		goto lbl_cleanup;
	}

	window = SDL_CreateWindow("CSVWorklog", 320, 180, 0);
	if (!window) {
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "CSVWorklog", "SDL_CreateWindow failed.", nullptr);
		goto lbl_cleanup;
	}

	renderer = SDL_CreateRenderer(window, nullptr);
	if (!renderer) {
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "CSVWorklog", "SDL_CreateRenderer failed.", nullptr);
		goto lbl_cleanup;
	}

	storage = SDL_OpenUserStorage("Klumzy Duk Gamez", "CSVWorklog", 0);
	if (!storage) {
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "CSVWorklog", "SDL_OpenUserStorage failed.", nullptr);
		goto lbl_cleanup;
	}

	{
		std::vector<char> buffer;
		nlohmann::json j;
		Uint64 fs = 0;
		if (!SDL_GetStorageFileSize(storage, DATA_PATH, &fs)) {
			SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_WARNING, "CSVWorklog", "SDL_GetStorageFileSize failed.", nullptr);
			goto lbl_data_defaults;
		}

		buffer.resize(fs);
		if (!SDL_ReadStorageFile(storage, DATA_PATH, buffer.data(), fs)) {
			SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_WARNING, "CSVWorklog", "SDL_ReadStorageFile failed.", nullptr);
			goto lbl_data_defaults;
		}

		j = nlohmann::json::parse(buffer.begin(), buffer.end(), nullptr, false);
		if (j.is_discarded()) {
			SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_WARNING, "CSVWorklog", "nlohmann::json::parse failed.", nullptr);
			goto lbl_data_defaults;
		}

		data = Data{j};

	lbl_data_defaults:
	}

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();

	ImGui_ImplSDL3_InitForSDLRenderer(window, renderer);
	ImGui_ImplSDLRenderer3_Init(renderer);

	while (running) {
		while (SDL_PollEvent(&event)) {
			ImGui_ImplSDL3_ProcessEvent(&event);
			switch (event.type) {
				case SDL_EVENT_QUIT:
					running = false;
					break;
				case SDL_EVENT_KEY_DOWN:
					switch (event.key.scancode) {
						case SDL_SCANCODE_ESCAPE:
							running = false;
							break;
						default:
							break;
					}
					break;
				default:
					break;
			}
		}

		ImGui_ImplSDLRenderer3_NewFrame();
		ImGui_ImplSDL3_NewFrame();
		ImGui::NewFrame();

		{
			bool open = true;
			const ImGuiViewport* vp = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(vp->WorkPos);
			ImGui::SetNextWindowSize(vp->WorkSize);

			ImGuiWindowFlags wf = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBringToFrontOnFocus;
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10.0f, 10.0f));
			ImGui::Begin("CSVWorklog", &open, wf);

			ImGui::TextDisabled("ACTIVE TASK");
			ImGui::TextWrapped("Start: %s", DateTimeToString(data.start).c_str());
			ImGui::TextWrapped("End: %s", DateTimeToString(data.end).c_str());
			ImGui::TextWrapped("Duration: %s", CalculateDurationString(data.start, data.end).c_str());
			ImGui::TextWrapped("Description: %s", data.description.c_str());
			ImGui::Separator();
			ImGui::Spacing();

			int _description = -1;
			for (size_t i = 0; i < data.descriptions.size(); ++i) {
				ImGui::PushID(static_cast<int>(i));

				if (ImGui::Button(data.descriptions[i].c_str())) {
					_description = static_cast<int>(i);
				}

				if (i + 1 < data.descriptions.size()) {
					ImGui::SameLine();
				}

				ImGui::PopID();
			}
			if (_description != -1) {
				data.description = data.descriptions[_description];
			}
			ImGui::Separator();
			ImGui::Spacing();

			float bw = (ImGui::GetContentRegionAvail().x - (ImGui::GetStyle().ItemSpacing.x * 3)) / 4.0f;

			if (ImGui::Button("Start", ImVec2(bw, 0))) {
				data.start = std::chrono::system_clock::now();
			}
			ImGui::SameLine();
			if (ImGui::Button("End", ImVec2(bw, 0))) {
				data.end = std::chrono::system_clock::now();
			}
			ImGui::SameLine();
			if (ImGui::Button("Copy", ImVec2(bw, 0))) {
				if (!SDL_SetClipboardText(DataToCSV(data).c_str())) {
					SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_WARNING, "CSVWorklog", "SDL_SetClipboardText failed.", nullptr);
				}
			}
			ImGui::SameLine();

			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.4f, 0.15f, 0.15f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.6f, 0.2f, 0.2f, 1.0f));
			if (ImGui::Button("Reset", ImVec2(bw, 0))) {
				auto descriptions = data.descriptions;
				data = {};
				data.descriptions = descriptions;
			}
			ImGui::PopStyleColor(2);

			ImGui::End();
			ImGui::PopStyleVar();
		}

		ImGui::Render();

		SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
		SDL_RenderClear(renderer);

		ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer);
		SDL_RenderPresent(renderer);
	}

	{
		nlohmann::json j = static_cast<nlohmann::json>(data);
		std::string str;

		try {
			str = j.dump(4);
		} catch (...) {
			SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "CSVWorklog", "nlohmann::json::dump failed.", nullptr);
			goto lbl_failed_dump;
		}

		if (!SDL_WriteStorageFile(storage, DATA_PATH, str.data(), str.size())) {
			SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "CSVWorklog", "SDL_WriteStorageFile failed.", nullptr);
		}

	lbl_failed_dump:
	}

lbl_cleanup:
	ImGui_ImplSDLRenderer3_Shutdown();
	ImGui_ImplSDL3_Shutdown();
	ImGui::DestroyContext();

	if (storage)
		SDL_CloseStorage(storage);
	if (renderer)
		SDL_DestroyRenderer(renderer);
	if (window)
		SDL_DestroyWindow(window);
	SDL_Quit();

	if (!storage)
		return 1;
	if (!renderer)
		return 1;
	if (!window)
		return 1;
	return 0;
}
