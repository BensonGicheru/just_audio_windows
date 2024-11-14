#include "include/just_audio_windows/main_thread_dispatcher.h"

#include <iostream>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.System.h>
#include <DispatcherQueue.h>


// Singleton instance

MainThreadDispatcher::MainThreadDispatcher() = default;

MainThreadDispatcher& MainThreadDispatcher::Instance() {
    static MainThreadDispatcher instance;
    return instance;
}

bool MainThreadDispatcher::Initialize() {
    if (controller_ != nullptr) {
        return true; // Already initialized
    }

    DispatcherQueueOptions options{
            sizeof(DispatcherQueueOptions),
            DQTYPE_THREAD_CURRENT,
            DQTAT_COM_NONE
    };

    // Use the DispatcherQueueController directly without casting to IDispatcherQueueController
//    winrt::Windows::System::DispatcherQueueController temp_controller{ nullptr };
//    HRESULT hr = CreateDispatcherQueueController(options, reinterpret_cast<PDISPATCHERQUEUECONTROLLER*>(winrt::put_abi(temp_controller)));
//
//    if (FAILED(hr)) {
//        return false; // Failed to create dispatcher controller
//    }
//
//    // Assign the controller
//    controller_ = temp_controller;

    // Create a temp controller as a com_ptr for IDispatcherQueueController
    winrt::com_ptr<winrt::Windows::System::IDispatcherQueueController> temp_controller;

    // Create the dispatcher queue controller
    // Here we use `put_void()` which is specifically designed to return a raw pointer to the controller
    HRESULT hr = CreateDispatcherQueueController(options, temp_controller.put_void());

    if (FAILED(hr)) {
        // Log the HRESULT error for debugging
        std::wcout << L"Failed to create DispatcherQueueController. HRESULT: " << std::hex << hr << std::endl;
        return false; // Failed to create dispatcher controller
    }

    // Since controller_ is of type `winrt::Windows::System::DispatcherQueueController`
    // we can safely assign `temp_controller` (which is a `com_ptr<IDispatcherQueueController>`) to it
    controller_ = temp_controller.as<winrt::Windows::System::DispatcherQueueController>();

    // Retrieve and assign the dispatcher queue
    dispatcher_queue_ = controller_.DispatcherQueue();

    return dispatcher_queue_ != nullptr;
}

void MainThreadDispatcher::RunOnMainThread(std::function<void()> func) {
    if (dispatcher_queue_ != nullptr) {
        dispatcher_queue_.TryEnqueue(std::move(func));
    } else {
        std::wcout << L"[just_audio_windows]: dispatcher_queue_ is null" << std::endl;
    }
}