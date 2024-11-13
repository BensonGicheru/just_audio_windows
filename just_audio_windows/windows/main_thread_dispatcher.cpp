#include "include/just_audio_windows/main_thread_dispatcher.h"

// Singleton instance
MainThreadDispatcher& MainThreadDispatcher::Instance() {
    static MainThreadDispatcher instance;
    return instance;
}

// Initialize the dispatcher
bool MainThreadDispatcher::Initialize() {
    if (dispatcher_queue_ != nullptr) {
        return true;  // Already initialized
    }

    // Create the DispatcherQueueController
    DispatcherQueueOptions options = { sizeof(DispatcherQueueOptions), DQTYPE_THREAD_CURRENT, DQTAT_COM_STA };
    HRESULT hr = CreateDispatcherQueueController(options, reinterpret_cast<PDISPATCHERQUEUECONTROLLER*>(controller_.put()));
    if (FAILED(hr)) {
        return false;  // Initialization failed
    }

    // Get the DispatcherQueue from the controller
    dispatcher_queue_ = controller_->DispatcherQueue();
    return true;
}

// Run a function on the main thread
void MainThreadDispatcher::RunOnMainThread(std::function<void()> func) {
    if (dispatcher_queue_) {
        // Enqueue the task to run on the main thread
        dispatcher_queue_.TryEnqueue([func]() { func(); });
    }
}

// Constructor to initialize the dispatcher on object creation
MainThreadDispatcher::MainThreadDispatcher() {
    Initialize();
}
