#define CL_TARGET_OPENCL_VERSION 300

#include <CL/cl.h>
#include <SDL2/SDL.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <filesystem>
#include "SrdApp/shader.hpp"

int main() {
    // Init SDL2 (fenêtre minimale)
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "Erreur SDL_Init: " << SDL_GetError() << "\n";
        return 1;
    }
    SDL_Window* window = SDL_CreateWindow("OpenCL + SDL2 minimal", 100, 100, 640, 480, SDL_WINDOW_SHOWN);
    if (!window) {
        std::cerr << "Erreur SDL_CreateWindow: " << SDL_GetError() << "\n";
        SDL_Quit();
        return 1;
    }

    // OpenCL : récupérer plateforme
    cl_uint platCount = 0;
    clGetPlatformIDs(0, nullptr, &platCount);
    if (platCount == 0) {
        std::cerr << "Aucune plateforme OpenCL trouvée.\n";
        return 1;
    }
    std::vector<cl_platform_id> platforms(platCount);
    clGetPlatformIDs(platCount, platforms.data(), nullptr);

    char name[128];
    clGetPlatformInfo(platforms[0], CL_PLATFORM_NAME, sizeof(name), name, nullptr);
    std::cout << "Plateforme OpenCL : " << name << "\n";

    SDL_Surface* surface = SDL_GetWindowSurface(window);
    
    uint32_t* pixels = (uint32_t*)surface->pixels;

    int bpp = surface->format->BytesPerPixel;
    int pitch = surface->pitch;

    uint32_t pixel = 0xFFFFFF;

    SDL_LockSurface(surface);

    for(int x=0; x < 640; ++x)
        for(int y=0; y < 480; ++y)
            pixels[y * (pitch / 4) + x] = pixel;


    uint32_t count = 640 * 480;


    // 2. Device
    cl_device_id device;
    clGetDeviceIDs(platforms[0], CL_DEVICE_TYPE_GPU, 1, &device, nullptr);

    // 1. Contexte
    cl_context context = clCreateContext(nullptr, 1, &device, nullptr, nullptr, nullptr);

    // 3. Queue
    cl_command_queue queue = clCreateCommandQueueWithProperties(context, device, 0, nullptr);

    std::cout << "Répertoire courant : " << std::filesystem::current_path() << "\n";

    // 4. Program + kernel
    Srd::App::Shader customShader(Srd::App::Shader::ShaderPath + "shader.cl", device, context);

    if(customShader.Build() != Srd::App::Shader::BuildCode::BuildSuccess)
        return -1;


    // 1. Crée buffer
    cl_mem buffer = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, count* sizeof(uint32_t), pixels, nullptr);

    // 2. Set args
    const cl_kernel& kernel = customShader.GetVertexKernel();

    clSetKernelArg(kernel, 0, sizeof(cl_mem), &buffer);
    cl_uint cl_count = count;
    clSetKernelArg(kernel, 1, sizeof(cl_uint), &cl_count);

    // 3. Lancer kernel
    size_t global_work_size = count;
    clEnqueueNDRangeKernel(queue, kernel, 1, nullptr, &global_work_size, nullptr, 0, nullptr, nullptr);

    // 4. Lire résultat
    clEnqueueReadBuffer(queue, buffer, CL_TRUE, 0, count * sizeof(uint), pixels, 0, nullptr, nullptr);

    // écrire directement dans surface->pixels ici
    SDL_UnlockSurface(surface);

    // boucle 
    bool running = true;
    SDL_Event event;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT)
                running = false;
        }

        // Ici : update logique / lancer kernel OpenCL / update buffer pixel

        // Ici : rendu SDL (clear, afficher texture, present...)

        SDL_UpdateWindowSurface(window);
        SDL_Delay(16); // ~60 FPS
    }


    // Quit SDL proprement
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
