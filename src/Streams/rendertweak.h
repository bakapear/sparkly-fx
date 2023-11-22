#pragma once
#include <unordered_set>
#include <string_view>
#include <vector>
#include <memory>
#include <array>

enum class FilterChoice : int { ALL, WHITELIST, BLACKLIST, _COUNT };
class CBaseEntity;
class ClientClass;

/// @brief Configurable variables for one part of the rendering process.
/// Each subclass will typically provide settings for a specific, hooked render function.
class RenderTweak
{
public:
    using Ptr = std::shared_ptr<RenderTweak>;
    using ConstPtr = std::shared_ptr<const RenderTweak>;
    
    virtual ~RenderTweak() {}
    
    /// @return A user-friendly class name
    virtual const char* GetName() const = 0;
    virtual std::shared_ptr<RenderTweak> Clone() const = 0;
    /// @brief Display the controls to edit the tweak
    virtual void OnMenu() = 0;

    /// @brief An array of every RenderTweak subclass.
    /// Useful for displaying all types.
    static const std::vector<ConstPtr> default_tweaks;
};

class MiscTweak : public RenderTweak
{
public:
    const char* GetName() const override { return "Miscellaneous"; }
    std::shared_ptr<RenderTweak> Clone() const override {
        return std::make_shared<MiscTweak>(*this);
    }
    void OnMenu() override;

    bool viewmodel_enabled = true; // Affects r_drawviewmodel
    bool hud_enabled = true; // Affects cl_drawhud
    bool props_enabled = true; // Affects r_drawstaticprops
    bool shadows_enabled = true; // Affects r_shadows
    bool skybox_enabled = true; // Affects r_3dsky and r_skybox
    bool decals_enabled = true; // Calls r_cleardecals. Will affect future frames.
    bool particles_enabled = true; // Affects r_drawparticles
    bool misc_effects_enabled = true; // (TF2: Affects glow_outline_effect_enable)
};

class EntityFilterTweak : public RenderTweak
{
public:
    enum class MaterialChoice : int { NORMAL, INVISIBLE, CUSTOM, _COUNT = CUSTOM };

    const char* GetName() const override { return "Entities"; }
    std::shared_ptr<RenderTweak> Clone() const override {
        return std::make_shared<EntityFilterTweak>(*this);
    }
    void OnMenu() override;
    
    /// @return `true` if an entity should be rendered differently than normal
    bool IsEntityAffected(CBaseEntity* entity) const;
    /// @return `true` if the effect will make an entity invisible
    bool IsEffectInvisible() const;

    static const char* MaterialChoiceName(MaterialChoice value);

    /// @brief Chooses whether entities should be filtered, and which ones
    FilterChoice filter_choice = FilterChoice::ALL;
    /// @brief An effect given to each entity
    MaterialChoice render_effect = MaterialChoice::NORMAL;
    std::shared_ptr<class CustomMaterial> custom_material = nullptr;
    /// @brief A color multiply given to each entity
    std::array<float, 4> color_multiply = { 1,1,1,1 };

    /// @brief A list of entity classes to filter
    std::unordered_set<ClientClass*> classes;
    bool filter_player = false;
    bool filter_weapon = false;
    bool filter_wearable = false;
    bool filter_projectile = false;
};

class MaterialTweak : public RenderTweak
{
public:
    static const std::array<const char*, 27> TEXTURE_GROUPS;

    const char* GetName() const override { return "Materials"; }
    std::shared_ptr<RenderTweak> Clone() const override {
        return std::make_shared<MaterialTweak>(*this);
    }
    void OnMenu() override;

    bool IsMaterialAffected(class IMaterial* material) const;

    /// @brief A color multiply given to each material
    std::array<float, 4> color_multiply = { 1,1,1,1 };
    /// @brief A fixed array containing `true` or `false` for each texture group
    std::array<bool, TEXTURE_GROUPS.size()> groups = {0};
    /// @brief This will affect props specially, since typical material overrides doesn't work on them.
    bool props = false;
    FilterChoice filter_choice = FilterChoice::ALL;
};

class CameraTweak : public RenderTweak
{
public:
    const char* GetName() const override { return "Camera"; }
    std::shared_ptr<RenderTweak> Clone() const override {
        return std::make_shared<CameraTweak>(*this);
    }
    void OnMenu() override;

    float fov = 90;
    bool fov_override = false;
};

class FogTweak : public RenderTweak
{
public:
    const char* GetName() const override { return "Fog"; }
    std::shared_ptr<RenderTweak> Clone() const override {
        return std::make_shared<FogTweak>(*this);
    }
    void OnMenu() override;

    bool fog_enabled;
    int fog_start;
    int fog_end;
    std::array<float, 3> fog_color;

    bool skyfog_enabled;
    int skyfog_start;
    int skyfog_end;
    std::array<float, 3> skyfog_color;
};

inline const std::vector<RenderTweak::ConstPtr> RenderTweak::default_tweaks = {
    std::make_shared<MiscTweak>(),
    std::make_shared<EntityFilterTweak>(),
    std::make_shared<MaterialTweak>(),
    std::make_shared<CameraTweak>(),
    std::make_shared<FogTweak>(),
};
