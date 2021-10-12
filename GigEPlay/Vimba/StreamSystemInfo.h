#pragma once

#include "pch.h"
#include <exception>

namespace AVT {
	namespace VmbAPI {

		template<typename STREAM>
		STREAM& operator<<(STREAM& os, AVT::VmbAPI::VimbaSystem& sys)
		{
			VmbVersionInfo_t info;
			if (VmbErrorSuccess != sys.QueryVersion(info))
			{
				throw std::exception();
			}
			os << info.major << "." << info.minor << "." << info.patch;
			return os;
		}

	}
}