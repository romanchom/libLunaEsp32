#pragma once

#include "Strand.hpp"

#include <luna/proto/SetColor.hpp>
#include <luna/proto/Scalar.hpp>

#include <esp_log.h>
#include <limits>
#include <utility>

namespace luna::esp32
{
    struct ElectricalLoad;

    struct PowerObserver
    {
        virtual ~PowerObserver() = default;
        virtual void currentDrawChanged(ElectricalLoad * emitter, float current) = 0;
    };

    struct ElectricalLoad
    {
        explicit ElectricalLoad(float currentPerPixel) :
            mPowerObserver(nullptr),
            mCurrentPerPixel(currentPerPixel),
            mTotalCurrent(0)
        {}

        void powerObserver(PowerObserver * value)
        {
            mPowerObserver = value;
        }
    protected:
        void notify()
        {
            if (mPowerObserver) {
                mPowerObserver->currentDrawChanged(this, mTotalCurrent);
            }
        }

        PowerObserver * mPowerObserver;
        float mCurrentPerPixel;
        float mTotalCurrent;
    };

    namespace detail
    {
        template<typename>
        struct SetLightParameterTrait;

        template<typename T, typename B>
        struct SetLightParameterTrait<void (B::*)(T const *, size_t, size_t)>
        {
            using Type = T;
        };

        template<typename T>
        struct StrandDataTypeTrait
        {
            using Type = typename SetLightParameterTrait<decltype(&T::setLight)>::Type;
        };

        template<typename T>
        using StrandDataType = typename StrandDataTypeTrait<T>::Type;


        template<typename T>
        struct PowerReporter : T, ElectricalLoad
        {
            template<typename... U>
            explicit PowerReporter(float currentPerPixel, U... rest) :
                T(std::forward<U>(rest)...),
                ElectricalLoad(currentPerPixel)
            {}
            
            void render() override
            {
                notify();
                mTotalCurrent = 0;
                T::render();
            }
        };
        
        template<typename T, typename P>
        struct PowerMeterWrapper;
        
        template<typename T>
        struct PowerMeterWrapper<T, proto::RGB> : PowerReporter<T>
        {   
            using PowerReporter<T>::PowerReporter;

            void setLight(proto::RGB const * data, size_t size, size_t offset) override
            {
                int sum = 0;
                for (size_t i = offset; i < size + offset; ++i) {
                    sum += data[i].r;
                    sum += data[i].g;
                    sum += data[i].b;
                }
                this->mTotalCurrent += float(sum) * this->mCurrentPerPixel / 255.0f;
                PowerReporter<T>::setLight(data, size, offset);
            }
        };
        
        template<typename T, typename U>
        struct PowerMeterWrapper<T, proto::Scalar<U>> : PowerReporter<T>
        {   
            using PowerReporter<T>::PowerReporter;

            void setLight(proto::Scalar<U> const * data, size_t size, size_t offset) override
            {
                int sum = 0;
                for (size_t i = offset; i < size + offset; ++i) {
                    sum += data[i];
                }
                this->mTotalCurrent += float(sum) * this->mCurrentPerPixel / std::numeric_limits<U>::max();
                PowerReporter<T>::setLight(data, size, offset);
            }
        };
    }

    template<typename T>
    using Metered = detail::PowerMeterWrapper<T, detail::StrandDataType<T>>;

}