Shader  "Custom/perlin_ascii_art" {
	Properties {
		_Octaves ("Octaves", Float) = 8.0
		_Frequency ("Frequency", Float) = 1.0
		_Amplitude ("Amplitude", Float) = 1.0
		_Lacunarity ("Lacunarity", Float) = 1.92
		_Persistence ("Persistence", Float) = 0.8
		_Offset ("Offset", Vector) = (0.0, 0.0, 0.0, 0.0)
		_AnimSpeed("Animation Speed", Float) = 1.0

		_DigitTex ("Digit (RGB)", 2D) = "white" {}
		_TileScale ("Tile Scale", Vector) = ( 128, 128, 16, 1 )
		_DistBlend ("Distance Blending Factor", Vector) = ( 10, 200, 0, 0 )

		_MainColor ("Main Color", Color) = (1, 1, 1, 1)
		_ColorShift ("Color Shift", Color) = (0, 0, 0, 0)
	}

	CGINCLUDE
		//
		//	FAST32_hash
		//	A very fast hashing function.  Requires 32bit support.
		//	http://briansharpe.wordpress.com/2011/11/15/a-fast-and-simple-32bit-floating-point-hash-function/
		//
		//	The hash formula takes the form....
		//	hash = mod( coord.x * coord.x * coord.y * coord.y, SOMELARGEFLOAT ) / SOMELARGEFLOAT
		//	We truncate and offset the domain to the most interesting part of the noise.
		//	SOMELARGEFLOAT should be in the range of 400.0->1000.0 and needs to be hand picked.  Only some give good results.
		//	3D Noise is achieved by offsetting the SOMELARGEFLOAT value by the Z coordinate
		//
		   void FAST32_hash_3D( 	float3 gridcell,
								out float4 lowz_hash_0,
								out float4 lowz_hash_1,
								out float4 lowz_hash_2,
								out float4 highz_hash_0,
								out float4 highz_hash_1,
								out float4 highz_hash_2	)		//	generates 3 random numbers for each of the 8 cell corners
		{
			//    gridcell is assumed to be an integer coordinate
		
			//	TODO: 	these constants need tweaked to find the best possible noise.
			//			probably requires some kind of brute force computational searching or something....
			const float2 OFFSET = float2( 50.0, 161.0 );
			const float DOMAIN = 69.0;
			const float3 SOMELARGEFLOATS = float3( 635.298681, 682.357502, 668.926525 );
			const float3 ZINC = float3( 48.500388, 65.294118, 63.934599 );
		
			//	truncate the domain
			gridcell.xyz = gridcell.xyz - floor(gridcell.xyz * ( 1.0 / DOMAIN )) * DOMAIN;
			float3 gridcell_inc1 = step( gridcell, float3( DOMAIN - 1.5, DOMAIN - 1.5, DOMAIN - 1.5 ) ) * ( gridcell + 1.0 );
		
			//	calculate the noise
			float4 P = float4( gridcell.xy, gridcell_inc1.xy ) + OFFSET.xyxy;
			P *= P;
			P = P.xzxz * P.yyww;
			float3 lowz_mod = float3( 1.0 / ( SOMELARGEFLOATS.xyz + gridcell.zzz * ZINC.xyz ) );
			float3 highz_mod = float3( 1.0 / ( SOMELARGEFLOATS.xyz + gridcell_inc1.zzz * ZINC.xyz ) );
			lowz_hash_0 = frac( P * lowz_mod.xxxx );
			highz_hash_0 = frac( P * highz_mod.xxxx );
			lowz_hash_1 = frac( P * lowz_mod.yyyy );
			highz_hash_1 = frac( P * highz_mod.yyyy );
			lowz_hash_2 = frac( P * lowz_mod.zzzz );
			highz_hash_2 = frac( P * highz_mod.zzzz );
		}
		//
		//	Interpolation functions
		//	( smoothly increase from 0.0 to 1.0 as x increases linearly from 0.0 to 1.0 )
		//	http://briansharpe.wordpress.com/2011/11/14/two-useful-interpolation-functions-for-noise-development/
		//
		float3 Interpolation_C2( float3 x ) { return x * x * x * (x * (x * 6.0 - 15.0) + 10.0); }
		//
		//	Perlin Noise 3D  ( gradient noise )
		//	Return value range of -1.0->1.0
		//	http://briansharpe.files.wordpress.com/2011/11/perlinsample.jpg
		//
		float Perlin3D( float3 P )
		{
			//	establish our grid cell and unit position
			float3 Pi = floor(P);
			float3 Pf = P - Pi;
			float3 Pf_min1 = Pf - 1.0;
		
			//
			//	classic noise.
			//	requires 3 random values per point.  with an efficent hash function will run faster than improved noise
			//
		
			//	calculate the hash.
			//	( various hashing methods listed in order of speed )
			float4 hashx0, hashy0, hashz0, hashx1, hashy1, hashz1;
			FAST32_hash_3D( Pi, hashx0, hashy0, hashz0, hashx1, hashy1, hashz1 );
		
			//	calculate the gradients
			float4 grad_x0 = hashx0 - 0.49999;
			float4 grad_y0 = hashy0 - 0.49999;
			float4 grad_z0 = hashz0 - 0.49999;
			float4 grad_x1 = hashx1 - 0.49999;
			float4 grad_y1 = hashy1 - 0.49999;
			float4 grad_z1 = hashz1 - 0.49999;
			float4 grad_results_0 = rsqrt( grad_x0 * grad_x0 + grad_y0 * grad_y0 + grad_z0 * grad_z0 ) * ( float2( Pf.x, Pf_min1.x ).xyxy * grad_x0 + float2( Pf.y, Pf_min1.y ).xxyy * grad_y0 + Pf.zzzz * grad_z0 );
			float4 grad_results_1 = rsqrt( grad_x1 * grad_x1 + grad_y1 * grad_y1 + grad_z1 * grad_z1 ) * ( float2( Pf.x, Pf_min1.x ).xyxy * grad_x1 + float2( Pf.y, Pf_min1.y ).xxyy * grad_y1 + Pf_min1.zzzz * grad_z1 );
		
			//	Classic Perlin Interpolation
			float3 blend = Interpolation_C2( Pf );
			float4 res0 = lerp( grad_results_0, grad_results_1, blend.z );
			float2 res1 = lerp( res0.xy, res0.zw, blend.y );
			float final = lerp( res1.x, res1.y, blend.x );
			final *= 1.1547005383792515290182975610039;		//	(optionally) scale things to a strict -1.0->1.0 range    *= 1.0/sqrt(0.75)
			return final;
		}
		float PerlinNormal(float3 p, int octaves, float3 offset, float frequency, float amplitude, float lacunarity, float persistence)
		{
			float sum = 0;
			for (int i = 0; i < octaves; i++)
			{
				float h = 0;
				h = Perlin3D((p + offset) * frequency);
				sum += h*amplitude;
				frequency *= lacunarity;
				amplitude *= persistence;
			}
			return sum;
		}

	ENDCG

	SubShader {
		Tags { "RenderType"="Opaque" }
		LOD 200

		CGPROGRAM
		#pragma surface surf Lambert vertex:vert
		#pragma glsl

		// #pragma surface surf Standard fullforwardshadows
		#pragma target 3.0

		fixed _Octaves;
		float _Frequency;
		float _Amplitude;
		float3 _Offset;
		float _Lacunarity;
		float _Persistence;
		float _AnimSpeed;

		sampler2D _DigitTex;
		float4 _DigitTex_TexelSize;
		float4 _TileScale;
		float4 _DistBlend;

		float4 _MainColor;
		float4 _ColorShift;

		struct Input {
			float3 pos;

			float2 uv_DigitTex : TEXCOORD0;

			float blend_factor;
		};

		void vert (inout appdata_full v, out Input OUT)
		{
			UNITY_INITIALIZE_OUTPUT(Input, OUT);

			OUT.pos = float3(v.texcoord.xy, _Time.y * _AnimSpeed);

			half3 viewDirW = _WorldSpaceCameraPos - mul((half4x4)unity_ObjectToWorld, v.vertex);
			half viewDist = length(viewDirW);
			half falloff = saturate((viewDist - _DistBlend.x) / _DistBlend.y);
			OUT.blend_factor = (1.0f - falloff);
		}

		void surf (Input IN, inout SurfaceOutput o) {
			float gray = PerlinNormal(IN.pos, _Octaves, _Offset, _Frequency, _Amplitude, _Lacunarity, _Persistence);

			float2 uv_scaled = IN.uv_DigitTex * _TileScale.xy;
			float2 uv_floor = floor(uv_scaled);
			float2 uv_local = uv_scaled - uv_floor;
			float2 uv = uv_floor / _TileScale.xy + 1 / (_TileScale.xy * 2);

			float4 c = 1 - PerlinNormal(float3(uv, IN.pos.z), _Octaves, _Offset, _Frequency, _Amplitude, _Lacunarity, _Persistence);;
			float level = floor(c * _TileScale.z) / _TileScale.z;
			float2 uv_level = float2((level + uv_local.x / _TileScale.z), uv_local.y);
			float4 digit = tex2D (_DigitTex, uv_level);
			float t = (int(fmod(digit.x, 2)) == 1 ? 1 : 0);

			float alpha = IN.blend_factor;

			o.Albedo = lerp(gray, t, alpha) * _MainColor + _ColorShift;
			// o.Albedo = t;
			o.Alpha = 1.0;
		}
		ENDCG
	}
	FallBack "Diffuse"
}
