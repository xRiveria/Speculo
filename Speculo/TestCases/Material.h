#pragma once
#include <string>
#include <unordered_map>

// Dummy material structure from Aurora.

enum class Material_Property : uint32_t
{
    Material_Color,
    Material_Normal,
    Material_Roughness,
    Material_Metallic
};

class SpeculoResource
{
public:
    const std::string GetResourcePath() const { return m_Resource_Path; }

protected:
    SpeculoResource(const std::string& resourcePath) : m_Resource_Path(resourcePath) { }

protected:
    std::string m_Resource_Path = "";
};

class Material : public SpeculoResource
{
public:
    Material() = default;
    Material(const std::string& resourcePath) : SpeculoResource(resourcePath) { }
    
    const std::string GetTexturePath(Material_Property materialProperty) { return m_Textures[materialProperty]; }
    const float GetTextureProperty(Material_Property materialProperty) { return m_Texture_Properties[materialProperty]; }

    void SetTexturePath(Material_Property materialProperty, const std::string& texturePath) { m_Textures[materialProperty] = texturePath; }
    void SetTextureProperty(Material_Property materialProperty, float value) { m_Texture_Properties[materialProperty] = value; }

private:
    std::unordered_map<Material_Property, std::string> m_Textures;
    std::unordered_map<Material_Property, float> m_Texture_Properties;
};