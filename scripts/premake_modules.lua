function AddOpenGL()
    defines { "MODULE_OpenGL" }

    postbuildcommands
    { 
    }
    links
    { 
        "opengl32.lib"
    }
   
    filter {}
end

function AddGLM()
    defines { "MODULE_GLM" }

	includedirs "$(SolutionDir)/thirdparty/glm/"

    filter {}
end

function AddGLFW()
    defines { "MODULE_GLFW" }

	includedirs "$(SolutionDir)/thirdparty/glfw/include/"
    libdirs	"$(SolutionDir)/thirdparty/glfw/lib-vc2019/"
  
    postbuildcommands
    { 
        "{COPY} \"$(SolutionDir)thirdparty\\glfw\\lib-vc2019\\glfw3.dll\" \"$(OutDir)\""
    }
    links
    { 
        "glfw3.lib"
    }

    filter {}
end

function AddGLEW()
    defines { "MODULE_GLEW" }

	includedirs "$(SolutionDir)/thirdparty/glew/include/"
    libdirs	"$(SolutionDir)/thirdparty/glew/lib/"
  
    postbuildcommands
    { 
    }
    links
    { 
        "glew32.lib"
    }

    filter {}
end