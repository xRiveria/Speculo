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

    libdirs
    {
        "%{wks.location}/Dependencies/Libraries/"
    }

    links
    {
        "yaml-cppd.lib"
    }

    filter "configurations:Debug"
        symbols "On"

    filter "configurations:Release"
        symbols "On"
