#include "TlsConfiguration.hpp"

namespace luna
{
    TlsConfiguration::TlsConfiguration(TlsCredentials const & credentials) :
        mOwnKey(credentials.ownKey),
        mOwnCertificate(credentials.ownCertificate),
        mCaCertificate(credentials.caCertificate)
    {
        mRandom.seed(&mEntropy, "", 0);

        mCookie.setup(&mRandom);
    }

    std::unique_ptr<tls::Configuration> TlsConfiguration::makeTlsConfiguration()
    {
        auto config = makeDefault();
        config->setDefaults(tls::Endpoint::server, tls::Transport::stream, tls::Preset::default_);
        return std::move(config);
    }

    std::unique_ptr<tls::Configuration> TlsConfiguration::makeDtlsConfiguration()
    {
        auto config = makeDefault();
        config->setDtlsCookies(&mCookie);
        config->setDefaults(tls::Endpoint::server, tls::Transport::datagram, tls::Preset::default_);
        return std::move(config);
    }
    
    std::unique_ptr<tls::Configuration> TlsConfiguration::makeDefault()
    {
        auto config = std::make_unique<tls::Configuration>();
        config->setRandomGenerator(&mRandom);
        config->setOwnCertificate(&mOwnCertificate, &mOwnKey);
        config->setCertifiateAuthorityChain(&mCaCertificate);
        config->setAuthenticationMode(tls::AuthenticationMode::required);
        return std::move(config);
    }
}
