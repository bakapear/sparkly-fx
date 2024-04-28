// https://github.com/jooonior/SourceRes

#pragma once

#include <stdio.h> 

#include <Modules/Menu.h>
#include <imgui.h>

#include <Base/Interfaces.h>
#include <SDK/convar.h>

typedef struct vmode_s
{
	int width;
	int height;
	int bpp;
	int refreshRate;
} vmode_t;

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

class ResModule : public CModule
{
public:
	ResModule() { }
	void StartListening() override {
        Listen(EVENT_MENU, [this] { return OnMenu(); });
    }

private:
	int OnMenu()
    {
        if (ImGui::CollapsingHeader("Resolution"))
        {
			static int res[2];

			ImGui::InputInt2("", res);
			ImGui::SameLine();

            if (ImGui::Button("Set Resolution"))
            {
            	RegisterResolution(res[0], res[1]);
				SetResolution(res[0], res[1]);
            }
        }
        return 0;
    }
};

static ResModule g_set_resolution;