#pragma once

#include "EffectMixer.hpp"
#include "EffectSet.hpp"

#include <luna/Plugin.hpp>
#include <luna/Configurable.hpp>
#include <luna/MemberProperty.hpp>

#include <asio/io_context.hpp>
#include <asio/steady_timer.hpp>

#include <memory>
#include <string>

namespace luna
{
    struct Effect;

    struct EffectPlugin : Plugin, Configurable
    {
        explicit EffectPlugin(std::vector<Effect *> && effects);

        Property<std::string> & activeEffect() { return mActiveEffect; }
        Property<bool> & enabled() { return mEnabled; }

        std::unique_ptr<PluginInstance> instantiate(LunaInterface * luna) override;

        std::vector<AbstractProperty *> properties() override;
        std::vector<Configurable *> children() override;

    private:
        struct Instance;

        std::string getActiveEffect() const;
        void setActiveEffect(std::string const & value);

        bool getEnabled() const;
        void setEnabled(bool const & value);

        EffectSet mEffects;

        MemberProperty<EffectPlugin, std::string> mActiveEffect;
        MemberProperty<EffectPlugin, bool> mEnabled;

        std::string mLastEffect;
        EffectMixer mEffectMixer;
    };
}
