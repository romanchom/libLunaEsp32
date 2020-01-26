#pragma once

#include <prism/Prism.hpp>

#include <nvs.h>

#include <functional>
#include <string>
#include <vector>

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
            mPublishers.emplace_back(other);

            if (other) {
                other->mSubscribers.emplace_back(this);
                set(other->get());
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

        operator T() const
        {
            return get();
        }
    protected:
        void notify(T const & value)
        {
            for (auto subscriber : mSubscribers) {
                subscriber->set(value);
            }
        }
        
        void operator =(T const & value)
        {
            set(value);
        }

    private:
        void accept(Visitor * visitor) override
        {
            visitor->visit(this);
        }

        virtual T getValue() const = 0;
        virtual void set(T const & value) = 0;

        std::vector<Property *> mPublishers;
        std::vector<Property *> mSubscribers;
    };
}
