#pragma once


#include "Property.hpp"

namespace luna
{
    template<typename Owner, typename T>
    struct MemberProperty : Property<T>
    {
        using Setter = void (Owner::*)(T const &);
        using Getter = T (Owner::*)() const;
        explicit MemberProperty(Owner * owner, Getter getter, Setter setter) :
            mOwner(owner),
            mGetter(getter),
            mSetter(setter)
        {}

        using Property<T>::notify;
        using Property<T>::operator=;
        using Property<T>::operator T;

        void set(T const & value) override
        {
            (mOwner->*mSetter)(value);
        }

    private:
        T getValue() const override
        {
            return (mOwner->*mGetter)();
        }

        Owner * mOwner;
        Getter mGetter;
        Setter mSetter;
    };
}
