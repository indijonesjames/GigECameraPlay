#include "pch.h"

using namespace AVT;
using namespace VmbAPI;

extern int NUM_FRAMES;
extern double SIMILARITY;

void PrintCamInfo(const CameraPtr& camera)
{
	std::string strID;
	std::string strName;
	std::string strModelName;
	std::string strSerialNumber;
	std::string strInterfaceID;

	std::ostringstream ErrorStream;

	VmbErrorType err = camera->GetID(strID);
	if (VmbErrorSuccess != err)
	{
		ErrorStream << "[Could not get camera ID. Error code: " << err << "(" << ErrorCodeToMessage(err) << ")" << "]";
		strID = ErrorStream.str();
	}

	err = camera->GetName(strName);
	if (VmbErrorSuccess != err)
	{
		ErrorStream << "[Could not get camera name. Error code: " << err << "(" << ErrorCodeToMessage(err) << ")" << "]";
		strName = ErrorStream.str();
	}

	err = camera->GetModel(strModelName);
	if (VmbErrorSuccess != err)
	{
		ErrorStream << "[Could not get camera mode name. Error code: " << err << "(" << ErrorCodeToMessage(err) << ")" << "]";
		strModelName = ErrorStream.str();
	}

	err = camera->GetSerialNumber(strSerialNumber);
	if (VmbErrorSuccess != err)
	{
		ErrorStream << "[Could not get camera serial number. Error code: " << err << "(" << ErrorCodeToMessage(err) << ")" << "]";
		strSerialNumber = ErrorStream.str();
	}

	err = camera->GetInterfaceID(strInterfaceID);
	if (VmbErrorSuccess != err)
	{
		ErrorStream << "[Could not get interface ID. Error code: " << err << "(" << ErrorCodeToMessage(err) << ")" << "]";
		strInterfaceID = ErrorStream.str();
	}

	std::cout << "/// Camera Name    : " << strName << "\n"
		<< "/// Model Name     : " << strModelName << "\n"
		<< "/// Camera ID      : " << strID << "\n"
		<< "/// Serial Number  : " << strSerialNumber << "\n"
		<< "/// @ Interface ID : " << strInterfaceID << "\n\n";
}

inline VmbErrorType SetValueIntMod2(const CameraPtr& camera, const std::string& featureName, VmbInt64_t& storage)
{
	VmbErrorType    res;
	FeaturePtr      pFeature;
	res = SP_ACCESS(camera)->GetFeatureByName(featureName.c_str(), pFeature);
	if (VmbErrorSuccess == res)
	{
		VmbInt64_t minValue, maxValue;
		res = SP_ACCESS(pFeature)->GetRange(minValue, maxValue);
		if (VmbErrorSuccess == res)
		{
			maxValue = (maxValue >> 1) << 1; // mod 2 dividable
			res = SP_ACCESS(pFeature)->SetValue(maxValue);
			if (VmbErrorSuccess == res)
			{
				storage = maxValue;
			}
		}
	}
	return res;
}
int main()
{
	std::cout << "******************************" << endl;
	std::cout << "*                            *" << endl;
	std::cout << "*                            *" << endl;
	std::cout << "*         LIVE DEMO          *" << endl;
	std::cout << "*                            *" << endl;
	std::cout << "*                            *" << endl;
	std::cout << "******************************" << endl;

	VmbInt64_t nPLS; // Payload size
	FeaturePtr pFeature; // Feature pointer used to control and execute camera properties
	VmbErrorType err = VmbErrorSuccess;
	VimbaSystem& sys = VimbaSystem::GetInstance();
	CameraPtrVector cams;
	CameraPtr cam;
	ApiController API;
	FramePtrVector frames(NUM_FRAMES); // Frame array
	IFrameObserverPtr pFrameObserver;

	// Start the Vimba Engine and API
	err = sys.Startup();
	// Check for errors
	if (VmbErrorSuccess != err)
	{
		std::cout << ErrorCodeToMessage(err) << endl;
		std::cout << "Something went wrong starting the Vimba engine and API, exiting..." << endl;
		sys.Shutdown();
		return 0;
	}

	// Check for cameras
	std::cout << "\nTrying to find a camera..." << endl;
	err = sys.GetCameras(cams);
	if (VmbErrorSuccess != err)
	{
		std::cout << ErrorCodeToMessage(err) << endl;
		std::cout << "Something went wrong looking for the camera/s, exiting..." << endl;
		sys.Shutdown();
		return 0;
	}

	else if (cams.size() < 1)
	{
		std::cout << "No cameras found, ending the program." << endl;
		sys.Shutdown();
		return 0;
	}

	// If successful we connect to the camera
	cam = cams[0];
	PrintCamInfo(cam); // print the model information
	err = cam.get()->Open(VmbAccessModeFull); // connect to the camera

	if (VmbErrorSuccess != err)
	{
		std::cout << ErrorCodeToMessage(err) << endl;
		std::cout << "Something went wrong connecting to the camera/s, exiting..." << endl;
		sys.Shutdown();
		return 0;
	}

	// Next we get the image size for the buffer
	// Then we need to allocate the memory for the frames, and register them to the camera
	cam.get()->GetFeatureByName("PayloadSize", pFeature);
	pFeature.get()->GetValue(nPLS);
	SP_SET(pFrameObserver, new FrameObserver(cam));
	for (FramePtrVector::iterator iter = frames.begin(); frames.end() != iter; ++iter)
	{
		(*iter).reset(new Frame(nPLS));
		(*iter)->RegisterObserver(pFrameObserver);
		cam.get()->AnnounceFrame(*iter);
	}

	// Start the Capture Engine to queue the frames
	err = cam.get()->StartCapture();
	for (FramePtrVector::iterator iter = frames.begin(); frames.end() != iter; ++iter)
	{
		// Put the frame in the queue
		cam.get()->QueueFrame(*iter);
	}

	// Next we need to create our OpenCV mats with the right dimensions
	VmbInt64_t width;
	VmbInt64_t height;
	cv::Mat image1;
	cv::Mat image2;

	err = SetValueIntMod2(cam, "Width", width);
	if (err == VmbErrorSuccess)
	{
		err = SetValueIntMod2(cam, "Height", height);
		if (err == VmbErrorSuccess)
		{
			image1.create(height, width, CV_8UC1);
			image2.create(height, width, CV_8UC1);
		}
	}

	// Now we are ready to start acquiring images
	// Start acquisition engine
	cam.get()->GetFeatureByName("AcquisitionStart", pFeature);
	err = pFeature.get()->RunCommand();

	if (VmbErrorSuccess != err)
	{
		std::cout << ErrorCodeToMessage(err) << endl;
		std::cout << "Error starting the stream with the camera, ending..." << endl;
		cam.get()->Close();
		sys.Shutdown();
		return 0;
	}

	// We need to wait for the buffer to start loading
	Sleep(250);

	if (VmbErrorSuccess != err)
	{
		std::cout << ErrorCodeToMessage(err) << endl;
		std::cout << "Error starting the stream with the camera, ending..." << endl;
		cam.get()->Close();
		sys.Shutdown();
		return 0;
	}

	// While images are being taken we can start processing them

	// First a short calibration to get the similarity
	//  NOTE: in the future this should include creating the homography between the LIDAR and the camera
	// maybe some tweaks to the camera saturation and exposure configuration as well (currently done manual)
	int niter = 0;
	for (int i = 0; i < 30; i++)
	{
		std::cout << "Calibrating Frame: " << i << endl;
		if (niter == 0)
		{
			if (API.GetFrame(image1, cam, pFrameObserver) == VmbFrameStatusComplete)
			{
				niter = 1;
				open_image(image1, "Reference");
			}
		}
		else if (niter == 1)
		{
			if (API.GetFrame(image2, cam, pFrameObserver) == VmbFrameStatusComplete)
			{
				try
				{
					calibrate(image1, image2);
				}
				catch (cv::Exception& e)
				{
					cerr << e.msg << endl;
				}
			}
		}
	}

	std::cout << "Calibration finished, max similairy value found was: " << SIMILARITY << endl;
	// We stop the acquisition to start the true checking
	cam.get()->GetFeatureByName("AcquisitionStop", pFeature);
	err = pFeature.get()->RunCommand();

	if (VmbErrorSuccess != err)
	{
		std::cout << ErrorCodeToMessage(err) << endl;
		std::cout << "Error stopping the stream with the camera, ending..." << endl;
		cam.get()->Close();
		sys.Shutdown();
		return 0;
	}
	
	// While loop to run until ESC is pressed, this way we reuse the allocated frames
	while(!GetAsyncKeyState(VK_ESCAPE))
	{
		// we restart from the beginning 
		cam.get()->GetFeatureByName("AcquisitionStart", pFeature);
		err = pFeature.get()->RunCommand();

		// We need to wait for the buffer to start loading
		Sleep(250);

		niter = 0;
		for (int i = 0; i < NUM_FRAMES; i++)
		{
			std::cout << "Processing Frame: " << i << endl;
			if (niter == 0) // first frame to be used as reference
			{
				if (API.GetFrame(image1, cam, pFrameObserver) == VmbFrameStatusComplete)
				{
					niter = 1;
					//open_image(image1, "First frame");
				}
			}
			else if (niter == 1)
			{
				if (API.GetFrame(image2, cam, pFrameObserver) == VmbFrameStatusComplete)
				{
					try
					{
						cv::namedWindow("feed", cv::WINDOW_NORMAL); // window to show the results
						check_scene(image1, image2); // look for any new or moving object
						image1 = image2; // save the second image as reference for the next iteration
					}
					catch (cv::Exception& e)
					{
						cerr << e.msg << endl;
					}
				}
			}
		}

		// stop the streaming
		cam.get()->GetFeatureByName("AcquisitionStop", pFeature);
		err = pFeature.get()->RunCommand();

	}

	// clean up
	cv::destroyAllWindows();
	cam.get()->EndCapture();
	cam.get()->FlushQueue();
	cam.get()->RevokeAllFrames();
	for (FramePtrVector::iterator iter = frames.begin(); frames.end() != iter; ++iter)
	{
		// Unregister the observer
		(*iter)->UnregisterObserver();
	}

	cam.get()->Close(); // close the communication
	sys.Shutdown(); // shutdown the engine and the API
	return 1;
}

// Different approach for the same purpose. It is outdated although it should still work. I recommend using the one above
int main_old()
{
	std::cout << "******************************" << endl;
	std::cout << "*                            *" << endl;
	std::cout << "*                            *" << endl;
	std::cout << "*         LIVE DEMO          *" << endl;
	std::cout << "*                            *" << endl;
	std::cout << "*                            *" << endl;
	std::cout << "******************************" << endl;

	VmbErrorType err = VmbErrorSuccess;
	VimbaSystem& sys = VimbaSystem::GetInstance();
	err = sys.Startup();
	CameraPtrVector cams;
	CameraPtr cam;
	ApiController API;

	std::cout << "\nTrying to connect to a camera..." << endl;
	err = sys.GetCameras(cams);
	if (VmbErrorSuccess != err)
	{
		std::cout << ErrorCodeToMessage(err) << endl;
		std::cout << "Something went wrong connecting to the camera/s, exiting..." << endl;
		sys.Shutdown();
		return 0;
	}

	else if (cams.size() < 1)
	{
		std::cout << "No cameras found, ending the program." << endl;
		sys.Shutdown();
		return 0;
	}
	
	// If successful we connect to the camera
	cam = cams[0];
	PrintCamInfo(cam);
	string camID;
	err = SP_ACCESS(cam)->GetID(camID);

	// First a short calibration to get the similarity
	//  NOTE: in the future this should include creating the homography between the LIDAR and the camera
	// maybe some tweaks to the camera saturation and exposure configuration as well (currently done manual)

	Set_NUM_FRAMES(30); // only 50 frames needed for calibration
	err = API.StartContinuousImageAcquisition(camID);

	// We need to wait for the buffer to start loading
	Sleep(250);

	if (VmbErrorSuccess != err)
	{
		std::cout << ErrorCodeToMessage(err) << endl;
		std::cout << "Error starting the stream with the camera, ending..." << endl;
		cam.get()->Close();
		sys.Shutdown();
		return 0;
	}

	cv::Mat image1;
	cv::Mat image2;
	int niter = 0;
	for (int i = 0; i < NUM_FRAMES; i++)
	{
	std::cout << "Calibrating Frame: " << i << endl;
		if (niter == 0)
		{
			if (API.GetFrame(image1) == VmbFrameStatusComplete)
			{
				niter = 1;
				open_image(image1, "Reference");
			}
		}
		else if (niter == 1)
		{
			if (API.GetFrame(image2) == VmbFrameStatusComplete)
			{
				try
				{
					calibrate(image1, image2);
				}
				catch (cv::Exception& e)
				{
					cerr << e.msg << endl;
				}
			}
		}
	}
	
	std::cout << "Calibration finished, max similairy value found was: " << SIMILARITY << endl;
	err = API.StopContinuousImageAcquisition();
	Sleep(250);
	// Now we analyze the scene
	Set_NUM_FRAMES(1000); // the more frame the longer you stream for, be careful with running out of memory
	err = API.StartContinuousImageAcquisition(camID);

	// We need to wait for the buffer to start loading
	Sleep(5000);

	if (VmbErrorSuccess != err)
	{
		std::cout << ErrorCodeToMessage(err) << endl;
		std::cout << "Error starting the stream with the camera, ending..." << endl;
		cam.get()->Close();
		sys.Shutdown();
		return 0;
	}

	niter = 0;
	for (int i = 0; i < NUM_FRAMES; i++)
	{
		std::cout << "Processing Frame: " << i << endl;
		if (niter == 0)
		{
			if (API.GetFrame(image1) == VmbFrameStatusComplete)
			{
				niter = 1;
				open_image(image1, "First frame");
			}
		}
		else if (niter == 1)
		{
			if (API.GetFrame(image2) == VmbFrameStatusComplete)
			{
				try
				{
					check_scene(image1, image2);
					image1 = image2;
				}
				catch (cv::Exception& e)
				{
					cerr << e.msg << endl;
				}
			}
		}
	}


	/*
	for (int i = 0; i < NUM_FRAMES; i++)
	{
		std:cout << "Processing Frame: " << i << endl;
		if (API.GetFrame(image) == VmbFrameStatusComplete) open_image(image, "Test");
	}
	*/

	err = API.StopContinuousImageAcquisition();
	cam.get()->Close();
	sys.Shutdown();
	return 1;
}


