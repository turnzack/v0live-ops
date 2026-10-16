#include "engine/core/Window.hpp"
#include "engine/core/Log.hpp"

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#endif

namespace engine::core {

#if defined(_WIN32)
struct Window::Impl {
    HWND hwnd{nullptr};
    HINSTANCE hInstance{nullptr};
    Window* owner{nullptr};

    static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
        Window::Impl* self = reinterpret_cast<Window::Impl*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
        if (!self || !self->owner) {
            return DefWindowProcW(hwnd, msg, wParam, lParam);
        }

        switch (msg) {
            case WM_CLOSE:
                self->owner->requestClose();
                if (self->owner->m_onClose) {
                    self->owner->m_onClose();
                }
                return 0;

            case WM_SIZE: {
                const uint32_t w = static_cast<uint32_t>(LOWORD(lParam));
                const uint32_t h = static_cast<uint32_t>(HIWORD(lParam));
                self->owner->m_config.width = w;
                self->owner->m_config.height = h;
                self->owner->m_isMinimized = (wParam == SIZE_MINIMIZED);
                if (self->owner->m_onResize) {
                    self->owner->m_onResize(w, h);
                }
                return 0;
            }

            case WM_KEYDOWN:
            case WM_SYSKEYDOWN:
                if (self->owner->m_onKey) {
                    self->owner->m_onKey(static_cast<int>(wParam), true);
                }
                break;

            case WM_KEYUP:
            case WM_SYSKEYUP:
                if (self->owner->m_onKey) {
                    self->owner->m_onKey(static_cast<int>(wParam), false);
                }
                break;

            default:
                break;
        }

        return DefWindowProcW(hwnd, msg, wParam, lParam);
    }
};
#else
struct Window::Impl {
    void* dummy{nullptr};
};
#endif

Window::Window() : m_impl(std::make_unique<Impl>()) {}

Window::~Window() {
    shutdown();
}

bool Window::init(const WindowConfig& config) {
    m_config = config;
    m_shouldClose = false;

#if defined(_WIN32)
    m_impl->owner = this;
    m_impl->hInstance = GetModuleHandleW(nullptr);

    const wchar_t* kClassName = L"EngineSuiteWindowClass";

    WNDCLASSEXW wc{};
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = Impl::WndProc;
    wc.hInstance = m_impl->hInstance;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.lpszClassName = kClassName;

    RegisterClassExW(&wc);

    DWORD style = WS_OVERLAPPEDWINDOW;
    if (!m_config.resizable) {
        style &= ~(WS_THICKFRAME | WS_MAXIMIZEBOX);
    }

    RECT rect{0, 0, static_cast<LONG>(m_config.width), static_cast<LONG>(m_config.height)};
    AdjustWindowRect(&rect, style, FALSE);

    std::wstring wTitle(m_config.title.begin(), m_config.title.end());

    m_impl->hwnd = CreateWindowExW(
        0,
        kClassName,
        wTitle.c_str(),
        style,
        CW_USEDEFAULT, CW_USEDEFAULT,
        rect.right - rect.left,
        rect.bottom - rect.top,
        nullptr,
        nullptr,
        m_impl->hInstance,
        nullptr
    );

    if (!m_impl->hwnd) {
        LOG_ERROR(Window, "Impossible de creer la fenetre Win32 native");
        return false;
    }

    SetWindowLongPtr(m_impl->hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(m_impl.get()));

    ShowWindow(m_impl->hwnd, SW_SHOW);
    UpdateWindow(m_impl->hwnd);

    LOG_INFO(Window, "Fenetre native initialisee avec succes (", m_config.width, "x", m_config.height, ") : ", m_config.title);
    return true;
#else
    LOG_INFO(Window, "Fenetre fictive initialisee pour environnement sans Win32");
    return true;
#endif
}

void Window::shutdown() {
#if defined(_WIN32)
    if (m_impl && m_impl->hwnd) {
        DestroyWindow(m_impl->hwnd);
        m_impl->hwnd = nullptr;
        LOG_INFO(Window, "Fenetre detruite proprement");
    }
#endif
}

void Window::pollEvents() {
#if defined(_WIN32)
    MSG msg;
    while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
#endif
}

bool Window::shouldClose() const noexcept {
    return m_shouldClose;
}

void Window::requestClose() noexcept {
    m_shouldClose = true;
}

WindowNativeHandles Window::getNativeHandles() const noexcept {
    WindowNativeHandles handles{};
#if defined(_WIN32)
    if (m_impl) {
        handles.nativeWindowHandle = m_impl->hwnd;
        handles.nativeDisplayHandle = m_impl->hInstance;
    }
#endif
    return handles;
}

} // namespace engine::core
