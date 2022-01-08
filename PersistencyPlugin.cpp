#include "PersistencyPlugin.hpp"

#include <Nvs.hpp>
#include <luna/Property.hpp>

#include <nvs_flash.h>
#include <esp_log.h>

#include <charconv>
#include <memory>
#include <vector>

namespace luna
{
    namespace
    {
        auto const TAG = "Pers";

        template<typename T>
        struct NvsProperty : Property<T>
        {
            explicit NvsProperty(nvs_handle_t handle, std::string && name, bool & loaded) :
                mHandle(handle),
                mName(std::move(name))
            {
                loaded = (ESP_OK == load());
            }

            T getValue() const override
            {
                return mValue;
            }

            void set(T const & value) override
            {
                if (mValue == value) { return; }
                mValue = value;
                save();
            }
        private:
            esp_err_t load();
            esp_err_t save();

            T mValue;
            nvs_handle_t const mHandle;
            std::string mName;
        };

        template<>
        esp_err_t NvsProperty<bool>::load()
        {
            return nvs_get_u8(mHandle, mName.c_str(), (uint8_t *)&mValue);
        }

        template<>
        esp_err_t NvsProperty<int>::load()
        {
            return nvs_get_i32(mHandle, mName.c_str(), &mValue);
        }

        template<>
        esp_err_t NvsProperty<float>::load()
        {
            return nvs_get_u32(mHandle, mName.c_str(), (uint32_t *) &mValue);
        }

        template<>
        esp_err_t NvsProperty<prism::RGB>::load()
        {
            size_t size = sizeof(prism::RGB);
            return nvs_get_blob(mHandle, mName.c_str(), &mValue, &size);
        }

        template<>
        esp_err_t NvsProperty<prism::RGBW>::load()
        {
            size_t size = sizeof(prism::RGBW);
            return nvs_get_blob(mHandle, mName.c_str(), &mValue, &size);
        }

        template<>
        esp_err_t NvsProperty<prism::CieXY>::load()
        {
            size_t size = sizeof(prism::CieXY);
            return nvs_get_blob(mHandle, mName.c_str(), &mValue, &size);
        }

        template<>
        esp_err_t NvsProperty<std::string>::load()
        {
            size_t size = 0;
            auto error = nvs_get_str(mHandle, mName.c_str(), nullptr, &size);
            if (error == ESP_OK) {
                mValue.resize(size - 1);
                error = nvs_get_str(mHandle, mName.c_str(), mValue.data(), &size);
            }
            return error;
        }


        template<>
        esp_err_t NvsProperty<bool>::save()
        {
            return nvs_set_u8(mHandle, mName.c_str(), uint8_t(mValue));
        }

        template<>
        esp_err_t NvsProperty<int>::save()
        {
            return nvs_set_i32(mHandle, mName.c_str(), mValue);
        }

        template<>
        esp_err_t NvsProperty<float>::save()
        {
            return nvs_set_u32(mHandle, mName.c_str(), (uint32_t &) mValue);
        }

        template<>
        esp_err_t NvsProperty<prism::RGB>::save()
        {
            return nvs_set_blob(mHandle, mName.c_str(), &mValue, sizeof(prism::RGB));
        }

        template<>
        esp_err_t NvsProperty<prism::RGBW>::save()
        {
            return nvs_set_blob(mHandle, mName.c_str(), &mValue, sizeof(prism::RGBW));
        }

        template<>
        esp_err_t NvsProperty<prism::CieXY>::save()
        {
            return nvs_set_blob(mHandle, mName.c_str(), &mValue, sizeof(prism::CieXY));
        }

        template<>
        esp_err_t NvsProperty<std::string>::save()
        {
            return nvs_set_str(mHandle, mName.c_str(), mValue.c_str());
        }

        struct NvsNamespace
        {
            explicit NvsNamespace(std::string const & name)
            {
                auto hash = std::hash<std::string>()(name);
                char key[2 * sizeof(hash) + 1] = {};
                std::to_chars(key, key + sizeof(key), hash, 16);
                auto error = nvs_open(key, NVS_READWRITE, &mHandle);
                if (error != ESP_OK) {
                    ESP_LOGE(TAG, "Failed to open NVS. %x", error);
                }
            }

            explicit NvsNamespace(NvsNamespace && other) :
                mHandle(other.mHandle)
            {
                other.mHandle = 0;
            }

            ~NvsNamespace()
            {
                if (mHandle) {
                    nvs_close(mHandle);
                }
            }
            nvs_handle_t handle() const { return mHandle; }
        private:
            nvs_handle_t mHandle;
        };

        struct PersistencyPluginInstance : PluginInstance
        {
            explicit PersistencyPluginInstance(Configurable * configurable);
            void recurse(Configurable * configurable, std::string const & name);
            void onNetworkAvaliable(LunaNetworkInterface * luna) final {}

        private:
            friend struct PropertyFactory;
            std::vector<NvsNamespace> mNamespaces;
            std::vector<std::unique_ptr<AbstractProperty>> mProperties;
        };

        struct PropertyFactory
        {
            explicit PropertyFactory(PersistencyPluginInstance * plugin, nvs_handle_t handle, std::string const & name) :
                mPlugin(plugin),
                mHandle(handle),
                mName(std::move(name))
            {}

            template<typename T>
            void operator()(Property<T> * property)
            {
                bool loaded;
                auto nvsProperty = std::make_unique<NvsProperty<T>>(mHandle, std::move(mName), loaded);
                if (loaded) {
                    property->set(nvsProperty->get());
                }
                nvsProperty->bindTo(property);
                mPlugin->mProperties.emplace_back(std::move(nvsProperty));
            }

        private:
            PersistencyPluginInstance * const mPlugin;
            nvs_handle_t const mHandle;
            std::string mName;
        };

        PersistencyPluginInstance::PersistencyPluginInstance(Configurable * configurable)
        {
            recurse(configurable, "");
        }

        void PersistencyPluginInstance::recurse(Configurable * configurable, std::string const & name)
        {
            mNamespaces.emplace_back(name);

            auto & ns = mNamespaces.back();

            for (auto & [propertyName, property] : configurable->properties()) {
                PropertyFactory factory(this, ns.handle(), propertyName);
                TemplatedVisitor visitor(factory);
                property->acceptVisitor(&visitor);
            }

            for (auto [childName, child] : configurable->children()) {
                recurse(child, childName);
            }
        }
    }

    PersistencyPlugin::PersistencyPlugin(Configurable * configurable) :
        mConfigurable(configurable)
    {
        Nvs::init();
    }

    PersistencyPlugin::~PersistencyPlugin() = default;

    std::unique_ptr<PluginInstance> PersistencyPlugin::instantiate(LunaInterface * luna)
    {
        return std::make_unique<PersistencyPluginInstance>(mConfigurable);
    }
}
