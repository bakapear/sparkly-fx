#pragma once
#include "SDK/cdll_int.h"

class IBaseClientDLL018
{
public:
	virtual int	Init(CreateInterfaceFn appSystemFactory, CreateInterfaceFn physicsFactory, CGlobalVarsBase* pGlobals) = 0;
	virtual void			PostInit() = 0;
	virtual void			Shutdown(void) = 0;
	virtual bool			ReplayInit(CreateInterfaceFn replayFactory) = 0;
	virtual bool			ReplayPostInit() = 0;
	virtual void			LevelInitPreEntity(char const* pMapName) = 0;
	virtual void			LevelInitPostEntity() = 0;
	virtual void			LevelShutdown(void) = 0;
	virtual ClientClass* GetAllClasses(void) = 0;
	virtual int				HudVidInit(void) = 0;
	virtual void			HudProcessInput(bool bActive) = 0;
	virtual void			HudUpdate(bool bActive) = 0;
	virtual void			HudReset(void) = 0;
	virtual void			HudText(const char* message) = 0;
	virtual void _unk14() = 0;
	virtual void _unk15() = 0;
	virtual void _unk16() = 0;
	virtual void _unk17() = 0;
	virtual void _unk18() = 0;
	virtual void _unk19() = 0;
	virtual bool			IN_IsKeyDown(const char* name, bool& isdown) = 0;
	virtual int				IN_KeyEvent(int eventcode, ButtonCode_t keynum, const char* pszCurrentBinding) = 0;
	virtual void			CreateMove(int sequence_number, float input_sample_frametime, bool active) = 0;
	virtual void			ExtraMouseSample(float frametime, bool active) = 0;
	virtual bool			WriteUsercmdDeltaToBuffer(bf_write* buf, int from, int to, bool isnewcommand) = 0;
	virtual void			EncodeUserCmdToBuffer(bf_write& buf, int slot) = 0;
	virtual void			DecodeUserCmdFromBuffer(bf_read& buf, int slot) = 0;
	virtual void			View_Render(vrect_t* rect) = 0;
	virtual void			RenderView(const CViewSetup& view, int nClearFlags, int whatToDraw) = 0;
	virtual void			View_Fade(ScreenFade_t* pSF) = 0;
	virtual void			SetCrosshairAngle(const QAngle& angle) = 0;
	virtual void			InitSprite(CEngineSprite* pSprite, const char* loadname) = 0;
	virtual void			ShutdownSprite(CEngineSprite* pSprite) = 0;
	virtual int				GetSpriteSize(void) const = 0;
	virtual void			VoiceStatus(int entindex, qboolean bTalking) = 0;
	virtual void _unk35() = 0;
	virtual void			InstallStringTableCallback(char const* tableName) = 0;
	virtual void			FrameStageNotify(ClientFrameStage_t curStage) = 0;
	virtual bool			DispatchUserMessage(int msg_type, bf_read& msg_data) = 0;
	virtual CSaveRestoreData* SaveInit(int size) = 0;
	virtual void			SaveWriteFields(CSaveRestoreData*, const char*, void*, datamap_t*, typedescription_t*, int) = 0;
	virtual void			SaveReadFields(CSaveRestoreData*, const char*, void*, datamap_t*, typedescription_t*, int) = 0;
	virtual void			PreSave(CSaveRestoreData*) = 0;
	virtual void			Save(CSaveRestoreData*) = 0;
	virtual void			WriteSaveHeaders(CSaveRestoreData*) = 0;
	virtual void			ReadRestoreHeaders(CSaveRestoreData*) = 0;
	virtual void			Restore(CSaveRestoreData*, bool) = 0;
	virtual void			DispatchOnRestore() = 0;
	virtual CStandardRecvProxies* GetStandardRecvProxies() = 0;
	virtual void _unk49() = 0;
	virtual void _unk50() = 0;
	virtual void _unk51() = 0;
	virtual void _unk52() = 0;
	virtual void _unk53() = 0;
	virtual void _unk54() = 0;
	virtual void _unk55() = 0;
	virtual void _unk56() = 0;
	virtual void _unk57() = 0;
	virtual void _unk58() = 0;
	virtual void _unk59() = 0;
	virtual void _unk60() = 0;
	virtual void _unk61() = 0;
	virtual void _unk62() = 0;
	virtual void _unk63() = 0;
	virtual void _unk64() = 0;
	virtual int GetScreenWidth() = 0;
	virtual int GetScreenHeight() = 0;
	virtual void _unk67() = 0;
	virtual void _unk68() = 0;
	virtual void _unk69() = 0;
	virtual bool			GetPlayerView(CViewSetup& playerView) = 0;
};