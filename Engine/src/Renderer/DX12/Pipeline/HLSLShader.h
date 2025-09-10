#pragma once

namespace Engine {

	class HLSLShader{

	public:

		HLSLShader() = default;
		~HLSLShader();

		enum ShaderType {
			VERTEX,
			PIXEL,
		};

		void Initialize(const LPCWSTR filename, const ShaderType type);

		void Release();

	private:

		ID3DBlob* byteCode = nullptr; 


	};

}


