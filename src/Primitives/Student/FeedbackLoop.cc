void FeedbackLoop()	{
	// Create directory to house new files
	if (mkdir("/data/training_data_1") < 0) { // Is this correct or ext/ROS/KinectNaoPipeline/data???
		perror("mkdir");
	}
	
	// Select each gesture file in dictionary to receive feedback
	// Also, create a new file in the new directory for demo data
	DIR *dir;
	struct dirent *entry;
	dir = opendir("/data/training_data");
	while ((entry = readdir(dir)) != NULL) {
		// Send that gesture to playback
		PlaybackAndGetFeedback(entry->d_name, "/data/training_data_1/" + entry->d_name);
	}

}

void PlaybackAndGetFeedback(char *gesture_file, char *new_file) {

	// Have the robot playback the action in the csv file
        // Should this be a call to the executable?
	System("ext/ROS/KinectNaoPipeline/src/playback.cc " + gesture_file + " --nonblocking");
	
	// Create feedback sensor so you can poll for feedback
	sensor = new FeedbackSensor(20000);

  ofstream file;
  file.open(new_file);
	
	// Internalize feedback 
	if (sensor->GetValues()[0] == 0) {		// "GOOD"
		// Write a line to new_file indicating it's okay
    file << "OK";
    ofstream.close();
	} else if (sensor->GetValues()[0] == 1 || sensor->GetValues()[0] == 2) { // "BAD" or "PANIC"
		// Nao asks for demonstration
	
		// Capture demonstration
                // I think this call should be to an executable, not the .cc
		System("ext/ROS/KinectNaoPipeline/src/capture.cc " + new_file);
		// Run RAPTOR on new_file <- not sure where/ how to call this
	}
	
	else { // "DONE" <- what should be done here? solicit feedback again? 
		
	}

}

