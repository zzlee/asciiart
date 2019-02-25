Shader "Custom/ascii_art" {
	Properties {
		_MainTex ("Albedo (RGB)", 2D) = "white" {}
		_DigitTex ("Digit (RGB)", 2D) = "white" {}
		_TileScale ("Tile Scale", Vector) = ( 128, 128, 16, 1 )
		_DistBlend ("Distance Blending Factor", Vector) = ( 10, 200, 0, 0 )
	}

	SubShader {
		Tags { "RenderType"="Opaque" }
		LOD 200

		CGPROGRAM
		#pragma surface surf Lambert vertex:vert

		#pragma target 3.0

		sampler2D _MainTex;
		float4 _MainTex_TexelSize;
		sampler2D _DigitTex;
		float4 _DigitTex_TexelSize;
		float4 _TileScale;
		float4 _DistBlend;

		struct Input {
			float2 uv_MainTex : TEXCOORD0;
			float2 uv_DigitTex : TEXCOORD0;

			float blend_factor;
		};

		void vert (inout appdata_full v, out Input OUT)
		{
			UNITY_INITIALIZE_OUTPUT(Input, OUT);

			half3 viewDirW = _WorldSpaceCameraPos - mul((half4x4)unity_ObjectToWorld, v.vertex);
			half viewDist = length(viewDirW);
			half falloff = saturate((viewDist - _DistBlend.x) / _DistBlend.y);
			OUT.blend_factor = (1.0f - falloff);
		}

		void surf (Input IN, inout SurfaceOutput o) {
			float2 uv_scaled = IN.uv_MainTex * _TileScale.xy;
			float2 uv_floor = floor(uv_scaled);
			float2 uv_local = uv_scaled - uv_floor;
			float2 uv = uv_floor / _TileScale.xy + 1 / (_TileScale.xy * 2);

			float4 c = tex2D (_MainTex, uv);
			float gray = 1 - (c.r + c.g + c.b) / 3;
			float level = floor(gray * _TileScale.z) / _TileScale.z;
			float2 uv_level = float2((level + uv_local.x / _TileScale.z), uv_local.y);
			float4 digit = tex2D (_DigitTex, uv_level);
			float t = (int(fmod(digit.x, 2)) == 1 ? 1 : 0);

			float alpha = IN.blend_factor;

			o.Albedo = lerp(c, t, alpha);
			// o.Albedo = t;
			o.Alpha = 1.0;
		}
		ENDCG
	}
	FallBack "Diffuse"
}
