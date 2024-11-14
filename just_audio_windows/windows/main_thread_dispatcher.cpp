#include "include/just_audio_windows/main_thread_dispatcher.h"

#include <winrt/Windows.Foundation.h>
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

    // Now using DispatcherQueueController directly for controller_
    HRESULT hr = CreateDispatcherQueueController(options, reinterpret_cast<ABI::Windows::System::DispatcherQueueController**>(winrt::put_abi(controller_)));
    if (FAILED(hr)) {
        return false; // Failed to create dispatcher controller
    }

    // Retrieve the dispatcher queue from the controller
    dispatcher_queue_ = controller_.DispatcherQueue();

    return dispatcher_queue_ != nullptr;
}

void MainThreadDispatcher::RunOnMainThread(std::function<void()> func) {
    if (dispatcher_queue_ != nullptr) {
        dispatcher_queue_.TryEnqueue(std::move(func));
    }
}