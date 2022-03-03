workspace "Speculo"
    architecture "x64"
    targetdir "Build"
    
    configurations
    {
        "Debug",
        "Release"
    }

outputDirectory = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

project "Speculo"
    location "Speculo"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    staticruntime "Off"

    targetdir ("Binaries/Bin" .. outputDirectory .. "/%{prj.name}")
    objdir ("Binaries/Bin-Int" .. outputDirectory .. "/%{prj.name}")

    pchheader "SpeculoPCH.h"
    pchsource "%{prj.name}/Core/SpeculoPCH.cpp"

    files
    {
        "%{prj.name}/**.h",
        "%{prj.name}/**.cpp"
    }

    includedirs
    {
        "%{prj.name}/",
        "%{prj.name}/Core/",
        "%{wks.location}/Dependencies/"
    }

    filter "configurations:Debug"
        symbols "On"

        libdirs
        {
            "%{wks.location}/Dependencies/Libraries/Debug/"
        }

        links
        {
            "yaml-cppd.lib"
        }

    filter "configurations:Release"
        optimize "On"

        libdirs
        {
            "%{wks.location}/Dependencies/Libraries/Release/"
        }

        links
        {
            "yaml-cpp.lib"
        }
