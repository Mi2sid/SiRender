#include "SrdApp/shader.hpp"
#include <vector>

namespace Srd::App {

    Shader::Shader(const std::string& iPath, const cl_device_id& iDevice, const cl_context& iContext)
    : _device(iDevice), _context(iContext)
    {
        std::ifstream file(iPath);
        _data = std::string(
            (std::istreambuf_iterator<char>(file)),
            std::istreambuf_iterator<char>()
        );
    }

    Shader::~Shader()
    {
        if(_fragmentShaderKernel)
            clReleaseKernel(_fragmentShaderKernel);
        if(_vertexShaderKernel)
            clReleaseKernel(_vertexShaderKernel);
        if(_program)
           clReleaseProgram(_program);
    }

    Shader::BuildCode Shader::Build()
    {
        if(_program) return Shader::BuildCode::AlreadyBuild;

        const char* source = _data.c_str();
        cl_int error;

        _program = clCreateProgramWithSource(_context, 1, &source, nullptr, &error);
        if (error != CL_SUCCESS) return Shader::BuildCode::BuildFailed;

        error = clBuildProgram(_program, 1, &_device, nullptr, nullptr, nullptr);
        if (error != CL_SUCCESS) 
        {
            size_t log_size;
            clGetProgramBuildInfo(_program, _device, CL_PROGRAM_BUILD_LOG, 0, nullptr, &log_size);
            std::vector<char> log(log_size);
            clGetProgramBuildInfo(_program, _device, CL_PROGRAM_BUILD_LOG, log_size, log.data(), nullptr);
            std::cerr << "Shader build error:\n" << log.data() << std::endl;

            return Shader::BuildCode::BuildFailed;
        }

        _fragmentShaderKernel = clCreateKernel(_program, "FragmentShader", &error);
        if (error != CL_SUCCESS) return Shader::BuildCode::BuildFailed;

        _vertexShaderKernel = clCreateKernel(_program, "VertexShader", &error);
        if (error != CL_SUCCESS) return Shader::BuildCode::BuildFailed;

        return Shader::BuildCode::BuildSuccess;
    }



}