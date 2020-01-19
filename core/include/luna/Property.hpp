#pragma once

#include <prism/Prism.hpp>

#include <nvs.h>

#include <functional>
#include <string>
#include <set>

namespace luna
{
    template<typename T>
    struct Property;

    struct Visitor {
        virtual void visit(Property<bool> * property) = 0;
        virtual void visit(Property<int> * property) = 0;
        virtual void visit(Property<float> * property) = 0;
        virtual void visit(Property<prism::CieXYZ> * property) = 0;
        virtual void visit(Property<std::string> * property) = 0;
    protected:
        ~Visitor() = default;
    };

    struct AbstractProperty
    {
        explicit AbstractProperty(std::string const & name) :
            mName(name)
        {}

        virtual ~AbstractProperty() = default;

        std::string const & name() const
        {
            return mName;
        }

        void acceptVisitor(Visitor * visitor)
        {
            accept(visitor);
        }
    private:
        virtual void accept(Visitor * visitor) = 0;

        std::string const mName;
    };

    template<typename T>
    struct Property : AbstractProperty
    {
        using AbstractProperty::AbstractProperty;

        void bindTo(Property * other)
        {
            mPublishers.emplace(other);

            if (other) {
                other->mSubscribers.emplace(this);
                setValue(other->get());
            }
        }

        void twoWayBindTo(Property * other)
        {
            bindTo(other);
            other->bindTo(this);
        }

        T get() const
        {
            return getValue();
        }

    protected:
        void notify(T const & value)
        {
            for (auto subscriber : mSubscribers) {
                subscriber->setValue(value);
            }
        }
    private:
        void accept(Visitor * visitor) override
        {
            visitor->visit(this);
        }

        virtual T getValue() const = 0;
        virtual void setValue(T const & value) = 0;

        std::set<Property *> mPublishers;
        std::set<Property *> mSubscribers;
    };

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

        void setValue(T const & value) override
        {
            (mOwner->*mSetter)(value);
        }

        Owner * mOwner;
        Getter mGetter;
        Setter mSetter;
    };
}
