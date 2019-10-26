#pragma once

#include "ConstantEffect.hpp"
#include "Effect.hpp"
#include "EffectMixer.hpp"

#include <luna/Service.hpp>

#include <asio/io_context.hpp>
#include <asio/steady_timer.hpp>

#include <set>

namespace luna
{
    struct HardwareController;

    struct EffectPointerNameCompare
    {
        using is_transparent = void;

        bool operator()(Effect * left, Effect * right) const noexcept
        {
            return left->name() < right->name();
        }

        template<typename T>
        bool operator()(T const & left, Effect * right) const noexcept
        {
            return left < right->name();
        }

        template<typename T>
        bool operator()(Effect * left, T const & right) const noexcept
        {
            return left->name() < right;
        }
    };

    struct EffectEngine : Service, Configurable, private EffectMixer::Observer
    {
        explicit EffectEngine(asio::io_context * ioContext, std::initializer_list<Effect *> effects);

        std::set<Effect *, EffectPointerNameCompare> const & effects() const noexcept { return mEffects; }
    private:
        void switchTo(std::string_view effectName);

        void takeOwnership(HardwareController * controller) final;
        void releaseOwnership() final;

        void startTick();
        void update();

        void enabledChanged(bool enabled) override;

        asio::steady_timer mTick;
        HardwareController * mController;

        std::set<Effect *, EffectPointerNameCompare> mEffects;
        EffectMixer mEffectMixer;
    };
}
