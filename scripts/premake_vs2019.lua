require "premake_modules"

local ProjectName = "RaycastedMetaBalls"

function AddSourceFiles(DirectoryPath)
    files
    { 
        ("../" .. DirectoryPath .. "/**.h"),
		("../" .. DirectoryPath .. "/**.hpp"),
        ("../" .. DirectoryPath .. "/**.cpp"),
        ("../" .. DirectoryPath .. "/**.inl"),
		("../" .. DirectoryPath .. "/**.c"),
    }
end

workspace (ProjectName)
	location "../"
	basedir "../"
	language "C++"
	configurations {"Debug", "Release"}
	platforms {"x64"}
	warnings "default"
	characterset ("MBCS")
	rtti "Off"
	toolset "v142"
	cppdialect "C++latest"
	--flags {"FatalWarnings"}
	
	filter { "configurations:Debug" }
		runtime "Debug"
		defines { "_DEBUG" }
		symbols "On"
		optimize "Off"
		debugdir "$(SolutionDir)"
		
	filter { "configurations:Release" }
	 	runtime "Release"
		defines { "_RELEASE", "NDEBUG" }
	 	symbols "Off"
	 	optimize "Full"

project (ProjectName)
	location ("../")
	targetdir ("../Build/" .. ProjectName .. "$(Configuration)_$(Platform)")
	objdir "!../Build/Intermediate/$(ProjectName)_$(Configuration)_$(Platform)"
	kind "ConsoleApp"

	AddSourceFiles("src")
	includedirs { "$(ProjectDir)" }
	
	disablewarnings
	{
        "4100", -- unreferenced formal paramter
		"4189"  -- local variable initalized but not referenced
	}

	AddOpenGL()
	AddGLFW()
	AddGLM()
	AddGLEW()

	filter "files:**/ThirdParty/**.*"
		flags "NoPCH"
		disablewarnings { "4100" }
