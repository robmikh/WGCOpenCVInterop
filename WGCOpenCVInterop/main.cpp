#include "pch.h"

namespace winrt
{
    using namespace Windows::Foundation;
    using namespace Windows::Graphics::Capture;
    using namespace Windows::Graphics::DirectX;
    using namespace Windows::Graphics::DirectX::Direct3D11;
}
    
winrt::GraphicsCaptureItem GetCaptureItemForPrimaryMonitor();
winrt::com_ptr<ID3D11Texture2D> CaptureSingleFrame(
    winrt::com_ptr<ID3D11Device> const& d3d11Device,
    winrt::GraphicsCaptureItem const& item);

int wmain()
{
    // Initialize COM
    winrt::init_apartment(winrt::apartment_type::multi_threaded);

    // Create our D3D11 device
    auto d3d11Device = util::CreateD3DDevice();
    
    // Capture the screen
    auto item = GetCaptureItemForPrimaryMonitor();
    auto texture = CaptureSingleFrame(d3d11Device, item);

    // Initialize OpenCV
    auto cvContext = cv::directx::ocl::initializeContextFromD3D11Device(d3d11Device.get());
    
    return 0;
}

winrt::GraphicsCaptureItem GetCaptureItemForPrimaryMonitor()
{
    auto monitor = MonitorFromWindow(GetDesktopWindow(), MONITOR_DEFAULTTOPRIMARY);
    auto interopFactory = winrt::get_activation_factory<
        winrt::GraphicsCaptureItem,
        IGraphicsCaptureItemInterop>();

    winrt::GraphicsCaptureItem item{ nullptr };
    winrt::check_hresult(interopFactory->CreateForMonitor(
        monitor,
        winrt::guid_of<winrt::GraphicsCaptureItem>(),
        winrt::put_abi(item)));

    return item;
}

// This method is meant for one-off screen captures. Do not use this in a loop
// if you want to get continual frames at high speed.
winrt::com_ptr<ID3D11Texture2D> CaptureSingleFrame(
    winrt::com_ptr<ID3D11Device> const& d3d11Device,
    winrt::GraphicsCaptureItem const& item)
{
    auto device = CreateDirect3DDevice(d3d11Device);
    auto itemSize = item.Size();

    // Because we want to synchronously wait for a frame, we use CreateFreeThreaded
    // instead of Create here. By doing this, the FrameArrived event is fired from
    // the frame pool's internal thread, and the calling thread doesn't need to 
    // pump messages. Check the Win32CaptureSample README for more details.
    auto framePool = winrt::Direct3D11CaptureFramePool::CreateFreeThreaded(
        device,
        winrt::DirectXPixelFormat::B8G8R8A8UIntNormalized,
        1,
        itemSize);
    auto session = framePool.CreateCaptureSession(item);
    session.IsCursorCaptureEnabled(false);

    // Setup the event we'll use to wait for the frame and the FrameArrived callback.
    winrt::Direct3D11CaptureFrame frame{ nullptr };
    wil::shared_event captureEvent(wil::EventOptions::None);
    framePool.FrameArrived([&frame, captureEvent](auto&& framePool, auto&&)
        {
            frame = framePool.TryGetNextFrame();
            captureEvent.SetEvent();
        });

    // Start the capture and wait
    session.StartCapture();
    captureEvent.wait(2000);

    // Stop the capture
    framePool.Close();
    session.Close();

    // Extract the ID3D11Texture2D
    auto surface = frame.Surface();
    auto texture = GetDXGIInterfaceFromObject<ID3D11Texture2D>(surface);

    return texture;
}