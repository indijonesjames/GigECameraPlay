// pch.h: This is a precompiled header file.
// Files listed below are compiled only once, improving build performance for future builds.
// This also affects IntelliSense performance, including code completion and many code browsing features.
// However, files listed here are ALL re-compiled if any one of them is updated between builds.
// Do not add files here that you will be updating frequently as this negates the performance advantage.

#ifndef PCH_H
#define PCH_H

// System libraries
#include <stdio.h>
#include <iostream>
#include <sstream>
#include <tchar.h>
#include <string>
// OpenCV
#include "opencv2/core.hpp"
#include "opencv2/core/core_c.h"
#include "opencv2/features2d.hpp"
#include "opencv2/opencv.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/videoio/videoio.hpp"
#include "opencv2/video.hpp"
#include "opencv2/xfeatures2d/nonfree.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/xfeatures2d.hpp"
// Vimba (camera)
#include "VimbaCPP.h"
#include "VimbaImageTransform/Include/VmbTransform.h"
// Project specifc
#include "ImageProcessing/image_process.h"
#include "const.h"
#include "Vimba/ApiController.h"
#include "Vimba/CameraObserver.h"
#include "Vimba/FrameObserver.h"
#include "Vimba/StreamSystemInfo.h"
#include "Vimba/ErrorCodeToMessage.h"



// add headers that you want to pre-compile here

#endif //PCH_H
