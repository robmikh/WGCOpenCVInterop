#pragma once

namespace util
{
    inline auto CreateD3DDevice(D3D_DRIVER_TYPE const type, UINT flags, winrt::com_ptr<ID3D11Device>& device)
    {
        WINRT_ASSERT(!device);

        return D3D11CreateDevice(nullptr, type, nullptr, flags, nullptr, 0, D3D11_SDK_VERSION, device.put(),
            nullptr, nullptr);
    }

    inline auto CreateD3DDevice(UINT flags = D3D11_CREATE_DEVICE_BGRA_SUPPORT)
    {
        winrt::com_ptr<ID3D11Device> device;
        HRESULT hr = CreateD3DDevice(D3D_DRIVER_TYPE_HARDWARE, flags, device);
        if (DXGI_ERROR_UNSUPPORTED == hr)
        {
            hr = CreateD3DDevice(D3D_DRIVER_TYPE_WARP, flags, device);
        }

        winrt::check_hresult(hr);
        return device;
    }
}