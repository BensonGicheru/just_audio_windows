#ifndef MAIN_THREAD_DISPATCHER_H_
#define MAIN_THREAD_DISPATCHER_H_

#include <functional>
#include <windows.h>
#include <winrt/Windows.System.h>
#include <DispatcherQueue.h>

class MainThreadDispatcher {
public:
    // Singleton instance
    static MainThreadDispatcher& Instance();

    // Initialize the dispatcher, called once
    bool Initialize();

    // Runs a function on the main thread
    void RunOnMainThread(std::function<void()> func);

private:
    // Use DispatcherQueueController directly instead of IDispatcherQueueController
    winrt::com_ptr<IDispatcherQueueController> controller_{ nullptr };
    winrt::Windows::System::DispatcherQueue dispatcher_queue_{ nullptr };

    // Private constructor and destructor for singleton
    MainThreadDispatcher();
    ~MainThreadDispatcher() = default;

    // Prevent copying
    MainThreadDispatcher(const MainThreadDispatcher&) = delete;
    MainThreadDispatcher& operator=(const MainThreadDispatcher&) = delete;
};

#endif  // MAIN_THREAD_DISPATCHER_H_
