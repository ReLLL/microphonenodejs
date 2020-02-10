# Microphone Recorder NodeJS Addon

Written using VC++ and N-API  


## EXAMPLE USAGE
Check example.js to see the usage
node example.js

```js
const microphoneAddon = require('./index.js');

console.log('Listing the devices:');
console.log(microphoneAddon.listRecordingDevices());


console.log('Start recording...');
const fullPathName = 'c://test.mp3';  // use .m4a extension for aac codec
const inputDeviceIndex = 0; // Record from the first device  (0 is default audio input device)
const codec = 'libmp3lame';  // aac, ac3, flac, opus, libmp3lame, wavpack    https://www.ffmpeg.org/ffmpeg-codecs.html
microphoneAddon.startRecording(fullPathName, inputDeviceIndex, codec);



console.log('Waiting for 5 sec to have something recorded...');
function stopfunc() {
	console.log('Stop recording...');
	microphoneAddon.stopRecording();
}
setTimeout(stopfunc, 5000);  // stop recording after 5 seconds
```


## Exported functions

function listRecordingDevices() 

function startRecording(fullPathName, inputDeviceIndex, codec)

function stopRecording()


## BUILD STEPS
- Install VS 2019  (Express should be fine   https://visualstudio.microsoft.com/vs/express/)
- install the latest node version   
- Run the following command to build (in project directory)
npm run build 
- If it fails, please run the following commands (in project directory)
npm install node-gyp --save-dev
npm install node-addon-api


### Notes:
The development platform I used:
- Node v12.15.0 
- Visual Studio 2019 v16.4.4
- Windows Server 2019 (Version 1809)