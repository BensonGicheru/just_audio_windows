#ifndef MAIN_THREAD_DISPATCHER_H_
#define MAIN_THREAD_DISPATCHER_H_

#include <windows.h>
#include <functional>
#include <winrt/Windows.System.h>
#include <DispatcherQueue.h> // Required for DispatcherQueue

using namespace winrt::Windows::System;

class MainThreadDispatcher {
public:
    // Singleton instance of MainThreadDispatcher
    static MainThreadDispatcher& Instance();

    // Initializes the dispatcher, should be called once
    bool Initialize();

    // Runs a function on the main thread
    void RunOnMainThread(std::function<void()> func);

private:
    // Private members for dispatcher and controller
    winrt::Windows::System::DispatcherQueue dispatcher_queue_{ nullptr };
    winrt::com_ptr<IDispatcherQueueController> controller_{ nullptr };

    // Private constructor and destructor for singleton pattern
    MainThreadDispatcher();
    ~MainThreadDispatcher() = default;

    // Prevent copying and assignment
    MainThreadDispatcher(const MainThreadDispatcher&) = delete;
    MainThreadDispatcher& operator=(const MainThreadDispatcher&) = delete;
};

#endif  // MAIN_THREAD_DISPATCHER_H_
