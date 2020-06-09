#include "PersistencyPlugin.hpp"

#include <Nvs.hpp>

#include <esp_log.h>

#include <charconv>

namespace luna
{
    namespace
    {
        auto const TAG = "Pers";

        template<typename T>
        struct NvsProperty : Property<T>
        {
            explicit NvsProperty(nvs_handle_t handle, std::string && name, bool & loaded) :
                Property<T>(std::move(name)),
                mHandle(handle)
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
        };

        template<>
        esp_err_t NvsProperty<bool>::load()
        {
            return nvs_get_u8(mHandle, name().c_str(), reinterpret_cast<uint8_t *>(&mValue));
        }

        template<>
        esp_err_t NvsProperty<int>::load()
        {
            return nvs_get_i32(mHandle, name().c_str(), &mValue);
        }

        template<>
        esp_err_t NvsProperty<float>::load()
        {
            return nvs_get_u32(mHandle, name().c_str(), reinterpret_cast<uint32_t *>(&mValue));
        }

        template<>
        esp_err_t NvsProperty<prism::RGB>::load()
        {
            size_t size = sizeof(prism::RGB);
            return nvs_get_blob(mHandle, name().c_str(), reinterpret_cast<void *>(&mValue), &size);
        }

        template<>
        esp_err_t NvsProperty<prism::CieXY>::load()
        {
            size_t size = sizeof(prism::CieXY);
            return nvs_get_blob(mHandle, name().c_str(), reinterpret_cast<void *>(&mValue), &size);
        }

        template<>
        esp_err_t NvsProperty<std::string>::load()
        {
            size_t size = 0;
            auto error = nvs_get_str(mHandle, name().c_str(), nullptr, &size);
            if (error == ESP_OK) {
                mValue.resize(size - 1);
                error = nvs_get_str(mHandle, name().c_str(), mValue.data(), &size);
            }
            return error;
        }


        template<>
        esp_err_t NvsProperty<bool>::save()
        {
            return nvs_set_u8(mHandle, name().c_str(), static_cast<uint8_t>(mValue));
        }

        template<>
        esp_err_t NvsProperty<int>::save()
        {
            return nvs_set_i32(mHandle, name().c_str(), mValue);
        }

        template<>
        esp_err_t NvsProperty<float>::save()
        {
            return nvs_set_u32(mHandle, name().c_str(), reinterpret_cast<uint32_t &>(mValue));
        }

        template<>
        esp_err_t NvsProperty<prism::RGB>::save()
        {
            return nvs_set_blob(mHandle, name().c_str(), reinterpret_cast<void *>(&mValue), sizeof(prism::RGB));
        }

        template<>
        esp_err_t NvsProperty<prism::CieXY>::save()
        {
            return nvs_set_blob(mHandle, name().c_str(), reinterpret_cast<void *>(&mValue), sizeof(prism::CieXY));
        }

        template<>
        esp_err_t NvsProperty<std::string>::save()
        {
            return nvs_set_str(mHandle, name().c_str(), mValue.c_str());
        }

    }

    struct NvsVisitor : Visitor
    {
        explicit NvsVisitor(PersistencyPlugin * plugin, nvs_handle_t handle, std::string && name) :
            mPlugin(plugin),
            mHandle(handle),
            mName(std::move(name))
        {}

        template<typename T>
        void makeProperty(Property<T> * property)
        {
            bool loaded;
            auto nvsProperty = std::make_unique<NvsProperty<T>>(mHandle, std::move(mName), loaded);
            if (loaded) {
                // if loaded bind the other first, so that the stored value is propagated
                property->bindTo(nvsProperty.get());
                nvsProperty->bindTo(property);
            } else {
                // if not loaded bind this first and accept whatever value the other has
                nvsProperty->bindTo(property);
                property->bindTo(nvsProperty.get());
            }
            mPlugin->mProperties.emplace_back(std::move(nvsProperty));
        }

        void visit(Property<bool> * property) final { makeProperty<bool>(property); }
        void visit(Property<int> * property) final { makeProperty<int>(property); }
        void visit(Property<float> * property) final { makeProperty<float>(property); }
        void visit(Property<prism::RGB> * property) final { makeProperty<prism::RGB>(property); }
        void visit(Property<prism::CieXY> * property) final { makeProperty<prism::CieXY>(property); }
        void visit(Property<std::string> * property) final { makeProperty<std::string>(property); }

    private:
        PersistencyPlugin * const mPlugin;
        nvs_handle_t const mHandle;
        std::string mName;
    };

    NvsNamespace::NvsNamespace(std::string const & name)
    {
        auto hash = std::hash<std::string>()(name);
        char key[2 * sizeof(hash) + 1] = {};
        std::to_chars(key, key + sizeof(key), hash, 16);
        auto error = nvs_open(key, NVS_READWRITE, &mHandle);
        if (error != ESP_OK) {
            ESP_LOGE(TAG, "Failed to open NVS. %x", error);
        }
    }

    NvsNamespace::NvsNamespace(NvsNamespace && other) :
        mHandle(other.mHandle)
    {
        other.mHandle = 0;
    }

    NvsNamespace::~NvsNamespace()
    {
        if (mHandle) {
            nvs_close(mHandle);
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
        recurse(mConfigurable, "");

        struct Instance : PluginInstance
        {
            void onNetworkAvaliable(LunaNetworkInterface * luna) final {}
        };
        return std::make_unique<Instance>();
    }

    void PersistencyPlugin::recurse(Configurable * configurable, std::string const & name)
    {
        std::string nsName = name + configurable->name();

        mNamespaces.emplace_back(nsName);

        auto & ns = mNamespaces.back();

        for (auto property : configurable->properties()) {
            NvsVisitor visitor(this, ns.handle(), std::string(property->name()));
            property->acceptVisitor(&visitor);
        }

        for (auto child : configurable->children()) {
            recurse(child, nsName);
        }
    }

}
