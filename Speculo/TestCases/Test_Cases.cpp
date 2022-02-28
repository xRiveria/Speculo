#include "SpeculoPCH.h"
#include "../Serialization/Serializer_Text.h"
#include "Material.h"

Material CreateDummyMaterial(const std::string& resourcePath)
{
    Material dummyMaterial("../UnitTests/Material_Test");

    dummyMaterial.SetTexturePath(Material_Property::Material_Color, "RandomColorPath.jpg");
    dummyMaterial.SetTexturePath(Material_Property::Material_Metallic, "RandomMetallicPath.jpg");
    dummyMaterial.SetTexturePath(Material_Property::Material_Normal, "RandomNormalPath.jpg");
    dummyMaterial.SetTexturePath(Material_Property::Material_Roughness, "RandomRoughnessPath.jpg");

    dummyMaterial.SetTextureProperty(Material_Property::Material_Color, 5.0f);
    dummyMaterial.SetTextureProperty(Material_Property::Material_Metallic, 55.0f);
    dummyMaterial.SetTextureProperty(Material_Property::Material_Normal, 75.0f);
    dummyMaterial.SetTextureProperty(Material_Property::Material_Roughness, 15.0f);

    return dummyMaterial;
}

int main(int argc, int argv[])
{
    Speculo::Serializer_Text testCase(Speculo::OperationType::Serialization, "../UnitTests/Feature_Tests.yml", "Feature_Tests");
    testCase.SerializeProperty("Player_Speed", 15);
    testCase.SerializeProperty("Player_Health", 255);
    testCase.EndSerialization();

    Speculo::Serializer_Text testCases(Speculo::OperationType::Deserialization, "../UnitTests/Feature_Tests.yml", "Feature_Tests");
    int version;
    testCases.DeserializeProperty("Player_Health", &version);

    std::cout << version << "\n";

    // ===========================================================
    Material dummyMaterial = CreateDummyMaterial("../UnitTests/Material.yml"); /// Automatically populate extension.

    // We can simply nest this inside the material class as well.
    Speculo::Serializer_Text materialSerialization(Speculo::OperationType::Serialization, dummyMaterial.GetResourcePath(), "Material");
    materialSerialization.SerializeProperty("Material_Color_Path", dummyMaterial.GetTexturePath(Material_Property::Material_Color));
    materialSerialization.SerializeProperty("Material_Color_Multiplier", dummyMaterial.GetTextureProperty(Material_Property::Material_Color));
    materialSerialization.SerializeProperty("Material_Metallic_Path", dummyMaterial.GetTexturePath(Material_Property::Material_Metallic));
    materialSerialization.SerializeProperty("Material_Metallic_Multiplier", dummyMaterial.GetTextureProperty(Material_Property::Material_Metallic));
    materialSerialization.SerializeProperty("Material_Normal_Path", dummyMaterial.GetTexturePath(Material_Property::Material_Normal));
    materialSerialization.SerializeProperty("Material_Normal_Multiplier", dummyMaterial.GetTextureProperty(Material_Property::Material_Normal));
    materialSerialization.SerializeProperty("Material_Roughness_Path", dummyMaterial.GetTexturePath(Material_Property::Material_Roughness));
    materialSerialization.SerializeProperty("Material_Roughness_Multiplier", dummyMaterial.GetTextureProperty(Material_Property::Material_Roughness));
    materialSerialization.EndSerialization();
}