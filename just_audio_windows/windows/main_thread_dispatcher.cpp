#include "include/just_audio_windows/main_thread_dispatcher.h"

#include <iostream>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.System.h>
#include <DispatcherQueue.h>
#include <thread>

// Singleton instance

MainThreadDispatcher::MainThreadDispatcher() = default;

MainThreadDispatcher& MainThreadDispatcher::Instance() {
    static MainThreadDispatcher instance;
    return instance;
}

bool MainThreadDispatcher::Initialize() {
    std::wcout << L"MainThreadDispatcher::Initialize() called. Current Thread ID: " << GetCurrentThreadId() << std::endl;
    if (controller_ != nullptr) {
        std::wcout << L"[just_audio_windows]: controller_ not null during MainThreadDispatcher initialize" << std::endl;
        return true; // Already initialized
    }

    // Ensure COM is initialized (needed for DispatcherQueue)
    HRESULT hr_com_init = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
    if (FAILED(hr_com_init)) {
        std::wcout << L"[just_audio_windows]: COM initialization failed, HRESULT: " << std::hex << hr_com_init << std::endl;
        return false; // If COM initialization fails, return early
    }

    DispatcherQueueOptions options{
            sizeof(DispatcherQueueOptions),
            DQTYPE_THREAD_CURRENT,  // This associates the current thread with the DispatcherQueue
            DQTAT_COM_NONE
    };

    winrt::Windows::System::DispatcherQueueController temp_controller{ nullptr };
    HRESULT hr = CreateDispatcherQueueController(options, reinterpret_cast<PDISPATCHERQUEUECONTROLLER*>(winrt::put_abi(temp_controller)));

    if (FAILED(hr)) {
        std::wcout << L"[just_audio_windows]: Failed to create DispatcherQueueController. HRESULT: " << std::hex << hr << std::endl;
        CoUninitialize();  // Uninitialize COM
        return false; // Failed to create dispatcher controller
    } else {
        std::wcout << L"[just_audio_windows]: Successfully created DispatcherQueueController" << std::endl;
    }

    // Assign the controller and dispatcher queue
    controller_ = temp_controller;
    dispatcher_queue_ = controller_.DispatcherQueue();

    CoUninitialize();  // Uninitialize COM after use

    return dispatcher_queue_ != nullptr;
}

void MainThreadDispatcher::RunOnMainThread(std::function<void()> func) {
    if (dispatcher_queue_ != nullptr) {
        std::wcout << L"[just_audio_windows]: RunOnMainThread - dispatcher_queue_ TryEnqueue called" << std::endl;
        bool enqueued = dispatcher_queue_.TryEnqueue(std::move(func));
        if (!enqueued) {
            std::wcout << L"[just_audio_windows]: RunOnMainThread dispatcher_queue_ Failed to enqueue task" << std::endl;
        } else {
            std::wcout << L"[just_audio_windows]: RunOnMainThread dispatcher_queue_ Task enqueued" << std::endl;
        }
    } else {
        std::wcout << L"[just_audio_windows]: dispatcher_queue_ is null" << std::endl;
    }
}