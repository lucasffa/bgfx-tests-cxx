#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include <bx/math.h>

#include <iostream>

#ifdef WIN32
#include <windows.h>  // Necessário para HWND no Windows
#endif

#include <SDL2/SDL.h>        // Inclui SDL2
#include <SDL2/SDL_syswm.h>  // Para obter informações da janela no Windows

// Vertex structure
struct PosColorVertex {
  float x, y, z;
  uint32_t abgr;
};

// Layout definition
static bgfx::VertexLayout s_layout;

// Retângulo com vértices e cores
static PosColorVertex vertices[] = {
    {-1.0f, 0.0f, 1.0f, 0xff0000ff},   // Vértice 1 (vermelho)
    {1.0f, 0.0f, 1.0f, 0xff00ff00},    // Vértice 2 (verde)
    {-1.0f, 0.0f, -1.0f, 0xffff0000},  // Vértice 3 (azul)
    {1.0f, 0.0f, -1.0f, 0xffffff00},   // Vértice 4 (amarelo)
};

// Índices para desenhar dois triângulos formando um retângulo
static const uint16_t indices[] = {
    0, 1, 2,  // Triângulo 1
    1, 3, 2,  // Triângulo 2
};

int main(int argc, char **argv) {
  // Variáveis para a câmera
  bx::Vec3 cameraPos = {0.0f, 2.0f, 5.0f};     // Posição da câmera
  bx::Vec3 cameraTarget = {0.0f, 0.0f, 0.0f};  // Alvo da câmera
  bx::Vec3 cameraUp = {0.0f, 1.0f, 0.0f};      // Vetor "cima"
  const float moveSpeed = 0.1f;
  const float sensitivity = 0.01f;

  // Inicializa SDL2
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
    return 1;
  }

  // Cria uma janela SDL2
  SDL_Window *window = SDL_CreateWindow(
      "BGFX + SDL2", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720,
      SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

  if (!window) {
    std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
    SDL_Quit();
    return 1;
  }

  // Configurar o PlatformData do bgfx
  bgfx::PlatformData pd{};
#if BX_PLATFORM_WINDOWS
  SDL_SysWMinfo wmi;
  SDL_VERSION(&wmi.version);
  if (SDL_GetWindowWMInfo(window, &wmi)) {
    pd.nwh = wmi.info.win.window;  // Handle da janela no Windows
  } else {
    std::cerr << "Failed to retrieve native window handle!" << std::endl;
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 1;
  }
#elif BX_PLATFORM_LINUX
  pd.ndt = nullptr;
  pd.nwh = nullptr;
#endif

  // Inicializar o BGFX
  bgfx::Init init;
  init.platformData = pd;
  init.resolution.width = 1280;
  init.resolution.height = 720;
  init.resolution.reset = BGFX_RESET_VSYNC;
  bgfx::init(init);

  // Define layout
  s_layout.begin()
      .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
      .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
      .end();

  // Create buffers
  bgfx::VertexBufferHandle vbh = bgfx::createVertexBuffer(
      bgfx::makeRef(vertices, sizeof(vertices)), s_layout);

  bgfx::IndexBufferHandle ibh =
      bgfx::createIndexBuffer(bgfx::makeRef(indices, sizeof(indices)));

  // Set up view and projection
  float view[16];
  float proj[16];
  bx::mtxLookAt(view, cameraPos, cameraTarget, cameraUp);
  bx::mtxProj(proj, 60.0f, 1280.0f / 720.0f, 0.1f, 100.0f,
              bgfx::getCaps()->homogeneousDepth);

  bgfx::setViewTransform(0, view, proj);
  bgfx::setViewRect(0, 0, 0, 1280, 720);

  // Main loop
  bool running = true;
  SDL_SetRelativeMouseMode(SDL_TRUE);  // Ativa captura relativa do mouse

  while (running) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        running = false;
      } else if (event.type == SDL_WINDOWEVENT &&
                 event.window.event == SDL_WINDOWEVENT_RESIZED) {
        int w = event.window.data1;
        int h = event.window.data2;
        bgfx::reset(w, h, BGFX_RESET_VSYNC);
        bgfx::setViewRect(0, 0, 0, uint16_t(w), uint16_t(h));
        bx::mtxProj(proj, 60.0f, float(w) / float(h), 0.1f, 100.0f,
                    bgfx::getCaps()->homogeneousDepth);
      } else if (event.type == SDL_KEYDOWN) {
        switch (event.key.keysym.sym) {
          case SDLK_w:
            cameraPos.z -= moveSpeed;
            break;
          case SDLK_s:
            cameraPos.z += moveSpeed;
            break;
          case SDLK_a:
            cameraPos.x -= moveSpeed;
            break;
          case SDLK_d:
            cameraPos.x += moveSpeed;
            break;
        }
      }
    }

    // Atualiza a câmera com o movimento do mouse
    int mouseX, mouseY;
    SDL_GetRelativeMouseState(&mouseX, &mouseY);
    cameraTarget.x += mouseX * sensitivity;
    cameraTarget.y -= mouseY * sensitivity;

    // Atualiza a matriz de view
    bx::mtxLookAt(view, cameraPos, cameraTarget, cameraUp);
    bgfx::setViewTransform(0, view, proj);

    // Renderiza
    bgfx::touch(0);
    bgfx::setVertexBuffer(0, vbh);
    bgfx::setIndexBuffer(ibh);
    bgfx::setState(BGFX_STATE_DEFAULT);
    bgfx::submit(0, BGFX_INVALID_HANDLE);
    bgfx::frame();
  }

  // Cleanup
  bgfx::destroy(vbh);
  bgfx::destroy(ibh);
  bgfx::shutdown();

  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}
