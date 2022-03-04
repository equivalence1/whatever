#pragma once

#include <NvInfer.h>
#include <NvInferRuntimeCommon.h>

#include <iostream>
#include <type_traits>
#include <utility>
#include <string>


void ensureCondition(bool cond, const char* condStr, const char* file, uint32_t line, const std::string& msg, bool addMsg);

#define ENSURE(cond, msg) ensureCondition(!!(cond), #cond, __FILE__, __LINE__, msg, true)
#define ENSURE(cond) ensureCondition(!!(cond), #cond, __FILE__, __LINE__, "", false)


class TLogger final : public nvinfer1::ILogger {
    void log(Severity severity, const char* msg) noexcept override {
        // suppress info-level messages
        if (severity <= Severity::kWARNING) {
            std::cout << msg << std::endl;
        }
    }
};

template <typename T>
class ITrtHolderBase {
public:
    ITrtHolderBase() = delete;
    ITrtHolderBase(const ITrtHolderBase&) = delete;
    ITrtHolderBase& operator=(const ITrtHolderBase&) = delete;

    ITrtHolderBase(T* object) noexcept {
        *this = object;
    }

    ITrtHolderBase(ITrtHolderBase&& other) noexcept {
        Object_ = std::exchange(other.Object_, nullptr);
    }

    ITrtHolderBase& operator=(ITrtHolderBase&& other) noexcept {
        std::swap(Object_, other.Object_);
        return *this;
    }

    ITrtHolderBase& operator=(T* object) noexcept {
        Object_ = object;
    }

    T& operator*() & noexcept {
        return *Object_;
    }

    const T& operator*() const& noexcept {
        return *Object_;
    }

    T* operator&() & noexcept {
        return Object_;
    }

    const T* operator&() const& noexcept {
        return Object_;
    }

    T* operator->() & {
        return Object_;
    }

    const T* operator->() const& {
        return Object_;
    }

    operator bool() const {
        return Object_ != nullptr;
    }

protected:
    virtual ~ITrtHolderBase() {};

protected:
    T* Object_;
};

template <typename T, typename = void>
class TTrtHolder : public ITrtHolderBase<T> {
public:
    TTrtHolder(T* object) : ITrtHolderBase<T>(object) {
    }

    TTrtHolder(TTrtHolder&& other) : ITrtHolderBase<T>(std::move(other)) {
    }

    ~TTrtHolder() override {
        if (ITrtHolderBase<T>::Object_ == nullptr) {
            return;
        }

        ITrtHolderBase<T>::Object_->~T();
    }
};

template <typename T>
class TTrtHolder<T, typename std::enable_if<std::is_member_function_pointer<decltype(&T::destroy)>::value>::type> : public ITrtHolderBase<T> {
public:
    TTrtHolder<T, typename std::enable_if<std::is_member_function_pointer<decltype(&T::destroy)>::value>::type>(T* object) : ITrtHolderBase<T>(object) {
    }

    TTrtHolder<T, typename std::enable_if<std::is_member_function_pointer<decltype(&T::destroy)>::value>::type>(TTrtHolder&& other) : ITrtHolderBase<T>(std::move(other)) {
    }

    ~TTrtHolder<T, typename std::enable_if<std::is_member_function_pointer<decltype(&T::destroy)>::value>::type>() override {
        if (ITrtHolderBase<T>::Object_ == nullptr) {
            return;
        }

        ITrtHolderBase<T>::Object_->destroy();
    }
};
