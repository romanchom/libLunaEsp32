// #include "Controller.hpp"



// #include <luna/Packets.hpp>
// #include <luna/server/Encoder.hpp>

// #include <esp_log.h>

// using namespace tls;
// using namespace lwip_async;
// using namespace luna;

// static char const TAG[] = "ControlChannel";

// namespace luna {
// namespace esp32 {

// Controller::Controller(ControlChannelConfig const & config, luna::server::StrandDecoder * strandDecoder) :
//     mStrandDecoder(strandDecoder),
//     mControlChannel(config, static_cast<luna::server::Listener *>(this))
// {
// }

// Controller::~Controller() = default;

// void Controller::strandConfigurationRequested()
// {
//     ESP_LOGI(TAG, "Config request");
//     mControlChannel.commandEncoder().sendStrandConfiguration(mStrandDecoder->getConfiguration());
// }

// void Controller::dataChannelRequested()
// {
//     ESP_LOGI(TAG, "Data channel request");

//     mDtls.reset(new lwip_async::DtlsInputOutput(mStrandDecoder));

//     luna::DataChannelConfiguration configuration;

//     configuration.port = mDtls->port();
//     configuration.sharedKey = mDtls->sharedKey();

//     mStrandDecoder->enabled(true);

//     mControlChannel.commandEncoder().sendDataChannelOpened(configuration);
// }

// void Controller::dataChannelClosed()
// {
//     ESP_LOGI(TAG, "Data channel closed");

//     reset();
// }

// void Controller::disconnected()
// {
//     reset();
// }

// void Controller::reset()
// {
//     mDtls.reset();
//     mStrandDecoder->enabled(false);
// }

// }}
