#pragma once

#include <vector>
#include <chrono>
#include <mutex>

#include "openxr_includes.h"
#include "window.h"
#include "swapchain.h"
#include "compositor.h"

#include "srhelpers.h"
#include "weaver_directx_12.h"

XrResult xrCreateSession(XrInstance instance, const XrSessionCreateInfo* createInfo, XrSession* session);
XrResult xrDestroySession(XrSession session);

XrResult xrBeginSession(XrSession session, const XrSessionBeginInfo* beginInfo);
XrResult xrEndSession(XrSession session);
XrResult xrRequestExitSession(XrSession session);

// Frames
XrResult xrWaitFrame(XrSession session, const XrFrameWaitInfo* frameWaitInfo, XrFrameState* frameState);
XrResult xrBeginFrame(XrSession session, const XrFrameBeginInfo* frameBeginInfo);
XrResult xrEndFrame(XrSession session, const XrFrameEndInfo* frameEndInfo);

namespace ch = std::chrono;

namespace XRGameBridge {
    enum FrameState {
        NewFrameAllowed,
        NewFrameBusy,
        Waiting,
        Rendering,
        Ended
    };

    struct GB_Session {
        XrSession id;
        XrInstance instance;
        XrSystemId system;
        XrSessionState session_state;
        XrViewConfigurationType view_configuration;
        XrSwapchain swap_chain;
        //std::vector<XrActionSet> g_action_sets;

        //std
        std::chrono::high_resolution_clock::time_point session_epoch;
        FrameState wait_frame_state;
        std::mutex wait_frame_state_mutex;

        // DirectX 12
        ComPtr<ID3D12Device> d3d12_device;
        ComPtr<ID3D12CommandQueue> command_queue;
        GB_Compositor compositor;
        GB_ProxySwapchain intermediate_resource;

        // Windows
        GB_Display display;
        GB_GraphicsDevice window_swapchain;

        // SR
        SR::SRContext* sr_context;
        DirectX12Weaver* d3d12weaver;
    };

    class GB_FrameTimer {
        uint32_t frame_index = 0;
        FrameState state = Waiting;

        std::chrono::high_resolution_clock::time_point last_frame_start;
        std::chrono::high_resolution_clock::duration last_frame_time;
        std::chrono::high_resolution_clock::duration next_frame_time;

        GB_FrameTimer() = delete;
        GB_FrameTimer(GB_FrameTimer& other) = delete;
        GB_FrameTimer(GB_FrameTimer&& other) = delete;

        explicit GB_FrameTimer(uint32_t frame_index) : frame_index(frame_index) {
        }

        void StartNewFrame(uint32_t frame_id) {
        }

        void EndFrame(uint32_t frame_id) {
        }

        uint64_t GetNextRenderTimeNanoseconds() {
            //auto display_time = ch::high_resolution_clock::now() - last_frame_start;
            //next_frame_time = ch::high_resolution_clock::now() + ch::milliseconds(33);
            //last_frame_start = ch::high_resolution_clock::now();
            //return ch::time_point_cast<ch::nanoseconds>(ch::high_resolution_clock::now() + ch::milliseconds(3));
        }

        uint64_t GetFrameTime() {
            //last_frame_time = ch::high_resolution_clock::now() - last_frame;
            //return ch::duration_cast<ch::nanoseconds>(last_frame_time).count();
        }
    };

    inline std::vector<GB_FrameTimer> g_frames;

    void ChangeSessionState(GB_Session& session, XrSessionState state);
}
