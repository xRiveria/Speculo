#pragma once
#include "yaml-cpp/yaml.h"
#include "TestCases/Math.h"

// Addition of custom classes for integration with YAML.

namespace YAML
{
    template<>
    struct convert<Speculo::Vector2>
    {
        static Node encode(const Speculo::Vector2& rhs)
        {
            Node node;
            node.push_back(rhs.x);
            node.push_back(rhs.y);
            return node;
        }

        static bool decode(const Node& node, Speculo::Vector2& rhs)
        {
            if (!node.IsSequence() || node.size() != 2)
            {
                return false;
            }

            rhs.x = node[0].as<float>();
            rhs.y = node[1].as<float>();
            return true;
        }
    };

    template<>
    struct convert<Speculo::Vector3>
    {
        static Node encode(const Speculo::Vector3& rhs)
        {
            Node node;
            node.push_back(rhs.x);
            node.push_back(rhs.y);
            node.push_back(rhs.z);
            return node;
        }

        static bool decode(const Node& node, Speculo::Vector3& rhs)
        {
            if (!node.IsSequence() || node.size() != 3)
            {
                return false;
            }

            rhs.x = node[0].as<float>();
            rhs.y = node[1].as<float>();
            rhs.z = node[2].as<float>();
            return true;
        }
    };
}

namespace Speculo
{
    inline YAML::Emitter& operator<<(YAML::Emitter& outStream, const Speculo::Vector2& targetVector)
    {
        outStream << YAML::Flow;
        outStream << YAML::BeginSeq << targetVector.x << targetVector.y << YAML::EndSeq;

        return outStream;
    }

    inline YAML::Emitter& operator<<(YAML::Emitter& outStream, const Speculo::Vector3& targetVector)
    {
        outStream << YAML::Flow;
        outStream << YAML::BeginSeq << targetVector.x << targetVector.y << targetVector.z << YAML::EndSeq;

        return outStream;
    }
}