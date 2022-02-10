#pragma once
#include "_feature_base.h"

namespace uif::features {

	class tunnel_decoder : public feature_base
	{
	public:
		explicit tunnel_decoder(uif::injector& injector) : feature_base(injector, "tunnel_encoding") {}
		void initialize() override;
		void finalize() override;

		std::wstring decode(const char* text, int count) const;

		std::wstring mapping{};
		bool enabled = false;
	};
}
