// main_thread_dispatcher.cpp

#include "main_thread_dispatcher.h"

MainThreadDispatcher& MainThreadDispatcher::Instance() {
    static MainThreadDispatcher instance;
    return instance;
}

bool MainThreadDispatcher::Initialize() {
    if (!dispatcher_queue_) {
        DispatcherQueueOptions options{
                sizeof(DispatcherQueueOptions), DQTYPE_THREAD_CURRENT, DQTAT_COM_STA
        };
        if (SUCCEEDED(CreateDispatcherQueueController(options, &controller_))) {
            dispatcher_queue_ = winrt::Windows::System::DispatcherQueue::GetForCurrentThread();
        }
    }
    return dispatcher_queue_ != nullptr;
}

void MainThreadDispatcher::RunOnMainThread(std::function<void()> func) {
    if (dispatcher_queue_) {
        dispatcher_queue_.TryEnqueue([func]() { func(); });
    }
}
