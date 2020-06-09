#include "Luna.hpp"
#include "OnlineContext.hpp"

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
            mWiFi = std::make_unique<WiFi>(mConfig->wifiCredentials.ssid, mConfig->wifiCredentials.password);
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
            mOnlineContext = std::make_unique<OnlineContext>();
            if (!mTlsConfiguration) {
                mTlsConfiguration = std::make_unique<TlsConfiguration>(mConfig->tlsCredentials);
            }
            for (auto & plugin : mPluginInstances) {
                plugin->onNetworkAvaliable(this);
            }
        });
    }

    void Luna::disconnected()
    {
        mMainLoop.post([this]{
            mNetworkServices.clear();
            mOnlineContext.reset();
        });
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
        return mTlsConfiguration.get();
    }

    asio::io_context * Luna::ioContext()
    {
        return mOnlineContext->ioContext();
    }
}
