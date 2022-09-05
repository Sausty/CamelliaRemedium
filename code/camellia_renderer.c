#include "camellia_renderer.h"

renderer_state Renderer;

void RendererLoad()
{
#if defined(RENDERER_D3D12)
    Renderer.InitBuffer = D3D12InitBuffer;
    Renderer.UploadBuffer = D3D12UploadBuffer;
    Renderer.FreeBuffer = D3D12FreeBuffer;

    Renderer.Init = D3D12Init;
    Renderer.Exit = D3D12Exit;

    Renderer.Begin = D3D12Begin;
    Renderer.End = D3D12End;

    Renderer.BindBuffer = D3D12BindBuffer;

	Renderer.PushTransform = D3D12PushTransform;
    Renderer.Draw = D3D12Draw;
	Renderer.DrawIndexed = D3D12DrawIndexed;

    Renderer.Wait = D3D12Wait;
    Renderer.Resize = D3D12Resize;
#endif
}