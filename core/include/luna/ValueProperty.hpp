#pragma once

#include "Property.hpp"

#include <cmath>

namespace luna
{
    template<typename T, typename Validator>
    struct ValueProperty : Property<T>
    {
        explicit ValueProperty(T defaultValue = T{}) :
            mValue(defaultValue)
        {}

        T getValue() const final
        {
            return mValue;
        }

        void set(T const & value) final
        {
            if (mValue != value && Validator()(value)) {
                mValue = value;
                this->notify(value);
            }
        }

        using Property<T>::operator=;
    private:
        T mValue;
    };

    struct ValidPositive
    {
        bool operator()(float value) const
        {
            return value >= 0.0f && !std::isinf(value);
        }
    };

    struct ValidZeroOne
    {
        bool operator()(float value) const
        {
            return value >= 0.0f && value <= 1.0f;
        }
    };
}