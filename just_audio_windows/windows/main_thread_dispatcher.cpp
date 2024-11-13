#include "include/just_audio_windows/main_thread_dispatcher.h"

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

    // Create a DispatcherQueueController instance
    DispatcherQueueOptions options{};
//    HRESULT hr = CreateDispatcherQueueController(options, controller_.put());
    HRESULT hr = CreateDispatcherQueueController(options, reinterpret_cast<PDISPATCHERQUEUECONTROLLER*>(controller_.put()));
    if (FAILED(hr)) {
        return false; // Failed to create dispatcher controller
    }

    // Get the DispatcherQueue from the controller
    dispatcher_queue_ = controller_->DispatcherQueue();

    return true; // Successfully initialized
}

void MainThreadDispatcher::RunOnMainThread(std::function<void()> func) {
    // Make sure the dispatcher queue is initialized
    if (dispatcher_queue_ != nullptr) {
        dispatcher_queue_.TryEnqueue(std::move(func));
    }
}