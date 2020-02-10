#include <napi.h>
namespace recordfunc {
  int startRecording(std::string fullPathName, int inputDeviceIndex, int mode, std::string ffmpegPath, std::string codec);
  Napi::Number startRecordingWrapped(const Napi::CallbackInfo& info);


  int stopRecording();
  Napi::Number stopRecordingWrapped(const Napi::CallbackInfo& info);

  std::vector<std::string> listRecordingDevices();
  Napi::Value listRecordingDevicesWrapped(const Napi::CallbackInfo& info);

  Napi::Object Init(Napi::Env env, Napi::Object exports);
}