#pragma once
#include <IPCMessaging.h>
#include <openxr/openxr.h>
#include <Log.h>

DECLARE_LOG_CATEGORY(FLogOpenXRInterface);

struct FXRFunctionInfo
{
    PFN_xrVoidFunction Address = nullptr;
    const char* Extension = nullptr;
};

extern FXRFunctionInfo FindXRFunction(const char* FunctionName);
extern bool IsXRFunctionImplemented(const char* FunctionName);

extern WyvernEngine::FString GetResultString(XrResult Result);
extern WyvernEngine::FString GetStructString(XrStructureType Type);

template<class T>
T* FindInputInChain(const void* ChainPtr, XrStructureType InputType)
{
    while (ChainPtr != nullptr)
    {
        const XrBaseInStructure* RawStruct = static_cast<const XrBaseInStructure*>(ChainPtr);
        if (RawStruct->type == InputType)
        {
            return (T*)RawStruct;
        }
        ChainPtr = RawStruct->next;
    }
    return nullptr;
}

#define XRT_CAST_PTR_TO_OXR_HANDLE(HANDLE_TYPE, PTR) ((HANDLE_TYPE)(uint64_t)(uintptr_t)(PTR))
#define XRT_CAST_OXR_HANDLE_TO_PTR(PTR_TYPE, HANDLE) ((PTR_TYPE)(uintptr_t)(uint64_t)(HANDLE))

#define PRINT_XR_RESULT_ERROR(LogCategory, Function, Result) Log(LogCategory, Warning, "%s returned %s", #Function, *GetResultString(Result))
#define REPORT_XR_RESULT_ERROR(LogCategory, Function, Result) PRINT_XR_RESULT_ERROR(LogCategory, Function, Result); return Result;
#define CHECK_XR_RESULT_ERROR(LogCategory, Function, Result) if (Result != XR_SUCCESS) { PRINT_XR_RESULT_ERROR(LogCategory, Function, Result); } return Result;
