#pragma once

//#include "pch.h"
#include "../pch.h"


namespace AVT {
	namespace VmbAPI {

		VmbErrorType TransformImage(const FramePtr& SourceFrame, std::vector<VmbUchar_t>& DestinationData, const std::string& DestinationFormat)
		{
			if (SP_ISNULL(SourceFrame))
			{
				return VmbErrorBadParameter;
			}
			VmbErrorType        Result;
			VmbPixelFormatType  InputFormat;
			VmbUint32_t         InputWidth, InputHeight;
			Result = SP_ACCESS(SourceFrame)->GetPixelFormat(InputFormat);
			if (VmbErrorSuccess != Result)
			{
				return Result;
			}
			Result = SP_ACCESS(SourceFrame)->GetWidth(InputWidth);
			if (VmbErrorSuccess != Result)
			{
				return Result;
			}
			Result = SP_ACCESS(SourceFrame)->GetHeight(InputHeight);
			if (VmbErrorSuccess != Result)
			{
				return Result;
			}
			// Prepare source image
			VmbImage SourceImage;
			SourceImage.Size = sizeof(SourceImage);
			Result = static_cast<VmbErrorType>(VmbSetImageInfoFromPixelFormat(InputFormat, InputWidth, InputHeight, &SourceImage));
			if (VmbErrorSuccess != Result)
			{
				return Result;
			}
			VmbUchar_t* DataBegin = NULL;
			Result = SP_ACCESS(SourceFrame)->GetBuffer(DataBegin);
			if (VmbErrorSuccess != Result)
			{
				return Result;
			}
			SourceImage.Data = DataBegin;
			// Prepare destination image
			VmbImage DestinationImage;
			DestinationImage.Size = sizeof(DestinationImage);
			Result = static_cast<VmbErrorType>(VmbSetImageInfoFromString(DestinationFormat.c_str(), static_cast<VmbUint32_t>(DestinationFormat.size()), InputWidth, InputHeight, &DestinationImage));
			if (VmbErrorSuccess != Result)
			{
				return Result;
			}
			const size_t ByteCount = (DestinationImage.ImageInfo.PixelInfo.BitsPerPixel * InputWidth * InputHeight) / 8;
			DestinationData.resize(ByteCount);
			DestinationImage.Data = &*DestinationData.begin();
			// Transform data
			Result = static_cast<VmbErrorType>(VmbImageTransform(&SourceImage, &DestinationImage, NULL, 0));
			return Result;
		}

		VmbErrorType TransformImage(const FramePtr& SourceFrame, std::vector<VmbUchar_t>& DestinationData, const std::string& DestinationFormat, const VmbFloat_t* Matrix)
		{
			if (SP_ISNULL(SourceFrame))
			{
				return VmbErrorBadParameter;
			}
			if (NULL == Matrix)
			{
				return VmbErrorBadParameter;
			}
			VmbErrorType        Result;
			VmbPixelFormatType  InputFormat;
			VmbUint32_t         InputWidth, InputHeight;
			Result = SP_ACCESS(SourceFrame)->GetPixelFormat(InputFormat);
			if (VmbErrorSuccess != Result)
			{
				return Result;
			}
			Result = SP_ACCESS(SourceFrame)->GetWidth(InputWidth);
			if (VmbErrorSuccess != Result)
			{
				return Result;
			}
			Result = SP_ACCESS(SourceFrame)->GetHeight(InputHeight);
			if (VmbErrorSuccess != Result)
			{
				return Result;
			}
			// Prepare source image
			VmbImage SourceImage;
			SourceImage.Size = sizeof(SourceImage);
			Result = static_cast<VmbErrorType>(VmbSetImageInfoFromPixelFormat(InputFormat, InputWidth, InputHeight, &SourceImage));
			if (VmbErrorSuccess != Result)
			{
				return Result;
			}
			VmbUchar_t* DataBegin = NULL;
			Result = SP_ACCESS(SourceFrame)->GetBuffer(DataBegin);
			if (VmbErrorSuccess != Result)
			{
				return Result;
			}
			SourceImage.Data = DataBegin;
			// Prepare destination image
			VmbImage DestinationImage;
			DestinationImage.Size = sizeof(DestinationImage);
			Result = static_cast<VmbErrorType>(VmbSetImageInfoFromString(DestinationFormat.c_str(), static_cast<VmbUint32_t>(DestinationFormat.size()), InputWidth, InputHeight, &DestinationImage));
			if (VmbErrorSuccess != Result)
			{
				return Result;
			}
			const size_t ByteCount = (DestinationImage.ImageInfo.PixelInfo.BitsPerPixel * InputWidth * InputHeight) / 8;
			DestinationData.resize(ByteCount);
			DestinationImage.Data = &*DestinationData.begin();
			// Setup Transform parameter

			// Transform data
			VmbTransformInfo TransformInfo;
			Result = static_cast<VmbErrorType>(VmbSetColorCorrectionMatrix3x3(Matrix, &TransformInfo));
			if (VmbErrorSuccess != Result)
			{
				return Result;
			}
			Result = static_cast<VmbErrorType>(VmbImageTransform(&SourceImage, &DestinationImage, &TransformInfo, 1));
			return Result;
		}

	}
}