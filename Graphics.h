#pragma once
#include <Windows.h>
#include <d2d1_1.h>
#include <dwrite.h>   // 기본 DirectWrite
#include <dwrite_1.h> // IDWriteFactory1 등
#include <dwrite_2.h> // IDWriteFactory2 등
#include <dwrite_3.h> // IDWriteFactory3, IDWriteFontSet, IDWriteFontSetBuilder, IDWriteFontCollection1 등
#include <wincodec.h>
#include <d3d11_1.h> 
#include <dxgi1_2.h>  

#pragma comment(lib, "d2d1")
#pragma comment(lib, "dwrite")
#pragma comment(lib, "d3d11")
#pragma comment(lib, "dxgi")

#define SPRITE_MAX 16
#define SPRITE_IMGMAXPERCHARACTER 8
#define FONTCOL 8
#define FONTFORMAT 4
#define SCREEN_X 640 
#define SCREEN_Y 360 

enum class FadeState {
    FADE_NONE,
    FADE_IN_PROGRESS,
    FADE_OUT_PROGRESS,
    FADE_IN_COMPLETE,
    FADE_OUT_COMPLETE
};

class Graphics
{
private:
    ID2D1Factory* factory; 
    IDWriteFactory* pWFactory; 

    ID2D1HwndRenderTarget* rendertarget;    
    ID2D1RenderTarget* m_pTextureRT;        

    ID2D1SolidColorBrush* brush;            
    ID2D1SolidColorBrush* Wbrush[FONTCOL];  

    ID2D1Bitmap* bitmap[SPRITE_MAX][SPRITE_IMGMAXPERCHARACTER]; 

    IDWriteTextFormat* pWFormat[FONTFORMAT]; 
    IDWriteFontCollection1* m_pCustomFontCollection; // 타입을 IDWriteFontCollection1* 로 변경

    ID3D11Device* m_pD3DDevice;
    ID3D11DeviceContext* m_pD3DImmediateContext;
    IDXGISwapChain1* m_pSwapChain;
    ID3D11Texture2D* m_pBackBuffer;

    HWND m_hWnd;
    bool m_isFullscreen;
    UINT m_currentWidth;
    UINT m_currentHeight;
    bool m_initialized;
    bool m_resourcesNeedReload;

    void CreateDeviceIndependentResources();
    void ReleaseDeviceIndependentResources();

    HRESULT CreateDeviceResources();
    void ReleaseDeviceResources();

    HRESULT RecreateWindowSizeDependentResources();
    void ReleaseWindowSizeDependentResources();

    ID2D1RenderTarget* GetCurrentRenderTarget(); 

    HRESULT CreateSpecificBrushes(ID2D1RenderTarget* pRT);
    HRESULT CreateTextFormatsInternal(); 
    void ReleaseBitmaps();

    FadeState m_fadeState;
    D2D1_COLOR_F m_fadeColor;
    float m_fadeDuration;
    float m_fadeElapsedTime;
    float m_currentFadeAlpha;
    ID2D1SolidColorBrush* m_fadeBrush; 


public:
    Graphics();
    ~Graphics();

    bool Init(HWND hWindow); 

    bool MyLoadimage(int i, int j, const wchar_t* imgpath);
    bool MyUnLoadimage(int i, int j);

    void BeginDraw(); 
    void EndDraw();   

    void ClearScreen(float r, float g, float b);
    void DrawCircle(float x, float y, float radius, float r, float g, float b, float a);
    void OnPaintRenderTarget(float x, float y, int i, int j, float fOpaque = 1.0f);
    void OnPaintText(const WCHAR* string, float x, float y, int nFontCol);

    void SetFullscreen(bool enable);
    bool IsFullscreen() const { return m_isFullscreen; }
    void Resize(UINT width, UINT height); 
    void Cleanup(); 

    bool NeedsResourceReload() const;
    void MarkResourcesAsLoaded();

    void StartFadeIn(float duration, D2D1_COLOR_F color = D2D1::ColorF(D2D1::ColorF::Black));
    void StartFadeOut(float duration, D2D1_COLOR_F color = D2D1::ColorF(D2D1::ColorF::Black));
    void UpdateFadeState(float timeDelta); 
    void RenderFadeOverlay();      
    bool IsFading() const;         
    FadeState GetFadeState() const; 
};
