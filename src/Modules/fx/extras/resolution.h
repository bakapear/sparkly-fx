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
};

inline ResModule g_resmodule;