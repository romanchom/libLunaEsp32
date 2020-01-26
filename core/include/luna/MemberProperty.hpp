#pragma once


#include "Property.hpp"

namespace luna
{
    template<typename Owner, typename T>
    struct MemberProperty : Property<T>
    {
        using Setter = void (Owner::*)(T const &);
        using Getter = T (Owner::*)() const;
        explicit MemberProperty(std::string name, Owner * owner, Getter getter, Setter setter) :
            Property<T>(name),
            mOwner(owner),
            mGetter(getter),
            mSetter(setter)
        {}

        using Property<T>::notify;
    private:
        T getValue() const override
        {
            return (mOwner->*mGetter)();
        }

        void set(T const & value) override
        {
            (mOwner->*mSetter)(value);
        }

        Owner * mOwner;
        Getter mGetter;
        Setter mSetter;
    };
}
