#pragma once

#include "pch.h"


typedef std::string      string_type;
#define MAKE_STRING_LITERAL_(s) s

#define MAKE_STRING_LITERAL(s) MAKE_STRING_LITERAL_(s)
namespace AVT {
	namespace VmbAPI {

		inline string_type ErrorCodeToMessage(VmbError_t eError)
		{
			switch (eError)
			{
			case VmbErrorSuccess:           return string_type(MAKE_STRING_LITERAL("Success."));
			case VmbErrorInternalFault:     return string_type(MAKE_STRING_LITERAL("Unexpected fault in VmbApi or driver."));
			case VmbErrorApiNotStarted:     return string_type(MAKE_STRING_LITERAL("API not started."));
			case VmbErrorNotFound:          return string_type(MAKE_STRING_LITERAL("Not found."));
			case VmbErrorBadHandle:         return string_type(MAKE_STRING_LITERAL("Invalid handle "));
			case VmbErrorDeviceNotOpen:     return string_type(MAKE_STRING_LITERAL("Device not open."));
			case VmbErrorInvalidAccess:     return string_type(MAKE_STRING_LITERAL("Invalid access."));
			case VmbErrorBadParameter:      return string_type(MAKE_STRING_LITERAL("Bad parameter."));
			case VmbErrorStructSize:        return string_type(MAKE_STRING_LITERAL("Wrong DLL version."));
			case VmbErrorMoreData:          return string_type(MAKE_STRING_LITERAL("More data returned than memory provided."));
			case VmbErrorWrongType:         return string_type(MAKE_STRING_LITERAL("Wrong type."));
			case VmbErrorInvalidValue:      return string_type(MAKE_STRING_LITERAL("Invalid value."));
			case VmbErrorTimeout:           return string_type(MAKE_STRING_LITERAL("Timeout."));
			case VmbErrorOther:             return string_type(MAKE_STRING_LITERAL("TL error."));
			case VmbErrorResources:         return string_type(MAKE_STRING_LITERAL("Resource not available."));
			case VmbErrorInvalidCall:       return string_type(MAKE_STRING_LITERAL("Invalid call."));
			case VmbErrorNoTL:              return string_type(MAKE_STRING_LITERAL("TL not loaded."));
			case VmbErrorNotImplemented:    return string_type(MAKE_STRING_LITERAL("Not implemented."));
			case VmbErrorNotSupported:      return string_type(MAKE_STRING_LITERAL("Not supported."));
			default:                        return string_type(MAKE_STRING_LITERAL("Unknown"));
			}
		}

	}
}

