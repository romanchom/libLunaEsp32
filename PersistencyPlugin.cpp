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
            explicit NvsProperty(nvs_handle_t handle, std::string && name) :
                Property<T>(std::move(name)),
                mHandle(handle)
            {
                load();
            }

            T getValue() const override
            {
                return mValue;
            }

            void setValue(T const & value) override
            {
                if (mValue == value) { return; }
                mValue = value;
                save();
            }
        private:
            void load();
            void save();

            T mValue;
            nvs_handle_t const mHandle;
        };

        template<>
        void NvsProperty<bool>::load()
        {
            auto error = nvs_get_u8(mHandle, name().c_str(), reinterpret_cast<uint8_t *>(&mValue));
        }

        template<>
        void NvsProperty<int>::load()
        {
            auto error = nvs_get_i32(mHandle, name().c_str(), &mValue);
        }

        template<>
        void NvsProperty<float>::load()
        {
            auto error = nvs_get_u32(mHandle, name().c_str(), reinterpret_cast<uint32_t *>(&mValue));
        }

        template<>
        void NvsProperty<prism::CieXYZ>::load()
        {
            size_t size = sizeof(prism::CieXYZ);
            auto error = nvs_get_blob(mHandle, name().c_str(), reinterpret_cast<void *>(&mValue), &size);
        }

        template<>
        void NvsProperty<std::string>::load()
        {
            size_t size = 0;
            auto error = nvs_get_str(mHandle, name().c_str(), nullptr, &size);
            if (error == ESP_OK) {
                mValue.resize(size - 1);
                error = nvs_get_str(mHandle, name().c_str(), mValue.data(), &size);
            }
        }


        template<>
        void NvsProperty<bool>::save()
        {
            auto error = nvs_set_u8(mHandle, name().c_str(), static_cast<uint8_t>(mValue));
        }

        template<>
        void NvsProperty<int>::save()
        {
            auto error = nvs_set_i32(mHandle, name().c_str(), mValue);
        }

        template<>
        void NvsProperty<float>::save()
        {
            auto error = nvs_set_u32(mHandle, name().c_str(), reinterpret_cast<uint32_t &>(mValue));
        }

        template<>
        void NvsProperty<prism::CieXYZ>::save()
        {
            auto error = nvs_set_blob(mHandle, name().c_str(), reinterpret_cast<void *>(&mValue), sizeof(prism::CieXYZ));
        }

        template<>
        void NvsProperty<std::string>::save()
        {
            auto error = nvs_set_str(mHandle, name().c_str(), mValue.c_str());
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
            auto nvsProperty = std::make_unique<NvsProperty<T>>(mHandle, std::move(mName));
            property->bindTo(nvsProperty.get());
            nvsProperty->bindTo(property);
            mPlugin->mProperties.emplace_back(std::move(nvsProperty));
        }

        void visit(Property<bool> * property) final { makeProperty<bool>(property); }
        void visit(Property<int> * property) final { makeProperty<int>(property); }
        void visit(Property<float> * property) final { makeProperty<float>(property); }
        void visit(Property<prism::CieXYZ> * property) final { makeProperty<prism::CieXYZ>(property); }
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
        if (mHandle) { nvs_close(mHandle); }
    }

    PersistencyPlugin::PersistencyPlugin(Configurable * effectEngine)
    {
        Nvs::init();
        recurse(effectEngine, "");
    }

    PersistencyPlugin::~PersistencyPlugin() = default;

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
