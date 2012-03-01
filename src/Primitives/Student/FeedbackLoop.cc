void FeedbackLoop()	{
	// Create directory to house new files
	if (mkdir("/data/training_data_1") < 0) { // Is this correct or ext/ROS/KinectNaoPipeline/data???
		perror("mkdir");
	}
	
	// Select each gesture file in dictionary to receive feedback
	// Also, create a new file in the new directory for demo data
	DIR *dir;
	struct dirent *gesture;
	dir = opendir("/data/training_data");
	while ((entry = readdir(dir)) != NULL) {
		
		// Create matching_file in /data/training_data_i
	
		// Send that gesture to playback
		PlaybackAndGetFeedback(entry->d_name, matching_file);
	}

}

void PlaybackAndGetFeedback(gesture_file, new_file) {

	// Have the robot playback the action in the csv file
	System("ext/ROS/KinectNaoPipeline/src/playback.cc", gesture_file, --nonblocking);
	
	// Create feedback sensor so you can poll for feedback
	sensor_ = new FeedbackSensor(20000);
	sensor_->Poll();
	
	// Internalize feedback 
	if (sensor_->GetValues()[0] == 0) {		// "GOOD"
		// Write a line to new_file indicating it's okay
	}

	else if (sensor_->GetValues()[0] == 1 || sensor_->GetValues()[0] == 2) { // "BAD" or "PANIC"
		// Nao asks for demonstration
		
		// Capture demonstration 
		System("ext/ROS/KinectNaoPipeline/src/capture.cc", new_file, --nonblocking);
		// Run RAPTOR on new_file <- not sure where/ how to call this
	}
	
	else { // "DONE" <- what should be done here? solicit feedback again? 
		
	}

}

