#pragma once
#include <Modules/BaseModule.h>

typedef struct vmode_s
{
	int width;
	int height;
	int bpp;
	int refreshRate;
} vmode_t;

class ResModule : public CModule
{
protected:
    void StartListening();

private:
    int OnTabBar();
	int OnConfigLoad();
	int OnConfigSave();

	int m_width;
	int m_height;
	bool m_startup;
};

inline ResModule g_resmodule;