#include "TlsConfiguration.hpp"

namespace luna
{
    TlsConfiguration::TlsConfiguration(std::string_view ownKey, std::string_view ownCertificate, std::string_view caCertificate) :
        mOwnKey(ownKey),
        mOwnCertificate(ownCertificate),
        mCaCertificate(caCertificate)
    {
        mRandom.seed(&mEntropy, "", 0);

        mCookie.setup(&mRandom);

        mUpdaterConfiguration.setRandomGenerator(&mRandom);
        mUpdaterConfiguration.setDefaults(tls::Endpoint::server, tls::Transport::stream, tls::Preset::default_);
        mUpdaterConfiguration.setOwnCertificate(&mOwnCertificate, &mOwnKey);
        mUpdaterConfiguration.setCertifiateAuthorityChain(&mCaCertificate);
        mUpdaterConfiguration.setAuthenticationMode(tls::AuthenticationMode::required);

        mRealtimeConfiguration.setRandomGenerator(&mRandom);
        mRealtimeConfiguration.setDefaults(tls::Endpoint::server, tls::Transport::datagram, tls::Preset::default_);
        mRealtimeConfiguration.setOwnCertificate(&mOwnCertificate, &mOwnKey);
        mRealtimeConfiguration.setCertifiateAuthorityChain(&mCaCertificate);
        mRealtimeConfiguration.setAuthenticationMode(tls::AuthenticationMode::required);
        mRealtimeConfiguration.setDtlsCookies(&mCookie);
    }
}
