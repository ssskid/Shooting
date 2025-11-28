#include "Graphics.h"
#include <Windows.h>
#include <iostream>
#include <algorithm>

// Helper macro for releasing COM objects
template<class Interface>
inline void SafeRelease(Interface **ppInterfaceToRelease)
{
    if (*ppInterfaceToRelease != nullptr)
    {
        (*ppInterfaceToRelease)->Release();
        (*ppInterfaceToRelease) = nullptr;
    }
}

Graphics::Graphics() :
    factory(nullptr),
    pWFactory(nullptr),
    rendertarget(nullptr),
    m_pTextureRT(nullptr),
    brush(nullptr),
    m_pCustomFontCollection(nullptr), // IDWriteFontCollection1* 로 초기화 (타입 변경됨)
    m_pD3DDevice(nullptr),
    m_pD3DImmediateContext(nullptr),
    m_pSwapChain(nullptr),
    m_pBackBuffer(nullptr),
    m_hWnd(nullptr),
    m_isFullscreen(false),
    m_currentWidth(SCREEN_X),
    m_currentHeight(SCREEN_Y),
    m_initialized(false),
    m_resourcesNeedReload(false), 
    m_fadeState(FadeState::FADE_NONE),
    m_fadeColor(D2D1::ColorF(D2D1::ColorF::Black)),
    m_fadeDuration(0.0f),
    m_fadeElapsedTime(0.0f),
    m_currentFadeAlpha(0.0f),
    m_fadeBrush(nullptr)
{
    for (int i = 0; i < FONTFORMAT; i++) pWFormat[i] = nullptr;
    for (int i = 0; i < FONTCOL; i++) Wbrush[i] = nullptr;
    for (int i = 0; i < SPRITE_MAX; i++) {
        for (int j = 0; j < SPRITE_IMGMAXPERCHARACTER; j++) {
            bitmap[i][j] = nullptr;
        }
    }
}

Graphics::~Graphics() {
    Cleanup();
}

void Graphics::Cleanup() {
    OutputDebugString(L"Graphics::Cleanup called.\n");
    ReleaseWindowSizeDependentResources();
    ReleaseDeviceResources();
    ReleaseDeviceIndependentResources();
    m_initialized = false;
}

// Device-independent resources
void Graphics::CreateDeviceIndependentResources() {
    HRESULT hr = S_OK;
    WCHAR errorMsg[256]; 
    OutputDebugString(L"Graphics::CreateDeviceIndependentResources called.\n");

    if (!factory) {
        hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &factory);
        if (FAILED(hr)) { 
            swprintf_s(errorMsg, L"D2D1CreateFactory failed: 0x%08X\n", hr);
            OutputDebugString(errorMsg);
            return; 
        }
    }

    if (!pWFactory) {
        hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(&pWFactory));
        if (FAILED(hr)) { 
            swprintf_s(errorMsg, L"DWriteCreateFactory failed: 0x%08X\n", hr);
            OutputDebugString(errorMsg);
            return;
        }
    }

    if (pWFactory) {
        IDWriteFactory3* pWFactory3 = nullptr;
        hr = pWFactory->QueryInterface(IID_PPV_ARGS(&pWFactory3));

        if (SUCCEEDED(hr) && pWFactory3) {
            OutputDebugString(L"Graphics: IDWriteFactory3 obtained. Attempting to load custom font DOSIyagiBoldface.ttf.\n");
            IDWriteFontFile* pFontFile = nullptr;
            const wchar_t* fontFilePath = L".\\DOSIyagiBoldface.ttf";
            hr = pWFactory3->CreateFontFileReference(fontFilePath, nullptr, &pFontFile);

            if (SUCCEEDED(hr) && pFontFile) {
                OutputDebugString(L"Graphics: Font file reference created for DOSIyagiBoldface.ttf.\n");
                
                IDWriteFontSetBuilder* pFontSetBuilderBase = nullptr; 
                hr = pWFactory3->CreateFontSetBuilder(&pFontSetBuilderBase);

                if (SUCCEEDED(hr) && pFontSetBuilderBase) {
                    IDWriteFontSetBuilder1* pFontSetBuilder1 = nullptr;
                    hr = pFontSetBuilderBase->QueryInterface(IID_PPV_ARGS(&pFontSetBuilder1)); // IDWriteFontSetBuilder1로 QI

                    if (SUCCEEDED(hr) && pFontSetBuilder1) {
                        OutputDebugString(L"Graphics: IDWriteFontSetBuilder1 obtained.\n");
                        hr = pFontSetBuilder1->AddFontFile(pFontFile); // IDWriteFontSetBuilder1 포인터로 AddFontFile 호출
                        if (SUCCEEDED(hr)) {
                            IDWriteFontSet* pFontSet = nullptr;
                            hr = pFontSetBuilder1->CreateFontSet(&pFontSet); // IDWriteFontSetBuilder1 포인터로 CreateFontSet 호출
                            if (SUCCEEDED(hr) && pFontSet) {
                                hr = pWFactory3->CreateFontCollectionFromFontSet(pFontSet, &m_pCustomFontCollection);
                                if (SUCCEEDED(hr)) {
                                    OutputDebugString(L"Graphics: Custom font collection (IDWriteFontCollection1) created successfully.\n");
                                } else {
                                    swprintf_s(errorMsg, L"Graphics: Failed to create font collection from font set. HR: 0x%08X\n", hr);
                                    OutputDebugString(errorMsg);
                                }
                                SafeRelease(&pFontSet);
                            } else {
                                swprintf_s(errorMsg, L"Graphics: Failed to create font set. HR: 0x%08X\n", hr);
                                OutputDebugString(errorMsg);
                            }
                        } else {
                            swprintf_s(errorMsg, L"Graphics: Failed to add font file to font set builder (using IDWriteFontSetBuilder1). HR: 0x%08X\n", hr);
                            OutputDebugString(errorMsg);
                        }
                        SafeRelease(&pFontSetBuilder1);
                    } else {
                        swprintf_s(errorMsg, L"Graphics: Failed to QueryInterface for IDWriteFontSetBuilder1. HR: 0x%08X\n", hr);
                        OutputDebugString(errorMsg);
                    }
                    SafeRelease(&pFontSetBuilderBase); // QueryInterface 성공 여부와 관계없이 원래 포인터 해제
                } else {
                    swprintf_s(errorMsg, L"Graphics: Failed to create IDWriteFontSetBuilder. HR: 0x%08X\n", hr);
                    OutputDebugString(errorMsg);
                }
                SafeRelease(&pFontFile);
            } else {
                swprintf_s(errorMsg, L"Graphics: Failed to create font file reference for '%s'. HR: 0x%08X\n", fontFilePath, hr);
                OutputDebugString(errorMsg);
            }
            SafeRelease(&pWFactory3);
        } else {
            swprintf_s(errorMsg, L"Graphics: Failed to QueryInterface for IDWriteFactory3 (HR: 0x%08X).\n", hr);
            OutputDebugString(errorMsg);
        }
    }

    hr = CreateTextFormatsInternal();
    if (FAILED(hr)) {
        return;
    }
    OutputDebugString(L"Graphics::CreateDeviceIndependentResources finished.\n");
}

void Graphics::ReleaseDeviceIndependentResources() {
    OutputDebugString(L"Graphics::ReleaseDeviceIndependentResources called.\n");
    for (int i = 0; i < FONTFORMAT; i++) SafeRelease(&pWFormat[i]);
    SafeRelease(&m_pCustomFontCollection); 
    SafeRelease(&pWFactory);
    SafeRelease(&factory);
}

// Device-dependent resources (D3D device, DXGI swap chain)
HRESULT Graphics::CreateDeviceResources() {
    HRESULT hr = S_OK;
    WCHAR errorMsg[128];
    OutputDebugString(L"Graphics::CreateDeviceResources called.\n");

    if (m_pD3DDevice) {
        OutputDebugString(L"D3DDevice already exists.\n");
        return S_OK;
    }

    UINT createDeviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#ifdef _DEBUG
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    D3D_FEATURE_LEVEL featureLevels[] = {
        D3D_FEATURE_LEVEL_11_1, D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1, D3D_FEATURE_LEVEL_10_0,
    };
    D3D_FEATURE_LEVEL featureLevel;

    hr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags,
                           featureLevels, ARRAYSIZE(featureLevels), D3D11_SDK_VERSION,
                           &m_pD3DDevice, &featureLevel, &m_pD3DImmediateContext);
    if (FAILED(hr)) {
        swprintf_s(errorMsg, L"D3D11CreateDevice failed: 0x%08X\n", hr);
        OutputDebugString(errorMsg);
        return hr;
    }

    IDXGIDevice* pDxgiDevice = nullptr;
    hr = m_pD3DDevice->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(&pDxgiDevice));
    if (FAILED(hr)) {
        swprintf_s(errorMsg, L"QueryInterface for IDXGIDevice failed: 0x%08X\n", hr);
        OutputDebugString(errorMsg);
        SafeRelease(&m_pD3DImmediateContext); SafeRelease(&m_pD3DDevice);
        return hr;
    }

    IDXGIAdapter* pDxgiAdapter = nullptr;
    hr = pDxgiDevice->GetAdapter(&pDxgiAdapter);
    if (FAILED(hr)) {
        swprintf_s(errorMsg, L"GetAdapter failed: 0x%08X\n", hr);
        OutputDebugString(errorMsg);
        SafeRelease(&pDxgiDevice); SafeRelease(&m_pD3DImmediateContext); SafeRelease(&m_pD3DDevice);
        return hr;
    }

    IDXGIFactory2* pDxgiFactory = nullptr;
    hr = pDxgiAdapter->GetParent(__uuidof(IDXGIFactory2), reinterpret_cast<void**>(&pDxgiFactory));
    if (FAILED(hr)) {
        swprintf_s(errorMsg, L"GetParent for IDXGIFactory2 failed: 0x%08X\n", hr);
        OutputDebugString(errorMsg);
        SafeRelease(&pDxgiAdapter); SafeRelease(&pDxgiDevice); SafeRelease(&m_pD3DImmediateContext); SafeRelease(&m_pD3DDevice);
        return hr;
    }

    DXGI_SWAP_CHAIN_DESC1 swapChainDesc = { 0 };
    swapChainDesc.Width = m_currentWidth;
    swapChainDesc.Height = m_currentHeight;
    swapChainDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.SampleDesc.Quality = 0;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.BufferCount = 2;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
    swapChainDesc.Scaling = DXGI_SCALING_STRETCH;

    hr = pDxgiFactory->CreateSwapChainForHwnd(m_pD3DDevice, m_hWnd, &swapChainDesc,
                                            nullptr, nullptr, &m_pSwapChain);
    if (FAILED(hr)) {
        swprintf_s(errorMsg, L"CreateSwapChainForHwnd failed: 0x%08X. Width: %u, Height: %u\n", hr, m_currentWidth, m_currentHeight);
        OutputDebugString(errorMsg);
        SafeRelease(&pDxgiFactory); SafeRelease(&pDxgiAdapter); SafeRelease(&pDxgiDevice);
        SafeRelease(&m_pD3DImmediateContext); SafeRelease(&m_pD3DDevice);
        return hr;
    }

    SafeRelease(&pDxgiFactory);
    SafeRelease(&pDxgiAdapter);
    SafeRelease(&pDxgiDevice);

    OutputDebugString(L"Graphics::CreateDeviceResources finished successfully.\n");
    return S_OK;
}

void Graphics::ReleaseDeviceResources() {
    OutputDebugString(L"Graphics::ReleaseDeviceResources called.\n");
    if (m_pSwapChain) {
        m_pSwapChain->SetFullscreenState(FALSE, nullptr);
    }
    SafeRelease(&m_pBackBuffer);
    SafeRelease(&m_pSwapChain);
    SafeRelease(&m_pD3DImmediateContext);
    SafeRelease(&m_pD3DDevice);
}

// Bitmap release utility
void Graphics::ReleaseBitmaps() {
    for (int i = 0; i < SPRITE_MAX; i++) {
        for (int j = 0; j < SPRITE_IMGMAXPERCHARACTER; j++) {
            SafeRelease(&bitmap[i][j]);
        }
    }
}

// Window-size dependent resources (Render targets, brushes, bitmaps)
HRESULT Graphics::RecreateWindowSizeDependentResources() {
    HRESULT hr = S_OK;
    WCHAR msgBuffer[256];

    OutputDebugString(L"Graphics::RecreateWindowSizeDependentResources called.\n");
    if (m_isFullscreen) {
        swprintf_s(msgBuffer, L"Mode: Fullscreen, Target Size: %u x %u\n", m_currentWidth, m_currentHeight);
    } else {
        swprintf_s(msgBuffer, L"Mode: Windowed, Target Size: %u x %u\n", m_currentWidth, m_currentHeight);
    }
    OutputDebugString(msgBuffer);

    SafeRelease(&rendertarget);
    SafeRelease(&m_pTextureRT);
    SafeRelease(&brush);
    for (int i = 0; i < FONTCOL; i++) SafeRelease(&Wbrush[i]);
    SafeRelease(&m_fadeBrush);
    SafeRelease(&m_pBackBuffer);
    ReleaseBitmaps();

    ID2D1RenderTarget* pCurrentRTForBrushCreation = nullptr;

    if (!m_pSwapChain) {
        OutputDebugString(L"Error: m_pSwapChain is null in RecreateWindowSizeDependentResources. Cannot proceed.\n");
        return E_FAIL;
    }

    hr = m_pSwapChain->ResizeBuffers(0, m_currentWidth, m_currentHeight, DXGI_FORMAT_B8G8R8A8_UNORM, 0);
    if (FAILED(hr)) {
        swprintf_s(msgBuffer, L"ResizeBuffers failed: 0x%08X. Target Size: %ux%u\n", hr, m_currentWidth, m_currentHeight);
        OutputDebugString(msgBuffer);
        return hr; 
    }
    OutputDebugString(L"ResizeBuffers succeeded.\n");

    if (m_isFullscreen) {
        OutputDebugString(L"Recreating for Fullscreen mode.\n");
        hr = m_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&m_pBackBuffer));
        if (FAILED(hr)) {
            swprintf_s(msgBuffer, L"GetBuffer (Fullscreen) failed: 0x%08X\n", hr);
            OutputDebugString(msgBuffer);
            return hr;
        }
        OutputDebugString(L"GetBuffer (Fullscreen) succeeded.\n");

        D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(
            D2D1_RENDER_TARGET_TYPE_DEFAULT,
            D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED));

        IDXGISurface* pDxgiSurface = nullptr;
        hr = m_pBackBuffer->QueryInterface(&pDxgiSurface);
        if (FAILED(hr)) {
            swprintf_s(msgBuffer, L"QueryInterface for IDXGISurface failed: 0x%08X\n", hr);
            OutputDebugString(msgBuffer);
            SafeRelease(&m_pBackBuffer); 
            return hr;
        }
        OutputDebugString(L"QueryInterface for IDXGISurface succeeded.\n");

        if (!factory) {
             OutputDebugString(L"Error: D2D1Factory is null when creating DxgiSurfaceRenderTarget.\n");
             SafeRelease(&pDxgiSurface); SafeRelease(&m_pBackBuffer);
             return E_FAIL;
        }
        hr = factory->CreateDxgiSurfaceRenderTarget(pDxgiSurface, &props, &m_pTextureRT);
        SafeRelease(&pDxgiSurface); 
        if (FAILED(hr)) {
            swprintf_s(msgBuffer, L"CreateDxgiSurfaceRenderTarget failed: 0x%08X\n", hr);
            OutputDebugString(msgBuffer);
            SafeRelease(&m_pBackBuffer); 
            return hr;
        }
        OutputDebugString(L"CreateDxgiSurfaceRenderTarget succeeded.\n");
        pCurrentRTForBrushCreation = m_pTextureRT;
    } else {
        OutputDebugString(L"Recreating for Windowed mode.\n");
        
        RECT rcClient;
        GetClientRect(m_hWnd, &rcClient);
        D2D1_SIZE_U clientSize = D2D1::SizeU(rcClient.right - rcClient.left, rcClient.bottom - rcClient.top);
        
        if (clientSize.width != m_currentWidth || clientSize.height != m_currentHeight) {
            swprintf_s(msgBuffer, L"Warning: ClientRect size (%u x %u) differs from m_currentSize (%u x %u) in Windowed recreate.\n", 
                       clientSize.width, clientSize.height, m_currentWidth, m_currentHeight);
            OutputDebugString(msgBuffer);
        }
        if (m_currentWidth == 0 || m_currentHeight == 0) {
             swprintf_s(msgBuffer, L"Error: Target size for HwndRenderTarget is zero. Using default: %ux%u\n", SCREEN_X, SCREEN_Y);
             OutputDebugString(msgBuffer);
             m_currentWidth = SCREEN_X;
             m_currentHeight = SCREEN_Y;
        }

        D2D1_RENDER_TARGET_PROPERTIES rtProps = D2D1::RenderTargetProperties();
        D2D1_HWND_RENDER_TARGET_PROPERTIES hwndProps = D2D1::HwndRenderTargetProperties(m_hWnd, D2D1::SizeU(m_currentWidth, m_currentHeight));
        
        if (!factory) {
             OutputDebugString(L"Error: D2D1Factory is null when creating HwndRenderTarget.\n");
             return E_FAIL;
        }
        hr = factory->CreateHwndRenderTarget(rtProps, hwndProps, &rendertarget);
        if (FAILED(hr)) {
            swprintf_s(msgBuffer, L"CreateHwndRenderTarget failed: 0x%08X. Size: %ux%u\n", hr, m_currentWidth, m_currentHeight);
            OutputDebugString(msgBuffer);
            return hr;
        }
        OutputDebugString(L"CreateHwndRenderTarget succeeded.\n");
        pCurrentRTForBrushCreation = rendertarget;
    }

    if (SUCCEEDED(hr) && pCurrentRTForBrushCreation) {
        OutputDebugString(L"Creating brushes.\n");
        hr = pCurrentRTForBrushCreation->CreateSolidColorBrush(D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.0f), &brush);
        if (FAILED(hr)) {
            swprintf_s(msgBuffer, L"CreateSolidColorBrush (generic) failed: 0x%08X\n", hr);
            OutputDebugString(msgBuffer);
            return hr;
        }
        hr = CreateSpecificBrushes(pCurrentRTForBrushCreation);
        if (FAILED(hr)) {
            return hr;
        }
        hr = pCurrentRTForBrushCreation->CreateSolidColorBrush(m_fadeColor, &m_fadeBrush);
        if (FAILED(hr)) {
          swprintf_s(msgBuffer, L"CreateSolidColorBrush (fade) failed: 0x%08X\n", hr);
          OutputDebugString(msgBuffer);
          return hr;
        }
        OutputDebugString(L"Brushes created successfully.\n");
    } else if (FAILED(hr)) {
        swprintf_s(msgBuffer, L"Failed to create render target, cannot create brushes. HR: 0x%08X\n", hr);
        OutputDebugString(msgBuffer);
        return hr; 
    } else if (!pCurrentRTForBrushCreation) {
        OutputDebugString(L"Error: pCurrentRTForBrushCreation is null after render target creation logic.\n");
        return E_UNEXPECTED;
    }

    m_resourcesNeedReload = true;
    OutputDebugString(L"Graphics::RecreateWindowSizeDependentResources finished successfully.\n");
    return S_OK;
}

void Graphics::ReleaseWindowSizeDependentResources() {
    OutputDebugString(L"Graphics::ReleaseWindowSizeDependentResources called.\n");
    SafeRelease(&rendertarget); 
    SafeRelease(&m_pTextureRT);
    SafeRelease(&brush);        
    for (int i = 0; i < FONTCOL; i++) SafeRelease(&Wbrush[i]);
    SafeRelease(&m_fadeBrush);
    ReleaseBitmaps(); 
    SafeRelease(&m_pBackBuffer);
}

ID2D1RenderTarget* Graphics::GetCurrentRenderTarget() {
    if (!m_initialized) return nullptr;
    return m_isFullscreen ? m_pTextureRT : rendertarget;
}

HRESULT Graphics::CreateSpecificBrushes(ID2D1RenderTarget* pRT) {
    if (!pRT) {
        OutputDebugString(L"Error: CreateSpecificBrushes called with null render target.\n");
        return E_POINTER;
    }
    HRESULT hr = S_OK;
    WCHAR errorMsg[128];

    hr = pRT->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::DarkKhaki, 1.0f), &Wbrush[0]); 
    if (FAILED(hr)) { swprintf_s(errorMsg, L"CreateSpecificBrushes[0] failed: 0x%08X\n", hr); OutputDebugString(errorMsg); return hr; }
    hr = pRT->CreateSolidColorBrush(D2D1::ColorF(0.4f, 0.4f, 0.4f, 1.0f), &Wbrush[1]); if (FAILED(hr)) { return hr; }
    hr = pRT->CreateSolidColorBrush(D2D1::ColorF(0.9f, 0.9f, 0.9f, 1.0f), &Wbrush[2]); if (FAILED(hr)) { return hr; }
    hr = pRT->CreateSolidColorBrush(D2D1::ColorF(1.0f, 0.5f, 0.5f, 1.0f), &Wbrush[3]); if (FAILED(hr)) { return hr; }
    hr = pRT->CreateSolidColorBrush(D2D1::ColorF(0.5f, 1.0f, 0.5f, 1.0f), &Wbrush[4]); if (FAILED(hr)) { return hr; }
    hr = pRT->CreateSolidColorBrush(D2D1::ColorF(0.5f, 0.5f, 1.0f, 1.0f), &Wbrush[5]); if (FAILED(hr)) { return hr; }
    if (FONTCOL > 6) { hr = pRT->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black, 1.0f), &Wbrush[6]); if (FAILED(hr)) { return hr; } }
    if (FONTCOL > 7) { hr = pRT->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White, 1.0f), &Wbrush[7]); if (FAILED(hr)) { return hr; } }
    return S_OK;
}

HRESULT Graphics::CreateTextFormatsInternal() {
    if (!pWFactory) {
         OutputDebugString(L"Error: CreateTextFormatsInternal called with null DWriteFactory.\n");
         return E_POINTER;
    }
    HRESULT hr = S_OK;
    WCHAR errorMsg[256]; 
    const wchar_t* fontFamilyName = L"DOSIyagiBoldface";

    IDWriteFontCollection* fontCollectionToUse = m_pCustomFontCollection; 
    if (m_pCustomFontCollection) {
        OutputDebugString(L"Graphics: Using custom font collection for text formats.\n");
    } else {
        OutputDebugString(L"Graphics: Custom font collection not available or failed to load. Will attempt to use system font 'DOSIyagiBoldface'.\n");
    }

    hr = pWFactory->CreateTextFormat(fontFamilyName, fontCollectionToUse, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 36.0f, L"ko-kr", &pWFormat[0]); 
    if (FAILED(hr)) { swprintf_s(errorMsg, L"CreateTextFormat[0] ('%s') failed: 0x%08X. Ensure font is loaded or installed.\n", fontFamilyName, hr); OutputDebugString(errorMsg); }
    
    hr = pWFactory->CreateTextFormat(fontFamilyName, fontCollectionToUse, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 12.0f, L"ko-kr", &pWFormat[1]); 
    if (FAILED(hr)) { swprintf_s(errorMsg, L"CreateTextFormat[1] ('%s') failed: 0x%08X. Ensure font is loaded or installed.\n", fontFamilyName, hr); OutputDebugString(errorMsg); }
    
    hr = pWFactory->CreateTextFormat(fontFamilyName, fontCollectionToUse, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 20.0f, L"ko-kr", &pWFormat[2]); 
    if (FAILED(hr)) { swprintf_s(errorMsg, L"CreateTextFormat[2] ('%s') failed: 0x%08X. Ensure font is loaded or installed.\n", fontFamilyName, hr); OutputDebugString(errorMsg); }

    hr = pWFactory->CreateTextFormat(fontFamilyName, fontCollectionToUse, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 36.0f, L"ko-kr", &pWFormat[3]);
    if (FAILED(hr)) { swprintf_s(errorMsg, L"CreateTextFormat[3] ('%s') failed: 0x%08X. Ensure font is loaded or installed.\n", fontFamilyName, hr); OutputDebugString(errorMsg); }

    for (int i = 0; i < FONTFORMAT; ++i) {
      if (pWFormat[i]) {
        if (i < 3) {
          HRESULT hrAlign = pWFormat[i]->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
          if (FAILED(hrAlign)) { /*오류 처리 또는 로깅*/ }
          hrAlign = pWFormat[i]->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
          if (FAILED(hrAlign)) { /*오류 처리 또는 로깅*/ }
        }
        else {
          HRESULT hrAlign = pWFormat[i]->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
          if (FAILED(hrAlign)) { /*오류 처리 또는 로깅*/ }
          hrAlign = pWFormat[i]->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
          if (FAILED(hrAlign)) { /*오류 처리 또는 로깅*/ }
        }
      }
    }
    OutputDebugString(L"Graphics: CreateTextFormatsInternal completed attempts to create text formats.\n");
    return S_OK;
}

bool Graphics::Init(HWND hWindow) {
    OutputDebugString(L"Graphics::Init called.\n");
    if (m_initialized) {
        OutputDebugString(L"Graphics already initialized.\n");
        return true;
    }

    m_hWnd = hWindow;
    m_isFullscreen = false; 

    RECT rcClient;
    GetClientRect(m_hWnd, &rcClient);
    m_currentWidth = rcClient.right - rcClient.left;
    m_currentHeight = rcClient.bottom - rcClient.top;
    if (m_currentWidth == 0 || m_currentHeight == 0) {
        OutputDebugString(L"Warning: ClientRect size is zero in Init. Using default.\n");
        m_currentWidth = SCREEN_X; 
        m_currentHeight = SCREEN_Y;
    }
    
    WCHAR sizeMsg[100];
    swprintf_s(sizeMsg, L"Initial size set to: %u x %u\n", m_currentWidth, m_currentHeight);
    OutputDebugString(sizeMsg);

    CreateDeviceIndependentResources();
    if (!factory || !pWFactory) {
        OutputDebugString(L"Failed to create device independent resources in Init.\n");
        Cleanup();
        return false;
    }

    if (FAILED(CreateDeviceResources())) { 
        OutputDebugString(L"Failed to create device resources in Init.\n");
        Cleanup();
        return false;
    }
    if (FAILED(RecreateWindowSizeDependentResources())) { 
        OutputDebugString(L"Failed to recreate window size dependent resources in Init.\n");
        Cleanup();
        return false;
    }

    bool textFormatsOk = true;
    for (int i = 0; i < FONTFORMAT; ++i) {
        if (!pWFormat[i]) {
            textFormatsOk = false;
            break;
        }
    }
    if (!textFormatsOk) {
        OutputDebugString(L"WARNING: Failed to create one or more text formats in Init using 'DOSIyagiBoldface'. Check font availability and DWrite logs.\n");
    }

    m_initialized = true;
    OutputDebugString(L"Graphics::Init finished successfully.\n");
    return true;
}

void Graphics::SetFullscreen(bool enable) {
    WCHAR msgBuffer[256];
    if (!m_initialized) {
        OutputDebugString(L"SetFullscreen called before initialization.\n");
        return;
    }

    swprintf_s(msgBuffer, L"Graphics::SetFullscreen called. Current fullscreen: %s, Target: %s\n", m_isFullscreen ? L"true" : L"false", enable ? L"true" : L"false");
    OutputDebugString(msgBuffer);

    if (!m_pSwapChain) {
        OutputDebugString(L"Error: m_pSwapChain is null in SetFullscreen. Cannot switch mode.\n");
        return;
    }
    
    SafeRelease(&rendertarget);
    SafeRelease(&m_pTextureRT);
    SafeRelease(&m_pBackBuffer);

    HRESULT hr;
    if (enable) {
        m_currentWidth = GetSystemMetrics(SM_CXSCREEN);
        m_currentHeight = GetSystemMetrics(SM_CYSCREEN);
        swprintf_s(msgBuffer, L"Attempting to switch to Fullscreen. Target resolution: %ux%u\n", m_currentWidth, m_currentHeight);
        OutputDebugString(msgBuffer);

        hr = m_pSwapChain->SetFullscreenState(TRUE, nullptr);
        if (FAILED(hr)) {
            swprintf_s(msgBuffer, L"SetFullscreenState(TRUE) failed: 0x%08X\n", hr);
            OutputDebugString(msgBuffer);
            return;
        }
        m_isFullscreen = true;
        OutputDebugString(L"SetFullscreenState(TRUE) succeeded.\n");
    } else {
        m_currentWidth = SCREEN_X; 
        m_currentHeight = SCREEN_Y;
        swprintf_s(msgBuffer, L"Attempting to switch to Windowed. Target size: %ux%u\n", m_currentWidth, m_currentHeight);
        OutputDebugString(msgBuffer);

        hr = m_pSwapChain->SetFullscreenState(FALSE, nullptr);
        if (FAILED(hr)) {
            swprintf_s(msgBuffer, L"SetFullscreenState(FALSE) failed: 0x%08X\n", hr);
            OutputDebugString(msgBuffer);
            return;
        }
        m_isFullscreen = false;
        OutputDebugString(L"SetFullscreenState(FALSE) succeeded.\n");

        SetWindowLongPtr(m_hWnd, GWL_STYLE, WS_OVERLAPPEDWINDOW | WS_VISIBLE);
        RECT rect = { 0, 0, (LONG)m_currentWidth, (LONG)m_currentHeight };
        AdjustWindowRectEx(&rect, WS_OVERLAPPEDWINDOW, FALSE, 0);
        SetWindowPos(m_hWnd, HWND_NOTOPMOST, 100, 100, 
                     rect.right - rect.left, rect.bottom - rect.top, 
                     SWP_FRAMECHANGED | SWP_SHOWWINDOW | SWP_NOZORDER);
        OutputDebugString(L"Window style and position restored for windowed mode.\n");
    }

    hr = RecreateWindowSizeDependentResources();
    if (FAILED(hr)) {
        swprintf_s(msgBuffer, L"RecreateWindowSizeDependentResources failed after mode switch: 0x%08X\n", hr);
        OutputDebugString(msgBuffer);
    } else {
        OutputDebugString(L"Mode switch process finished, resources should be reloaded by the game.\n");
    }
}

void Graphics::Resize(UINT width, UINT height) {
    WCHAR msgBuffer[256];
    if (!m_initialized) {
        OutputDebugString(L"Resize called before initialization.\n");
        return;
    }

    if (width == 0 || height == 0) {
        swprintf_s(msgBuffer, L"Resize called with zero dimension (likely minimized). Width: %u, Height: %u. Ignoring.\n", width, height);
        OutputDebugString(msgBuffer);
        return;
    }
    
    if (m_currentWidth == width && m_currentHeight == height && !m_isFullscreen) {
        swprintf_s(msgBuffer, L"Resize called with no actual size change (Windowed). Current: %ux%u, New: %ux%u. Ignoring.\n", m_currentWidth, m_currentHeight, width, height);
        OutputDebugString(msgBuffer);
        return;
    }
    
    swprintf_s(msgBuffer, L"Graphics::Resize called. New size: %u x %u. Current mode: %s\n", width, height, m_isFullscreen ? L"Fullscreen" : L"Windowed");
    OutputDebugString(msgBuffer);

    m_currentWidth = width;
    m_currentHeight = height;

    if (FAILED(RecreateWindowSizeDependentResources())) {
        swprintf_s(msgBuffer, L"RecreateWindowSizeDependentResources failed during Resize.\n");
        OutputDebugString(msgBuffer);
    } else {
        OutputDebugString(L"Resize successful, resources should be reloaded by the game.\n");
    }
}

void Graphics::BeginDraw() {
    ID2D1RenderTarget* pRT = GetCurrentRenderTarget();
    if (pRT) {
        pRT->BeginDraw();

        if (m_initialized) {
            D2D1_MATRIX_3X2_F scaleMatrix = D2D1::Matrix3x2F::Identity();
            if (m_currentWidth != 0 && m_currentHeight != 0) {
                float logicalWidth = static_cast<float>(SCREEN_X);
                float logicalHeight = static_cast<float>(SCREEN_Y);

                float scaleX = static_cast<float>(m_currentWidth) / logicalWidth;
                float scaleY = static_cast<float>(m_currentHeight) / logicalHeight;

                scaleMatrix = D2D1::Matrix3x2F::Scale(scaleX, scaleY);
            }
            pRT->SetTransform(scaleMatrix);
        }
    }
}

void Graphics::EndDraw() {
    ID2D1RenderTarget* pRT = GetCurrentRenderTarget();
    WCHAR errorMsg[128];

    if (pRT) {
        RenderFadeOverlay();
        pRT->SetTransform(D2D1::Matrix3x2F::Identity());
        HRESULT hr = pRT->EndDraw();
        if (FAILED(hr)) { 
            swprintf_s(errorMsg, L"EndDraw failed with HRESULT 0x%08X. Attempting to recreate target.\n", hr);
            OutputDebugString(errorMsg);
            
            if(FAILED(RecreateWindowSizeDependentResources())) {
                OutputDebugString(L"FATAL: RecreateWindowSizeDependentResources failed after EndDraw failure. Terminating.\n");
                PostQuitMessage(0);
            } else {
                 OutputDebugString(L"Render target recreated after EndDraw failure. Resources need reload.\n");
            }
            return; 
        }
    }

    if (m_isFullscreen && m_pSwapChain) {
        HRESULT hr = m_pSwapChain->Present(1, 0); 
        if (FAILED(hr) && hr != DXGI_ERROR_WAS_STILL_DRAWING && hr != DXGI_STATUS_OCCLUDED) {
            swprintf_s(errorMsg, L"Present failed with HRESULT 0x%08X.\n", hr);
            OutputDebugString(errorMsg);
            if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET || hr == DXGI_ERROR_DEVICE_HUNG) {
                OutputDebugString(L"Device removed/reset/hung during Present. Releasing all D3D/DXGI resources.\n");
                ReleaseWindowSizeDependentResources();
                ReleaseDeviceResources();
                m_initialized = false;
                PostQuitMessage(0); 
            }
        }
    }
}

bool Graphics::NeedsResourceReload() const {
    return m_resourcesNeedReload;
}

void Graphics::MarkResourcesAsLoaded() {
    m_resourcesNeedReload = false;
    OutputDebugString(L"Resources marked as loaded.\n");
}

bool Graphics::MyLoadimage(int i, int j, const wchar_t* imgpath) {
    if (i < 0 || i >= SPRITE_MAX || j < 0 || j >= SPRITE_IMGMAXPERCHARACTER) return false;

    ID2D1RenderTarget* pRT = GetCurrentRenderTarget();
    if (!pRT) {
        OutputDebugString(L"MyLoadimage: Current Render Target is null. Cannot load image.\n");
        return false;
    }

    SafeRelease(&bitmap[i][j]); 

    IWICImagingFactory* wicFactory = nullptr;
    HRESULT hr = CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&wicFactory));
    if (FAILED(hr)) { SafeRelease(&wicFactory); return false; }

    IWICBitmapDecoder* pDecoder = nullptr;
    hr = wicFactory->CreateDecoderFromFilename(imgpath, NULL, GENERIC_READ, WICDecodeMetadataCacheOnDemand, &pDecoder);
    if (FAILED(hr)) { SafeRelease(&pDecoder); SafeRelease(&wicFactory); return false; }

    IWICBitmapFrameDecode* pFrame = nullptr;
    hr = pDecoder->GetFrame(0, &pFrame);
    if (FAILED(hr)) { SafeRelease(&pFrame); SafeRelease(&pDecoder); SafeRelease(&wicFactory); return false; }

    IWICFormatConverter* pConverter = nullptr;
    hr = wicFactory->CreateFormatConverter(&pConverter);
    if (FAILED(hr)) { SafeRelease(&pConverter); SafeRelease(&pFrame); SafeRelease(&pDecoder); SafeRelease(&wicFactory); return false; }

    hr = pConverter->Initialize(pFrame, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, NULL, 0.0f, WICBitmapPaletteTypeMedianCut);
    if (FAILED(hr)) { SafeRelease(&pConverter); SafeRelease(&pFrame); SafeRelease(&pDecoder); SafeRelease(&wicFactory); return false; }

    hr = pRT->CreateBitmapFromWicBitmap(pConverter, NULL, &bitmap[i][j]);
    
    SafeRelease(&pConverter);
    SafeRelease(&pFrame);
    SafeRelease(&pDecoder);
    SafeRelease(&wicFactory);

    if (FAILED(hr)) { 
        WCHAR errorMsg[256];
        swprintf_s(errorMsg, L"CreateBitmapFromWicBitmap failed for %s: 0x%08X\n", imgpath, hr);
        OutputDebugString(errorMsg);
        return false; 
    }
    return true;
}

bool Graphics::MyUnLoadimage(int i, int j) {
    if (i < 0 || i >= SPRITE_MAX || j < 0 || j >= SPRITE_IMGMAXPERCHARACTER) return false;
    if (bitmap[i][j]) { 
        SafeRelease(&bitmap[i][j]);
        return true;
    }
    return false;
}

void Graphics::ClearScreen(float r, float g, float b) {
    ID2D1RenderTarget* pRT = GetCurrentRenderTarget();
    if (pRT) {
        pRT->Clear(D2D1::ColorF(r, g, b));
    }
}

void Graphics::DrawCircle(float x, float y, float radius, float r, float g, float b, float a) {
    ID2D1RenderTarget* pRT = GetCurrentRenderTarget();
    if (pRT && brush) { 
        brush->SetColor(D2D1::ColorF(r, g, b, a));
        pRT->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(x, y), radius, radius), brush, 3.0f);
    }
}

void Graphics::OnPaintRenderTarget(float x, float y, int spriteIdx, int frameIdx, float fOpaque) {
    if (spriteIdx < 0 || spriteIdx >= SPRITE_MAX || frameIdx < 0 || frameIdx >= SPRITE_IMGMAXPERCHARACTER) return;

    ID2D1RenderTarget* pRT = GetCurrentRenderTarget();
    if (!pRT || !bitmap[spriteIdx][frameIdx]) { 
        return;
    }

    D2D1_SIZE_F imageSize = bitmap[spriteIdx][frameIdx]->GetSize();
    D2D1_RECT_F destRect = D2D1::RectF(
        x - imageSize.width / 2.0f,
        y - imageSize.height / 2.0f,
        x + imageSize.width / 2.0f,
        y + imageSize.height / 2.0f
    );
    pRT->DrawBitmap(bitmap[spriteIdx][frameIdx], &destRect, fOpaque, D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR);
}

void Graphics::OnPaintText(const WCHAR* string, float x, float y, int nFontCol) {
    ID2D1RenderTarget* pRT = GetCurrentRenderTarget();
    if (!pRT) return;

    int formatIndex = nFontCol / FONTCOL;
    int colorIndex = nFontCol % FONTCOL;

    if (formatIndex < 0 || formatIndex >= FONTFORMAT || !pWFormat[formatIndex] ||
        colorIndex < 0 || colorIndex >= FONTCOL || !Wbrush[colorIndex]) {
        return;
    }

    UINT32 length = (UINT32)wcslen(string);
    D2D1_RECT_F layoutRect = D2D1::RectF(x, y, x + static_cast<float>(m_currentWidth), y + static_cast<float>(m_currentHeight));
    pRT->DrawTextW(string, length, pWFormat[formatIndex], layoutRect, Wbrush[colorIndex]);
}

void Graphics::StartFadeIn(float duration, D2D1_COLOR_F color) {
    m_fadeState = FadeState::FADE_IN_PROGRESS;
    m_fadeColor = color;
    m_fadeDuration = duration > 0.0f ? duration : 0.001f;
    m_fadeElapsedTime = 0.0f;
    m_currentFadeAlpha = 1.0f;
    if (m_fadeBrush) {
        m_fadeBrush->SetColor(m_fadeColor);
    }
    OutputDebugString(L"Graphics: StartFadeIn called.\n");
}

void Graphics::StartFadeOut(float duration, D2D1_COLOR_F color) {
    m_fadeState = FadeState::FADE_OUT_PROGRESS;
    m_fadeColor = color;
    m_fadeDuration = duration > 0.0f ? duration : 0.001f;
    m_fadeElapsedTime = 0.0f;
    m_currentFadeAlpha = 0.0f;
    if (m_fadeBrush) {
        m_fadeBrush->SetColor(m_fadeColor);
    }
    OutputDebugString(L"Graphics: StartFadeOut called.\n");
}

void Graphics::UpdateFadeState(float timeDelta) {
    if (m_fadeState == FadeState::FADE_IN_PROGRESS) {
        m_fadeElapsedTime += timeDelta;
        float progress = m_fadeElapsedTime / m_fadeDuration;
        m_currentFadeAlpha = 1.0f - (std::min)(progress, 1.0f);

        if (progress >= 1.0f) {
            m_fadeState = FadeState::FADE_IN_COMPLETE;
            m_currentFadeAlpha = 0.0f;
            OutputDebugString(L"Graphics: FadeIn complete.\n");
        }
    } else if (m_fadeState == FadeState::FADE_OUT_PROGRESS) {
        m_fadeElapsedTime += timeDelta;
        float progress = m_fadeElapsedTime / m_fadeDuration;
        m_currentFadeAlpha = (std::min)(progress, 1.0f);

        if (progress >= 1.0f) {
            m_fadeState = FadeState::FADE_OUT_COMPLETE;
            m_currentFadeAlpha = 1.0f;
            OutputDebugString(L"Graphics: FadeOut complete.\n");
        }
    }
}

void Graphics::RenderFadeOverlay() {
    if (m_fadeState == FadeState::FADE_IN_PROGRESS || m_fadeState == FadeState::FADE_OUT_PROGRESS || m_fadeState == FadeState::FADE_OUT_COMPLETE) {
        ID2D1RenderTarget* pRT = GetCurrentRenderTarget();
        if (pRT && m_fadeBrush) {
            D2D1_MATRIX_3X2_F previousTransform;
            pRT->GetTransform(&previousTransform);
            pRT->SetTransform(D2D1::Matrix3x2F::Identity());

            m_fadeBrush->SetOpacity(m_currentFadeAlpha);
            D2D1_RECT_F screenRect = D2D1::RectF(0.0f, 0.0f, static_cast<float>(m_currentWidth), static_cast<float>(m_currentHeight));
            pRT->FillRectangle(&screenRect, m_fadeBrush);

            pRT->SetTransform(&previousTransform);
        }
    }
}

bool Graphics::IsFading() const {
    return m_fadeState == FadeState::FADE_IN_PROGRESS || m_fadeState == FadeState::FADE_OUT_PROGRESS;
}

FadeState Graphics::GetFadeState() const {
    return m_fadeState;
}