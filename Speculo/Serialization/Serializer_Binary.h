#pragma once

/*

template <typename T, typename = typename std::enable_if<!std::is_same<T, std::string>::value>::type>
void SerializeProperty(const std::string& propertyName, T value)
{
    m_ActiveEmitter << YAML::Key << propertyName << YAML::Value << value;
}

*/