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
        std::wcout << L"controller_ not null " << std::endl;
        return true; // Already initialized
    }

    DispatcherQueueOptions options{
            sizeof(DispatcherQueueOptions),
            DQTYPE_THREAD_CURRENT,
            DQTAT_COM_NONE
    };

    // Use the DispatcherQueueController directly without casting to IDispatcherQueueController
    HRESULT hr = CreateDispatcherQueueController(options, reinterpret_cast<PDISPATCHERQUEUECONTROLLER*>(winrt::put_abi(controller_)));

    if (FAILED(hr)) {
        std::wcout << L"Failed to create DispatcherQueueController. HRESULT: " << std::hex << hr << std::endl;
        return false; // Failed to create dispatcher controller
    }

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