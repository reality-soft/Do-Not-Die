workspace "Do-Not-Die"
	architecture "x86_64"
	toolset "v143"
	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

project "Do-Not-Die"
	location "Do-Not-Die"
	kind "WindowedApp"
	language "C++"

	targetdir("../output/bin/" .. outputdir .. "/%{prj.name}")
	objdir("../output/bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"premake5.lua",
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/HLSL/**.hlsl"
	} 

	includedirs
	{
		"../Game-Engine/Engine/src/Actors",
		"../Game-Engine/Engine/src/Components",
		"../Game-Engine/Engine/src/Core",
		"../Game-Engine/Engine/src/DataTypes",
		"../Game-Engine/Engine/src/Headers",
		"../Game-Engine/Engine/src/Physics",
		"../Game-Engine/Engine/src/ResourceTypes",
		"../Game-Engine/Engine/src/Systems",
		"../Game-Engine/Engine/src/Tools",
		"../Game-Engine/Engine/src/UI",
		"../Game-Engine/Engine/src/World",
		"../Game-Engine/Engine/src/Event",
		"../Game-Engine/Engine/src/Scene",
		"../Game-Engine/Engine/src/Sound",
		"../Game-Engine/Engine/src/Input",
		"../Game-Engine/Engine/src/Managers",
		"../Game-Engine/Engine/src",
		"../Game-Engine/Engine/vendor/spdlog/include",
		"Do-Not-Die/src/Actors",
		"Do-Not-Die/src/FX",
		"Do-Not-Die/src/GUI",
		"Do-Not-Die/src/Scenes",
		"Do-Not-Die/src/UI",
		"Do-Not-Die/src/UI/InGameScene",
		"Do-Not-Die/src/UI/LoadingScene",
		"Do-Not-Die/src/UI/StartScene",
		"../SDK/DirectXTK/include",
		"../SDK/FBXSDK/include",
		"../SDK/FMOD/include",
		"../SDK/IMGUI/include",
		"../SDK/RP3D/include"
	}

	libdirs
	{
		"../output/bin/Debug-windows-x86_64/Engine/",
		"../SDK/DirectXTK/lib",
		"../SDK/IMGUI/lib",
		"../SDK/RP3D/lib"
	}

	links
	{
		"Engine",
		"libfbxsdk-md",
		"libxml2-md",
		"zlib-md",
		"fmod_vc",
		"fmodL_vc",
		"ImGui_Win32_Dx11_D"
	}

	filter "files:**VS.hlsl"
		shadertype "Vertex"
		shaderentry "VS"
	    shadermodel "5.0"

	filter "files:**PS.hlsl"
	    shadertype "Pixel"
		shaderentry "PS"
	    shadermodel "5.0"

	filter "files:**GS.hlsl"
	    shadertype "Geometry"
		shaderentry "GS"
	    shadermodel "5.0"

	filter "files:**CS.hlsl"
	    shadertype "Compute"
		shaderentry "CS"
	    shadermodel "5.0"

	filter "system:windows"
		cppdialect "C++20"
		staticruntime "Off"
		systemversion "latest"

		defines
		{
			"PLATFORM_WINDOWS"
		}

	filter "configurations:Debug"
		defines "_DEBUG"
		symbols "On"
		runtime "Debug"

		libdirs
		{
			"../SDK/FBXSDK/lib/debug",
			"../SDK/FMOD/lib/debug",
			"../output/bin/Debug-windows-x86_64/Engine/",
		}

		prebuildcommands
		{
			"copy \"..\\..\\output\\bin\\Debug-windows-x86_64\\Engine\\*.dll\" \"..\\..\\output\\bin\\Debug-windows-x86_64\\Do-Not-Die\\*.dll\"",
			"copy \"..\\..\\SDK\\FMOD\\lib\\debug\\*.dll\" \"..\\..\\output\\bin\\Debug-windows-x86_64\\Do-Not-Die\\*.dll\""
		}

		postbuildcommands
		{
			"copy \"..\\..\\output\\bin\\Debug-windows-x86_64\\Do-Not-Die\\*.cso\" \"..\\..\\Contents\\Shader\\*.cso\"",
		}

	filter "configurations:Release"
		defines "_RELEASE"
		optimize "On"
		runtime "Release"

		libdirs
		{
			"../SDK/FBXSDK/lib/release",
			"../SDK/FMOD/lib/release",
			"../output/bin/Release-windows-x86_64/Engine/",
		}

		prebuildcommands
		{
			"copy \"..\\..\\output\\bin\\Release-windows-x86_64\\Engine\\*.dll\" \"..\\..\\output\\bin\\Release-windows-x86_64\\Do-Not-Die\\*.dll\"",
			"copy \"..\\..\\SDK\\FBXSDK\\lib\\release\\*.dll\" \"..\\..\\output\\bin\\Release-windows-x86_64\\Do-Not-Die\\*.dll\""
		}

		postbuildcommands
		{
			"copy \"..\\..\\output\\bin\\Release-windows-x86_64\\Do-Not-Die\\*.cso\" \"..\\..\\Contents\\Shader\\*.cso\"",
		}

	filter "configurations:Dist"
		defines "_DIST"
		optimize "On"
		runtime "Release"

		libdirs
		{
			"../SDK/FBXSDK/lib/release",
			"../SDK/FMOD/lib/release",
			"../output/bin/Dist-windows-x86_64/Engine/",
		}

		prebuildcommands
		{
			"copy \"..\\..\\output\\bin\\Dist-windows-x86_64\\Engine\\*.dll\" \"..\\..\\output\\bin\\Dist-windows-x86_64\\Do-Not-Die\\*.dll\"",
			"copy \"..\\..\\SDK\\FBXSDK\\lib\\release\\*.dll\" \"..\\..\\output\\bin\\Release-windows-x86_64\\Do-Not-Die\\*.dll\""
		}

		postbuildcommands
		{
			"copy \"..\\..\\output\\bin\\Dist-windows-x86_64\\Do-Not-Die\\*.cso\" \"..\\..\\Contents\\Shader\\*.cso\"",
		}
