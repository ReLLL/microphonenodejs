const microphoneAddon = require('./index.js');

console.log('Listing the devices:');
console.log(microphoneAddon.listRecordingDevices());


console.log('Start recording...');
const fullPathName = 'c://test.mp3';  // use .m4a extension for aac codec
const inputDeviceIndex = 0; // Record from the first device (0 is default audio input device)
const codec = 'libmp3lame';  // aac, ac3, flac, opus, libmp3lame, wavpack    https://www.ffmpeg.org/ffmpeg-codecs.html
microphoneAddon.startRecording(fullPathName, inputDeviceIndex, codec);



console.log('Waiting for 5 sec to have something recorded...');
function stopfunc() {
	console.log('Stop recording...');
	microphoneAddon.stopRecording();
}
setTimeout(stopfunc, 5000);  // stop recording after 5 seconds