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

		inline ID3DBlob* GetByteBlob() { return byteCode; }

		void Release();

	private:

		ID3DBlob* byteCode = nullptr; 


	};

}


