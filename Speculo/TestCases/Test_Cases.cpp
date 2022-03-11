#include "SpeculoPCH.h"
#include "../Serialization/Serializer_Text.h"
#include "../Serialization/Serializer_Binary.h"
#include "Material.h"
#include "Math.h"
#include "RTTI/Reflect.hpp"
#include "Delegates/Signal.hpp"

using namespace Speculo;

template <typename T>
void Print(const T& t)
{
    std::cout << Speculo::Resolve<T>()->GetName() << ": " << t << std::endl;
}

Material CreateDummyMaterial(const std::string& resourcePath)
{
    Material dummyMaterial(resourcePath);

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

void TextSerializationTest()
{
    Speculo::Serializer_Text testCase(Speculo::Serializer_Operation_Type::Serialization, "../UnitTests/Feature_Tests.yml", "Feature_Tests");
    testCase.SerializeProperty("Player_Speed", 15);
    testCase.SerializeProperty("Player_Health", 255);
    testCase.SerializeProperty("Player_Location", Speculo::Vector2(3, 5));
    testCase.EndSerialization();
}

void TextDeserializationTest()
{
    int playerSpeed;
    Speculo::Vector2 locationVector;

    Speculo::Serializer_Text testCases(Speculo::Serializer_Operation_Type::Deserialization, "../UnitTests/Feature_Tests.yml", "Feature_Tests");
    testCases.DeserializeProperty("Player_Speed", &playerSpeed);
    testCases.DeserializeProperty("Player_Location", &locationVector);
    testCases.EndDeserialization();

    std::cout << playerSpeed << "\n" << locationVector.x << "\n" << locationVector.y << "\n";
}

void BinarySerializationTest()
{
    Speculo::Serializer_Binary binaryCaseWrite(Speculo::Serializer_Operation_Type::Serialization, "../UnitTests/BinaryTest", "Binary_Test");
    binaryCaseWrite.SerializeProperty(6);
    binaryCaseWrite.SerializeProperty(5.5f);
    binaryCaseWrite.EndSerialization();
}

void BinaryDeserializationTest()
{
    Speculo::Serializer_Binary binaryCaseRead(Speculo::Serializer_Operation_Type::Deserialization, "../UnitTests/BinaryTest", "Binary_Test");
    int value;
    binaryCaseRead.DeserializeProperty(&value);
    std::cout << value << "\n" << binaryCaseRead.DeserializePropertyAs<float>() << "\n";
    binaryCaseRead.EndDeserialization();
}

void MaterialSerializationTest()
{
    // ===========================================================
    Material dummyMaterial = CreateDummyMaterial("../UnitTests/Material_Test"); /// Automatically populate extension.

    // We can simply nest this inside the material class as well.
    Speculo::Serializer_Text materialSerialization(Speculo::Serializer_Operation_Type::Serialization, dummyMaterial.GetResourcePath(), "Material");

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

void MaterialDeserializationTest()
{
    Speculo::Serializer_Text materialDeserialization(Speculo::Serializer_Operation_Type::Deserialization, "../UnitTests/Material_Test", "Material");
    std::cout << materialDeserialization.DeserializePropertyAs<std::string>("Material_Color_Path") << "\n";
    materialDeserialization.EndDeserialization();
}

SIGNAL_RETURN_ONE_PARAM(MultiDelegate, int, double);
MultiDelegate multiDelegate;

class MyClass
{
public:
    MyClass(int i) : i(i)
    {
        m_Connections.push_back(multiDelegate.Bind(*this, &MyClass::MemberFunction));
        m_Connections.push_back(multiDelegate.Bind(*this, &MyClass::ConstMemberFunction));
        m_Connections.push_back(multiDelegate.Bind(&MyClass::StaticMemberFunction, 3));
        m_Connections.push_back(multiDelegate.Bind(*this, 2));
        m_Connections.push_back(multiDelegate.Bind(*static_cast<const MyClass*>(this), 1));
    }

    ~MyClass()
    {
        // Disconnect from 
        for (auto& connection : m_Connections)
        {   
            std::cout << "Disconnected Connection!" << "\n";
            connection.Disconnect();
        }
    }

    int MemberFunction(double d) { std::cout << "in member function" << std::endl; return int(++i * d); }
    int ConstMemberFunction(double d) const { std::cout << "in const member function" << std::endl; return int(i * d); }
    int operator()(double d) { std::cout << "in overloaded function call operator" << std::endl; return (int)(++i + d); }
    int operator()(double d) const { std::cout << "in const overloaded function call operator" << std::endl; return (int)(i + d); }
    static int StaticMemberFunction(double d) { std::cout << "in static member function" << std::endl; return int(10 + d); }

private:
    int i;
    std::vector<Connection> m_Connections; // A vector of delegate connections.
};

int main(int argc, int argv[])
{
    {
        MyClass myClass(10);
        multiDelegate(5.5);
    }

    multiDelegate(1.2);

    int i = 10;

    auto lambda1 = [&i](double) { std::cout << "In Lambda" << "\n"; return 10; };
    multiDelegate.Bind(lambda1, 3);

    multiDelegate.Bind([i](int d) mutable -> int { std::cout << "In Temporary" << std::endl; return 10; }, 2);
    multiDelegate(1.2);
    multiDelegate([](int i) -> bool { return i == 10; }, 1.20);

    // ===========================================================================

    Speculo::Reflect<int>("int").AddMemberFunction(&Print<int>, "Print");
    auto a = Speculo::Resolve("int");
    Speculo::Resolve("int")->GetMemberFunction("Print")->Invoke(a, 5);

    BinarySerializationTest();
    BinaryDeserializationTest();

    TextSerializationTest();
    TextDeserializationTest();

    MaterialSerializationTest();
    MaterialDeserializationTest();
}







