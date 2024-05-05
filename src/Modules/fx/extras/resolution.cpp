// https://github.com/jooonior/SourceRes
#include "resolution.h"
#include "../mainwindow.h"

#include <stdio.h> 

#include <imgui.h>

#include <Base/Interfaces.h>
#include <SDK/convar.h>

#include <nlohmann/json.hpp>
#include <Helper/json.h>
#include <Modules/fx/configmodule.h>

const int MAX_MODE_LIST = 512;

static int* GetInternalModeCount(const vmode_s *modeList, int count)
{
	// Mode count should be right before the mode array.
	int *modeCount = (int *)modeList - 1;

	// Verify that the value matches.
	if (*modeCount != count)
	{
		throw "Failed to access video mode count. Aborting.";
	}

	return modeCount;
}

/**
 * Register a new video mode with specific resolution.
 * Does nothing if a mode with the same resolution is already present.
 *
 * @return If a new video mode was added.
 */
static bool RegisterResolution(int width, int height)
{
	// Get modes.
	vmode_s *modeList;
	int count;
	Interfaces::engine->GetVideoModes(count, modeList);

	// Return if a mode with desired resolution is already present.
	for (auto i = 0; i < count; i++)
	{
		vmode_s mode = modeList[i];
		if (mode.width == width && mode.height == height)
		{
			return false;
		}
	}

	if (count >= MAX_MODE_LIST)
	{
		throw "Video mode array is full. Can't add any more modes.";
	}

	int *modeCount = GetInternalModeCount(modeList, count);

	// These seem to have the same value across all modes.
	int refreshRate = modeList[0].refreshRate;
	int bitsPerPixel = modeList[0].bpp;

	// Shift modes with higher resolution.
	for (; count; count--)
	{
		vmode_s mode = modeList[count - 1];
		if (mode.width < width || (mode.width == width && mode.height < height))
			break;

		modeList[count] = modeList[count - 1];
	}

	// Insert new mode.
	modeList[count].width = width;
	modeList[count].height = height;
	modeList[count].bpp = bitsPerPixel;
	modeList[count].refreshRate = refreshRate;

	// Update the internal counter.
	*modeCount += 1;

	return true;
}

static void SetResolution(int width, int height)
{
	// Use mat_setvideomode to change the resolution.
	// It will change to a registered resolution that is the closest to given dimensions.
	char command[256];
	sprintf(command, "mat_setvideomode %i %i 1", width, height);
	Interfaces::engine->ClientCmd_Unrestricted(command);
}

static ConCommand sf_set_resolution("sf_set_resolution",
    [](const CCommand& cmd) {
        if (cmd.ArgC() < 3)
            Interfaces::engine->ClientCmd_Unrestricted("echo sf_set_resolution <width> <height>");
        else 
		{
			int width = atoi(cmd.Arg(1));
			int height = atoi(cmd.Arg(2));

			RegisterResolution(width, height);
			SetResolution(width, height);
			
			char command[256];
			sprintf(command, "echo Set Resolution to %ix%i.", width, height);
			Interfaces::engine->ClientCmd_Unrestricted(command);
		}
    },
    "Usage: sf_set_resolution <width> <height>\n"
    "Set the current exact windowed resolution\n"
);


void ResModule::StartListening()
{
    MainWindow::OnTabBar.Listen(&ResModule::OnTabBar, this);
	ConfigModule::OnConfigSave.Listen(&ResModule::OnConfigSave, this);
    ConfigModule::OnConfigLoad.Listen(&ResModule::OnConfigLoad, this);
}

int ResModule::OnTabBar()
{
	if (!ImGui::BeginTabItem("Resolution"))
		return 0;

	ImGui::Checkbox("Set on startup", &m_startup);

	ImGui::InputInt("Width", &m_width, 0);
	ImGui::InputInt("Height", &m_height, 0);

    if (ImGui::Button("Set Resolution"))
	{
		RegisterResolution(m_width, m_height);
		SetResolution(m_width, m_height);
	}

    ImGui::EndTabItem();
    return 0;
}

int ResModule::OnConfigSave()
{
    nlohmann::json j = {
        {"m_width", m_width},
		{"m_height", m_height},
		{"m_startup", m_startup}
    };
    ConfigModule::GetOutput().emplace("Resolution", std::move(j));
    return 0;
}

int ResModule::OnConfigLoad()
{
    const nlohmann::json* j = Helper::FromJson(ConfigModule::GetInput(), "Resolution");
    if (!j) return 0;
    
    Helper::FromJson(j, "m_width", m_width);
	Helper::FromJson(j, "m_height", m_height);
	Helper::FromJson(j, "m_startup", m_startup);

	if (m_startup) 
	{
		RegisterResolution(m_width, m_height);
		SetResolution(m_width, m_height);
	}

    return 0;
}