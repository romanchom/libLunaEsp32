#pragma once

#include <prism/Prism.hpp>

#include <string>
#include <vector>
#include <algorithm>

namespace luna
{
    template<typename T>
    struct Property;

    struct Visitor {
        virtual void visit(Property<bool> * property) = 0;
        virtual void visit(Property<int> * property) = 0;
        virtual void visit(Property<float> * property) = 0;
        virtual void visit(Property<prism::RGB> * property) = 0;
        virtual void visit(Property<prism::RGBW> * property) = 0;
        virtual void visit(Property<prism::CieXY> * property) = 0;
        virtual void visit(Property<std::string> * property) = 0;
    protected:
        ~Visitor() = default;
    };

    template<typename Functor>
    struct TemplatedVisitor : Visitor
    {
        explicit TemplatedVisitor(Functor & functor) :
            mFunctor(functor)
        {}

        void visit(Property<bool> * property) override { mFunctor(property); }
        void visit(Property<int> * property) override { mFunctor(property); }
        void visit(Property<float> * property) override { mFunctor(property); }
        void visit(Property<prism::RGB> * property) override { mFunctor(property); }
        void visit(Property<prism::RGBW> * property) override { mFunctor(property); }
        void visit(Property<prism::CieXY> * property) override { mFunctor(property); }
        void visit(Property<std::string> * property) override { mFunctor(property); }

    private:
        Functor & mFunctor;
    };

    struct AbstractProperty
    {
        virtual ~AbstractProperty() = default;

        void acceptVisitor(Visitor * visitor)
        {
            accept(visitor);
        }
    private:
        virtual void accept(Visitor * visitor) = 0;
    };

    template<typename T>
    struct Property : AbstractProperty
    {
        using AbstractProperty::AbstractProperty;

        ~Property() override
        {
            for (auto publisher : mPublishers)
            {
                auto thisIt = std::find(publisher->mSubscribers.begin(), publisher->mSubscribers.end(), this);
                publisher->mSubscribers.erase(thisIt);
            }

            for (auto subscriber : mSubscribers)
            {
                auto it = std::find(subscriber->mPublishers.begin(), subscriber->mPublishers.end(), this);
                subscriber->mPublishers.erase(it);
            }
        }

        void bindTo(Property * other)
        {
            if (other) {
                mPublishers.emplace_back(other);
                other->mSubscribers.emplace_back(this);
                set(other->get());
            }
        }

        void unbindFrom(Property * other)
        {
            auto it = std::find(mPublishers.begin(), mPublishers.end(), other);

            if (it != mPublishers.end()) {
                auto thisIt = std::find(other->mSubscribers.begin(), other->mSubscribers.end(), this);
                other->mSubscribers.erase(thisIt);
                mPublishers.erase(it);
            }
        }

        T get() const
        {
            return getValue();
        }

        operator T() const
        {
            return get();
        }

        virtual void set(T const & value) = 0;

        void operator=(T const & value)
        {
            set(value);
        }

    protected:
        void notify(T const & value)
        {
            for (auto subscriber : mSubscribers) {
                subscriber->set(value);
            }
        }

    private:
        void accept(Visitor * visitor) override
        {
            visitor->visit(this);
        }

        virtual T getValue() const = 0;

        std::vector<Property *> mPublishers;
        std::vector<Property *> mSubscribers;
    };
}
