#include "shaders.h"
#include <mutex>
#include <cassert>
#include <fstream>
#include <iostream>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <Helper/dxerr.h>
#include <Helper/str.h>
#include <Base/Base.h>
#include <Hooks/OverlayHook.h>
#include <d3d9.h>

namespace Shader
{

static std::mutex g_pixel_shaders_mutex;
static std::vector<PixelShader::Ptr> g_pixel_shaders;

/// @brief Get the loaded shaders without doing any loading. Just getting.
static Helper::LockedRef<std::vector<PixelShader::Ptr>> GetLoadedShadersPrivate() {
    return Helper::LockedRef(g_pixel_shaders, g_pixel_shaders_mutex);
}

bool PixelShader::LoadAllShaders()
{
    static bool is_loaded = false;
    auto loaded = GetLoadedShadersPrivate();
    IDirect3DDevice9* device = g_hk_overlay.Device();

    if (is_loaded || !device)
        return is_loaded;
    
    for (std::shared_ptr<PixelShader> shader : all_types)
    {
        if (shader->Load(device))
            loaded->emplace_back(std::move(shader));
    }

    is_loaded = true;
    return true;
}

Helper::LockedRef<const std::vector<PixelShader::ConstPtr>> PixelShader::GetLoadedShaders()
{
    LoadAllShaders();

    // Cast PixelShader::Ptr to PixelShader::ConstPtr :)
    return Helper::LockedRef(reinterpret_cast<const std::vector<PixelShader::ConstPtr>&>(g_pixel_shaders), g_pixel_shaders_mutex);
}

std::shared_ptr<PixelShader> PixelShader::CreateFromJson(const nlohmann::json* json)
{
    const nlohmann::json* filename = Helper::FromJson(json, "m_filename");
    if (!filename || !filename->is_string())
        return nullptr;
    
    auto loaded_shaders = GetLoadedShaders();
    for (const PixelShader::ConstPtr& shader : *loaded_shaders)
    {
        if (!Helper::stricmp(shader->GetFileName(), filename->get_ref<const std::string&>()))
        {
            PixelShader::Ptr new_instance = shader->NewInstance();
            new_instance->FromJson(json);
            return new_instance;
        }
    }

    return nullptr;
}

bool PixelShader::Load(IDirect3DDevice9* device)
{
    if (m_ptr)
        return true;

    std::filesystem::path path = Base::GetModuleDir() / "shaders" / m_filename;
    std::ifstream file(path, std::ios::binary);
    if (!file)
    {
        std::cout << "Failed to open shader: " << path << std::endl;
        return false;
    }

    std::vector<char> file_data(
        std::istreambuf_iterator(file),
        {});

    if (!file)
    {
        std::cout << "Failed to read shader: " << path << std::endl;
        return false;
    }

    file.close();
    
    HRESULT result = device->CreatePixelShader((const DWORD*)file_data.data(), &m_ptr);
    if (FAILED(result))
    {
        std::cout << "Failed to load pixel shader (";
        const wchar_t* err_string = DXGetErrorStringW(result);
        for (const wchar_t* ch = err_string; *ch != 0; ++ch)
            std::cout << (char)*ch;
        std::cout << "): " << path << std::endl;
        return false;
    }
    
    return true;
}

PixelShader::PixelShader(const char* filename, const char* displayname, const char* desc)
    : m_filename(filename), m_displayname(displayname), m_desc(desc) {}
PixelShader::PixelShader(const PixelShader& other)
    : m_filename(other.m_filename), m_displayname(other.m_displayname), m_desc(other.m_desc), m_ptr(other.m_ptr)
{
    if (m_ptr)
        m_ptr->AddRef();
}
PixelShader::~PixelShader()
{
    if (m_ptr)
        m_ptr->Release();
}
nlohmann::json PixelShader::ToJson() const
{
    nlohmann::json j = SubclassToJson();
    j.emplace("m_filename", m_filename);
    return j;
}

IDirect3DDevice9* PixelShader::GetDevice()
{
    IDirect3DDevice9* device = nullptr;
    if (m_ptr)
        m_ptr->GetDevice(&device);
    return device;
}

void DepthLinear::ApplyConstants(const float* inv_projection, IDirect3DTexture9* depth)
{
    const float consts[4] = {m_near, m_far, (float)m_spherical_correction, 0};
    GetDevice()->SetTexture(0, depth);
    GetDevice()->SetPixelShaderConstantF(0, inv_projection, 4);
    GetDevice()->SetPixelShaderConstantF(4, consts, 1);
}

void DepthLogarithm::ApplyConstants(const float* inv_projection, IDirect3DTexture9* depth)
{
    const float consts[4] = {m_near, m_far, (float)m_spherical_correction, m_near_precision};
    GetDevice()->SetTexture(0, depth);
    GetDevice()->SetPixelShaderConstantF(0, inv_projection, 4);
    GetDevice()->SetPixelShaderConstantF(4, consts, 1);
}

void DepthToRgb::ApplyConstants(const float* inv_projection, IDirect3DTexture9* depth)
{
    GetDevice()->SetTexture(0, depth);
    GetDevice()->SetPixelShaderConstantF(0, inv_projection, 4);
}

void NormalToRgb::ApplyConstants(const float* inv_projection, IDirect3DTexture9* depth)
{
    GetDevice()->SetTexture(0, depth);
    GetDevice()->SetPixelShaderConstantF(0, inv_projection, 4);
}

}