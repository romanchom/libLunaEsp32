#include "Luna.hpp"

#include <luna/Plugin.hpp>

#include <esp_log.h>

namespace luna
{
    Luna::Luna(LunaConfiguration const * config) :
        mConfig(config),
        mControllerMux(mConfig->device)
    {
        for (auto plugin : config->plugins) {
            mPluginInstances.emplace_back(std::move(plugin->instantiate(this)));
        }

        mMainLoop.post([this]{
            mWiFi.emplace(mConfig->name, mConfig->wifiCredentials.ssid, mConfig->wifiCredentials.password);
            mWiFi->observer(this);
            mWiFi->enabled(true);
        });
    }

    Luna::~Luna() = default;

    void Luna::run()
    {
        ESP_LOGW("Luna", "Up and running.");
        mMainLoop.execute();
    }

    void Luna::connected()
    {
        mMainLoop.post([this]{
#if ESP32
            mOnlineContext.emplace();
#endif
            if (!mTlsConfiguration &&
                !mConfig->tlsCredentials.caCertificate.empty() &&
                !mConfig->tlsCredentials.ownCertificate.empty() &&
                !mConfig->tlsCredentials.ownKey.empty()) {
                mTlsConfiguration.emplace(mConfig->tlsCredentials);
            }
            for (auto & plugin : mPluginInstances) {
                plugin->onNetworkAvaliable(this);
            }
#if ESP32
            mOnlineContext->start();
#endif
        });
    }

    void Luna::disconnected()
    {
        mMainLoop.post([this]{
            mNetworkServices.clear();
#if ESP32
            mOnlineContext.reset();
#endif
        });
    }

    std::string const & Luna::name()
    {
        return mConfig->name;
    }

    void Luna::post(std::function<void()> && task)
    {
        mMainLoop.post(std::move(task));
    }

    std::unique_ptr<ControllerHandle> Luna::addController(Controller * controller)
    {
        struct Handle : ControllerHandle
        {
            Handle(ControllerMux * mux, Controller * controller) :
                mMux(mux),
                mController(controller)
            {
                mMux->add(mController);
            }

            ~Handle() final
            {
                mMux->remove(mController);
            }

            void enabled(bool value) final
            {
                mMux->setEnabled(mController, value);
            }

        private:
            ControllerMux * mMux;
            Controller * mController;
        };

        return std::make_unique<Handle>(&mControllerMux, controller);
    }

    Device * Luna::device()
    {
        return mConfig->device;
    }

    void Luna::addNetworkService(std::unique_ptr<NetworkService> service)
    {
        mNetworkServices.emplace_back(std::move(service));
    }

    TlsCredentials const & Luna::tlsCredentials()
    {
        return mConfig->tlsCredentials;
    }

    TlsConfiguration * Luna::tlsConfiguration()
    {
        if (mTlsConfiguration) {
            return &mTlsConfiguration.value();
        } else {
            return nullptr;
        }
    }

    asio::io_context * Luna::ioContext()
    {
#if ESP32
        return mOnlineContext->ioContext();
#else
        return nullptr;
#endif
    }
}
