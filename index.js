const microphoneAddon = require('./build/Release/microphone.node');
var path = require('path');


module.exports.listRecordingDevices = function listRecordingDevices() {
	microphoneAddon.listRecordingDevices();
}

module.exports.startRecording = function startRecording(fullPathName, inputDeviceIndex, codec) {
	// get relative path of ffmpeg.exe embedded inside package
	var ffmpegPath = path.join(__dirname, 'embed', 'ffmpeg.exe');  
	console.log('[VERBOSE] ' + ffmpegPath);

	const mode = 1;  // mode 0=WindowsAPI/WAV  1=ffmpeg/MP4   (please use mode 1 for now,  WAV headers are complicated and can take time to implement)
	microphoneAddon.startRecording(fullPathName, inputDeviceIndex, mode, ffmpegPath, codec);
}

module.exports.stopRecording = function stopRecording() {
	microphoneAddon.stopRecording();
}




