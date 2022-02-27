#include "../Serialization/Serializer_Text.h"
#include <iostream>

int main(int argc, int argv[])
{
    Speculo::Serializer_Text testCase(Speculo::OperationType::Serialization, "../TestCase.yml", "Test_Case_File");
    testCase.SerializeProperty("Player_Speed", 25.0);
    testCase.SerializeProperty("Player_Health", 255);
    testCase.EndSerialization();

    Speculo::Serializer_Text testCases(Speculo::OperationType::Deserialization, "../TestCase.yml", "Test_Case_File");
    int version;
    testCases.DeserializeProperty("Version_Revision", &version);

    std::cout << version << "\n";
}