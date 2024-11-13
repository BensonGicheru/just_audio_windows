#include "include/just_audio_windows/main_thread_dispatcher.h"

MainThreadDispatcher& MainThreadDispatcher::Instance() {
    static MainThreadDispatcher instance;
    return instance;
}

bool MainThreadDispatcher::Initialize() {
    if (dispatcher_queue_ != nullptr) {
        return true;  // Already initialized
    }

    // Create the DispatcherQueueController
    DispatcherQueueOptions options = { sizeof(DispatcherQueueOptions), DQTYPE_THREAD_CURRENT, DQTAT_COM_STA };
    HRESULT hr = CreateDispatcherQueueController(options, controller_.put_void());
    if (FAILED(hr)) {
        return false;  // Initialization failed
    }

    // Get the DispatcherQueue from the controller
    dispatcher_queue_ = controller_->DispatcherQueue();
    return true;
}

void MainThreadDispatcher::RunOnMainThread(std::function<void()> func) {
    if (dispatcher_queue_) {
        // Try to enqueue the task to run on the main thread
        dispatcher_queue_.TryEnqueue([func]() { func(); });
    }
}

MainThreadDispatcher::MainThreadDispatcher() {
    Initialize();
}
