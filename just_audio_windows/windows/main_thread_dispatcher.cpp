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
    std::wcout << L"[just_audio_windows]: MainThreadDispatcher::Initialize() called" << std::endl;
    if (controller_ != nullptr) {
        std::wcout << L"[just_audio_windows]: controller_ not null during MainThreadDispatcher initialize" << std::endl;
        return true; // Already initialized
    }

    bool init_success = false;

    // Run on a background thread
    std::thread dispatcher_init_thread([&]() {
        DispatcherQueueOptions options{
                sizeof(DispatcherQueueOptions),
                DQTYPE_THREAD_CURRENT,
                DQTAT_COM_NONE
        };

        winrt::Windows::System::DispatcherQueueController temp_controller{ nullptr };
        HRESULT hr = CreateDispatcherQueueController(options, reinterpret_cast<PDISPATCHERQUEUECONTROLLER*>(winrt::put_abi(temp_controller)));

        if (FAILED(hr)) {
            std::wcout << L"[just_audio_windows]: Failed to create DispatcherQueueController. HRESULT: " << std::hex << hr << std::endl;
        } else {
            std::wcout << L"[just_audio_windows]: [just_audio_windows]: Successfully created DispatcherQueueController on background thread" << std::endl;
            controller_ = temp_controller;
            dispatcher_queue_ = controller_.DispatcherQueue();
            init_success = dispatcher_queue_ != nullptr;
        }
    });

    dispatcher_init_thread.join(); // Wait for the thread to complete

    return init_success;
}

void MainThreadDispatcher::RunOnMainThread(std::function<void()> func) {
    if (dispatcher_queue_ != nullptr) {
        std::wcout << L"[just_audio_windows]: RunOnMainThread - dispatcher_queue_ TryEnqueue called" << std::endl;
        dispatcher_queue_.TryEnqueue(std::move(func));
    } else {
        std::wcout << L"[just_audio_windows]: dispatcher_queue_ is null" << std::endl;
    }
}