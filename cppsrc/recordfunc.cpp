#include "recordfunc.h"
#include "audiorecorder.h"
#include <iostream>
#include <windows.h>
#include <stdio.h>
#include <thread> 
#include <fstream> 
#include <tlhelp32.h>
#include <DSound.h>
#include "audiocapturedevices.h"
#include "strconv.h"

#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "DSound.lib")
//#pragma comment(lib, "portaudio_x64.lib")



bool bquit;
std::string fpath;
std::ofstream ofile;

void callback(WAVEHDR data)
{
  LPSTR audioData = data.lpData;
  
  ofile.write (audioData, data.dwBytesRecorded);
}



void recordthreadfunc() 
{
  AudioRecorder recorder;

  // Inits everything
  recorder.Init(callback);

  // Start the recording process
  recorder.StartRecording();

  while (!bquit)
  {
    recorder.Update();
  }

  // Stop recording from the microphone
  recorder.StopRecording(); 
  ofile.close();

}

PROCESS_INFORMATION processInfo;
int wmode=2;

int recordfunc::startRecording(std::string fullPathName, int inputDeviceIndex, int mode, std::string ffmpegPath, std::string codec){
  wmode = mode;
  std::vector<std::string> v = recordfunc::listRecordingDevices();

  if (mode == 1) {

    std::string cmdline =  "ffmpeg.exe -y -f dshow -i audio=\"" +  v[inputDeviceIndex] + "\" -acodec " + codec + " \"" + fullPathName +"\"";
    printf("[VERBOSE] %s\n", cmdline.c_str());

    //wchar_t s[ MAX_PATH ];
    //swprintf(s, MAX_PATH, L"ffmpeg.exe", L"-f dshow -i audio=\"%s\" -acodec aac %s", v[inputDeviceIndex].c_str(), fullPathName.c_str());

    STARTUPINFO info={sizeof(info)};
    
    // CreateProcess("c:\\windows\\notepad.exe", "notepad.exe C:\\Users\\Administrator\\Desktop\\projects\\microphone\\package.json", NULL, NULL, TRUE, 0, NULL, NULL, &info, &processInfo);
    BOOL b = CreateProcessA(ffmpegPath.c_str(), const_cast<char *>(cmdline.c_str()), NULL, NULL, TRUE, CREATE_NEW_PROCESS_GROUP, NULL, NULL, &info, &processInfo);
    
    return b ? 0 : 1;
    //ShellExecute(NULL, "open", , NULL, SW_SHOW);
    // ffmpeg -f avfoundation -i ":2" -acodec aac  a.m4a
  } 
  else {
    bquit = false;
    fpath = fullPathName;
    ofile.open (fullPathName, std::ofstream::out | std::ofstream::app | std::ofstream::binary);
    std::thread recordthread (recordthreadfunc);

    return 0;
  }
}


#ifndef MAKEULONGLONG
#define MAKEULONGLONG(ldw, hdw) ((ULONGLONG(hdw) << 32) | ((ldw) & 0xFFFFFFFF))
#endif
  
#ifndef MAXULONGLONG
#define MAXULONGLONG ((ULONGLONG)~((ULONGLONG)0))
#endif
  
bool CloseProcessMainThread(DWORD dwProcID)
{
  DWORD dwMainThreadID = 0;
  ULONGLONG ullMinCreateTime = MAXULONGLONG;
  
  HANDLE hThreadSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
  if (hThreadSnap != INVALID_HANDLE_VALUE) {
    THREADENTRY32 th32;
    th32.dwSize = sizeof(THREADENTRY32);
    BOOL bOK = TRUE;
    for (bOK = Thread32First(hThreadSnap, &th32); bOK;
         bOK = Thread32Next(hThreadSnap, &th32)) {
      if (th32.th32OwnerProcessID == dwProcID) {
        HANDLE hThread = OpenThread(THREAD_QUERY_INFORMATION,
                                    TRUE, th32.th32ThreadID);
        if (hThread) {
          FILETIME afTimes[4] = {0};
          if (GetThreadTimes(hThread,
                             &afTimes[0], &afTimes[1], &afTimes[2], &afTimes[3])) {
            ULONGLONG ullTest = MAKEULONGLONG(afTimes[0].dwLowDateTime,
                                              afTimes[0].dwHighDateTime);
            if (ullTest && ullTest < ullMinCreateTime) {
              ullMinCreateTime = ullTest;
              dwMainThreadID = th32.th32ThreadID; // let it be main... 
            }
          }
          CloseHandle(hThread);
        }
      }
    }
#ifndef UNDER_CE
    CloseHandle(hThreadSnap);
#else
    CloseToolhelp32Snapshot(hThreadSnap);
#endif
  }
  
  if (dwMainThreadID) {
    PostThreadMessage(dwMainThreadID, WM_QUIT, 0, 0); 
  }
  
  return (0 != dwMainThreadID);
}

int recordfunc::stopRecording(){
  if (wmode == 1) {
    // https://stackoverflow.com/questions/2274668/how-to-send-keys-instead-of-characters-to-a-process
    //GenerateConsoleCtrlEvent(0, processInfo.dwProcessId);
    //Sleep(500);
    GenerateConsoleCtrlEvent(1, processInfo.dwProcessId);
    //CloseProcessMainThread(processInfo.dwProcessId);
    // TerminateProcess(processInfo.hProcess, 0);

    // 500 ms timeout; use INFINITE for no timeout
    const DWORD result = WaitForSingleObject(processInfo.hProcess, 500);
    if (result == WAIT_OBJECT_0) {
        // Success
    }
    else {
        // Timed out or an error occurred
      TerminateProcess(processInfo.hProcess, 0);
      WaitForSingleObject(processInfo.hProcess, 500);
    }

    CloseHandle(processInfo.hProcess);
    CloseHandle(processInfo.hThread);

  }
  bquit = true;
  return 0;
}




std::vector<std::string> recordfunc::listRecordingDevices() {
  std::vector<std::wstring> names = {};
  listAudioCaptureDevicesFromWaveInAndRegistry(names);
  std::vector<std::string> devices = {};
  for (uint32_t i=0;i<names.size(); i++)
    devices.push_back(ws2s(names[i]));
  return devices;
}

Napi::Value recordfunc::listRecordingDevicesWrapped(const Napi::CallbackInfo& info) 
{
  Napi::Env env = info.Env();
  std::vector<std::string> v = recordfunc::listRecordingDevices();
  Napi::Array return_array = Napi::Array::New(env, v.size());
  for(uint32_t i = 0; i < v.size(); i++)
   return_array[i] = Napi::String::New(env, (const std::string &)v[i]);
  
  return return_array;
}



Napi::Number recordfunc::startRecordingWrapped(const Napi::CallbackInfo& info) {
   Napi::Env env = info.Env();
    if (info.Length() < 1 || !info[0].IsString()) {
        Napi::TypeError::New(env, "File path expected").ThrowAsJavaScriptException();
    } 

    Napi::String first = info[0].As<Napi::String>();
    Napi::Number inputDeviceIndex = info[1].As<Napi::Number>();
    Napi::Number mode = info[2].As<Napi::Number>();
    Napi::String ffmpegPath = info[3].As<Napi::String>();
    Napi::String codec = info[4].As<Napi::String>();


    int returnValue = recordfunc::startRecording(first.Utf8Value(), inputDeviceIndex.Int32Value(), mode.Int32Value(), ffmpegPath.Utf8Value(), codec.Utf8Value());
    
    return Napi::Number::New(env, returnValue);
}


Napi::Number recordfunc::stopRecordingWrapped(const Napi::CallbackInfo& info) 
{
  Napi::Env env = info.Env();
  return  Napi::Number::New(env, recordfunc::stopRecording());
}


Napi::Object recordfunc::Init(Napi::Env env, Napi::Object exports) 
{
  exports.Set(
    "startRecording", Napi::Function::New(env, recordfunc::startRecordingWrapped)
  );
 exports.Set("stopRecording", Napi::Function::New(env, recordfunc::stopRecordingWrapped));
 exports.Set("listRecordingDevices", Napi::Function::New(env, recordfunc::listRecordingDevicesWrapped));
  return exports;
}
