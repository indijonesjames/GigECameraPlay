# Quick Demo of GigE Camera (AV).

### OVERVIEW

This project is meant to be used to connect to a GigE Allied Vision camera that supports the Vimba SDK, such as MAKO. The project will automatically try to detect the camera and connect to it.

If the connection is useful, the camera will be set up for continuous streaming, and 500 frames will be allocated. After a short calibration the camera will start streaming and frames will be compared using OpenCV.

OpenCV is used to find new or moving objects between frames, isolate them and find the centroid. 

### Key Components

The key components of the project are:

- The way the Vimba SDK is used to be able to live stream for the camera.
- How the frames are allocated once only, and then reused in an infinite loop until the user presses ESCAPE.
- Unfortunately, frames do not arrive as fast as we would like. In consequent, continuous checks for the frame to arrive are needed. This is done used the Frame Observer class.
- The Vimba SDK gives you the tool to communicate with the camera, but user specific classes need to be implemented to achieve the desired functionality. This project implements three classes:
	- APIController
	- FrameObserver
	- CameraObserver
- Shared pointers are widely used.
- How OpenCV is used to check for differences, and how different methods are implemented to do preliminary work for efficiency.

### Needed Libraries

- OpenCV 
- Vimba SDK

### Future Work

Improve the image processing part, as of right now it was done to a point where new object are detected, and the biggest component is drawn and bounded. However, this could be improved to a point where the whole object is bounded.
