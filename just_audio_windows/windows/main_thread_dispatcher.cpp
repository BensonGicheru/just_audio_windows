#include "include/just_audio_windows/main_thread_dispatcher.h"

MainThreadDispatcher& MainThreadDispatcher::Instance() {
    static MainThreadDispatcher instance;
    return instance;
}

bool MainThreadDispatcher::Initialize() {
    if (dispatcher_queue_ != nullptr) {
        return true;  // Already initialized
    }

    DispatcherQueueOptions options = {
            sizeof(DispatcherQueueOptions),
            DQTYPE_THREAD_CURRENT,
            DQTAT_COM_STA
    };

    // Use put_void() to get a raw pointer compatible with CreateDispatcherQueueController
    HRESULT hr = CreateDispatcherQueueController(options, reinterpret_cast<PDISPATCHERQUEUECONTROLLER*>(controller_.put_void()));
    if (SUCCEEDED(hr)) {
        dispatcher_queue_ = controller_->DispatcherQueue();
        return true;
    } else {
        return false;  // Initialization failed
    }
}

void MainThreadDispatcher::RunOnMainThread(std::function<void()> func) {
    if (dispatcher_queue_) {
        dispatcher_queue_.TryEnqueue([func]() { func(); });
    }
}

MainThreadDispatcher::MainThreadDispatcher() {
    Initialize();
}
