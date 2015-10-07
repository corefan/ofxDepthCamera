#include "ofxDepthCameraProvider.h"

ofxDepthCameraProvider::ofxDepthCameraProvider() {
	bLive = true;
	bRemote = false;
	bRecording = false;
	bPlaying = false;
	bPlayerLoaded = false;

	name = "cam";
	recorder.setFormat("raw");
}

ofxDepthCameraProvider::~ofxDepthCameraProvider() {
	device->close();
}

void ofxDepthCameraProvider::setup(ofxBaseDepthCamera& baseCam) {
	device = &baseCam;

	player.setSize(device->getDepthWidth(), device->getDepthHeight());
	player.setImageType(OF_IMAGE_GRAYSCALE);
}

void ofxDepthCameraProvider::update() {
	if (bLive) {
		if (!bRemote) {
			device->update();;
			
			if (bRecording && device->isFrameNew()) {
				recorder.addFrame(device->getRawDepth());
			}
		} else {
			receiver.update();

			// TODO Check for new frames when recording
			if (bRecording) {
				recorder.addFrame(receiver.getDepthPixels());
			}
		}
	} else {
		player.update();
	}

	if (!bRecording && recorder.isThreadRunning() && !recorder.getQueueLength()) {
		recorder.stopThread();
	}
}

void ofxDepthCameraProvider::draw(int x, int y, int w, int h) {
	w = w ? w : device->getDepthWidth();
	h = h ? h : device->getDepthHeight();
	getDepthImage().draw(x, y, w, h);
}

void ofxDepthCameraProvider::setName(string name) {
	this->name = name;
}

void ofxDepthCameraProvider::setLive() {
	bLive = true;

	if (bPlaying) {
		pause();
	}
	if (bRecording) {
		endRecording();
	}	
}

void ofxDepthCameraProvider::setLocal() {
	bRemote = false;
	receiver.disconnect();
}

void ofxDepthCameraProvider::setRemote(string host, int port) {
	if (receiver.getHost().empty() || !receiver.getPort()) {
		if (host.empty() || !port) {
			ofLogError("ofxDepthCameraProvider", "Set remote params with setRemote(\"host\", port) first");
			return;
		}

		receiver.setup(device->getDepthWidth(), device->getDepthHeight(), host, port);
	}

	bRemote = true;
	receiver.connect();
}

void ofxDepthCameraProvider::setRecordPath(string path) {
	recordPath += "/" + name + "_";
	recorder.setPrefix(recordPath);
}

void ofxDepthCameraProvider::beginRecording(string path) {
	if (path.empty()) {
		path = "recordings/" + ofGetTimestampString("%Y-%m-%d-%H-%M-%S");
	}
	else {
		path = "recordings/" + path;
	}

	bRecording = true;
	setRecordPath(path);
	recorder.resetCounter();
	recorder.startThread();
}

void ofxDepthCameraProvider::endRecording() {
	player.loadSequence(recordPath, "raw", 0, recorder.getFrameCount(), 4, device->frameRate());
	bPlayerLoaded = player.getTotalFrames() > 0;
}

void ofxDepthCameraProvider::setPlaybackPath(string path) {
	player.loadSequence(path);
	bPlayerLoaded = player.getTotalFrames() > 0;
}

void ofxDepthCameraProvider::play(string path) {
	if (!bPlayerLoaded) {
		if (path.empty()) {
			ofLogError("ofxDepthCameraProvider", "Set playback path with play(\"path\") first");
			return;
		}

		setPlaybackPath(path);
	}

	bPlaying = true;
	player.play();
}

void ofxDepthCameraProvider::pause() {
	bPlaying = false;
	player.pause();

}
void ofxDepthCameraProvider::stop() {
	bPlaying = false;
	player.stop();
}

ofShortPixels& ofxDepthCameraProvider::getRawDepth() {
	if (bLive) {
		if (!bRemote) {
			return device->getRawDepth();
		} else {
			return receiver.getDepthPixels();
		}
	} else {
		return player.getSequence().getPixels();
	}
}

ofImage& ofxDepthCameraProvider::getDepthImage() {
	if (bLive) {
		if (!bRemote) {
			return device->getDepthImage();
		}
		else {
			return receiver.getDepthImage();
		}
	}
	else {
		// TODO Implement depthImage for SequencePlayback
		//return player.getSequence().getPixels();
	}
}

ofImage& ofxDepthCameraProvider::getRawIRImage() {
	if (bLive) {
		if (!bRemote) {
			return device->getRawIRImage();
		}
		else {
			// Not supporting remote IR
			//return receiver.getPixels();
		}
	}
	else {
		// Not supporting playback of IR
		//return player.getSequence().getPixels();
	}
}

ofImage& ofxDepthCameraProvider::getColorImage() {
	if (bLive) {
		if (!bRemote) {
			return device->getColorImage();
		}
		else {
			// Not supporting remote color
			//return receiver.getPixels();
		}
	}
	else {
		// Not supporting playback of color
		//return player.getSequence().getPixels();
	}
}