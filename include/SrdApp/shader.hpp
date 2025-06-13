#define CL_TARGET_OPENCL_VERSION 300

#pragma once

#include <CL/cl.h>
#include <iostream>
#include <fstream>

#include "SrdApp/null.hpp"

namespace Srd::App {
    class Shader
    {
    public:
        static inline const std::string ShaderPath = "../SrdFile/Shader/";

        enum class BuildCode {
            BuildSuccess,
            AlreadyBuild,
            BuildFailed
        };

    public:   
        Shader(const std::string& iPath, const cl_device_id& iDevice, const cl_context& iContext);
        ~Shader();

        BuildCode Build();

        inline const cl_kernel& GetFragmentKernel() const { return _fragmentShaderKernel; };
        inline const cl_kernel& GetVertexKernel() const { return _vertexShaderKernel; };

    private:
        std::string _data;

        cl_device_id _device;
        cl_context _context;

        cl_program _program = Null::Program;

        cl_kernel _fragmentShaderKernel = Null::Kernel;
        cl_kernel _vertexShaderKernel   = Null::Kernel;
    };
};