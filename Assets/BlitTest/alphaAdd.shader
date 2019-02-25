// Shader created with Shader Forge v1.38 
// Shader Forge (c) Neat Corporation / Joachim Holmer - http://www.acegikmo.com/shaderforge/
// Note: Manually altering this data may prevent you from opening it in Shader Forge
/*SF_DATA;ver:1.38;sub:START;pass:START;ps:flbk:,iptp:0,cusa:False,bamd:0,cgin:,lico:1,lgpr:1,limd:0,spmd:1,trmd:0,grmd:0,uamb:True,mssp:True,bkdf:False,hqlp:False,rprd:False,enco:False,rmgx:True,imps:True,rpth:0,vtps:0,hqsc:True,nrmq:1,nrsp:0,vomd:0,spxs:False,tesm:0,olmd:1,culm:0,bsrc:0,bdst:1,dpts:2,wrdp:False,dith:0,atcv:False,rfrpo:True,rfrpn:Refraction,coma:15,ufog:False,aust:True,igpj:True,qofs:0,qpre:3,rntp:2,fgom:False,fgoc:False,fgod:False,fgor:False,fgmd:0,fgcr:0.5,fgcg:0.5,fgcb:0.5,fgca:1,fgde:0.01,fgrn:0,fgrf:300,stcl:False,atwp:False,stva:128,stmr:255,stmw:255,stcp:6,stps:0,stfa:0,stfz:0,ofsf:0,ofsu:0,f2p0:False,fnsp:False,fnfb:False,fsmp:False;n:type:ShaderForge.SFN_Final,id:3138,x:32719,y:32712,varname:node_3138,prsc:2|emission-6714-RGB,alpha-2516-OUT;n:type:ShaderForge.SFN_Color,id:6714,x:32259,y:32746,ptovrint:False,ptlb:mainColor,ptin:_mainColor,varname:node_6714,prsc:2,glob:False,taghide:False,taghdr:False,tagprd:False,tagnsco:False,tagnrm:False,c1:1,c2:1,c3:1,c4:1;n:type:ShaderForge.SFN_Tex2d,id:6588,x:32047,y:32928,ptovrint:False,ptlb:alpha1,ptin:_alpha1,varname:node_6588,prsc:2,glob:False,taghide:False,taghdr:False,tagprd:False,tagnsco:False,tagnrm:False,tex:6ef1000497a4a7541b852a15a6d8913c,ntxv:0,isnm:False|UVIN-6915-UVOUT;n:type:ShaderForge.SFN_Tex2d,id:8890,x:32033,y:33122,ptovrint:False,ptlb:alpha2,ptin:_alpha2,varname:node_8890,prsc:2,glob:False,taghide:False,taghdr:False,tagprd:False,tagnsco:False,tagnrm:False,tex:0f8c5295f1c51c14384e6f0d7112f7eb,ntxv:0,isnm:False|UVIN-6642-UVOUT;n:type:ShaderForge.SFN_Add,id:2516,x:32292,y:32994,varname:node_2516,prsc:2|A-6588-R,B-8890-R;n:type:ShaderForge.SFN_Slider,id:7193,x:31314,y:32760,ptovrint:False,ptlb:speed1,ptin:_speed1,varname:node_7193,prsc:2,glob:False,taghide:False,taghdr:False,tagprd:False,tagnsco:False,tagnrm:False,min:-2,cur:1,max:2;n:type:ShaderForge.SFN_TexCoord,id:7092,x:31645,y:32884,varname:node_7092,prsc:2,uv:0,uaff:False;n:type:ShaderForge.SFN_Time,id:6702,x:31302,y:33055,varname:node_6702,prsc:2;n:type:ShaderForge.SFN_Panner,id:6915,x:31853,y:32884,varname:node_6915,prsc:2,spu:1,spv:1|UVIN-7092-UVOUT,DIST-9207-OUT;n:type:ShaderForge.SFN_Multiply,id:9207,x:31483,y:32917,varname:node_9207,prsc:2|A-6702-T,B-7193-OUT;n:type:ShaderForge.SFN_Panner,id:6642,x:31826,y:33106,varname:node_6642,prsc:2,spu:1,spv:1|UVIN-7092-UVOUT,DIST-3126-OUT;n:type:ShaderForge.SFN_Slider,id:5083,x:31286,y:33280,ptovrint:False,ptlb:speed2,ptin:_speed2,varname:_speed2,prsc:2,glob:False,taghide:False,taghdr:False,tagprd:False,tagnsco:False,tagnrm:False,min:-2,cur:1,max:2;n:type:ShaderForge.SFN_Multiply,id:3126,x:31627,y:33217,varname:node_3126,prsc:2|A-6702-T,B-5083-OUT;proporder:6714-6588-8890-7193-5083;pass:END;sub:END;*/

Shader "Shader Forge/alphaAdd" {
    Properties {
        _mainColor ("mainColor", Color) = (1,1,1,1)
        _alpha1 ("alpha1", 2D) = "white" {}
        _alpha2 ("alpha2", 2D) = "white" {}
        _speed1 ("speed1", Range(-2, 2)) = 1
        _speed2 ("speed2", Range(-2, 2)) = 1
        [HideInInspector]_Cutoff ("Alpha cutoff", Range(0,1)) = 0.5
    }
    SubShader {
        Tags {
            "IgnoreProjector"="True"
            "Queue"="Transparent"
            "RenderType"="Transparent"
        }
        Pass {
            Name "FORWARD"
            Tags {
                "LightMode"="ForwardBase"
            }
            ZWrite Off
            
            CGPROGRAM
            #pragma vertex vert
            #pragma fragment frag
            #define UNITY_PASS_FORWARDBASE
            #include "UnityCG.cginc"
            #pragma multi_compile_fwdbase
            #pragma only_renderers d3d9 d3d11 glcore gles 
            #pragma target 3.0
            uniform float4 _mainColor;
            uniform sampler2D _alpha1; uniform float4 _alpha1_ST;
            uniform sampler2D _alpha2; uniform float4 _alpha2_ST;
            uniform float _speed1;
            uniform float _speed2;
            struct VertexInput {
                float4 vertex : POSITION;
                float2 texcoord0 : TEXCOORD0;
            };
            struct VertexOutput {
                float4 pos : SV_POSITION;
                float2 uv0 : TEXCOORD0;
            };
            VertexOutput vert (VertexInput v) {
                VertexOutput o = (VertexOutput)0;
                o.uv0 = v.texcoord0;
                o.pos = UnityObjectToClipPos( v.vertex );
                return o;
            }
            float4 frag(VertexOutput i) : COLOR {
////// Lighting:
////// Emissive:
                float3 emissive = _mainColor.rgb;
                float3 finalColor = emissive;
                float4 node_6702 = _Time;
                float2 node_6915 = (i.uv0+(node_6702.g*_speed1)*float2(1,1));
                float4 _alpha1_var = tex2D(_alpha1,TRANSFORM_TEX(node_6915, _alpha1));
                float2 node_6642 = (i.uv0+(node_6702.g*_speed2)*float2(1,1));
                float4 _alpha2_var = tex2D(_alpha2,TRANSFORM_TEX(node_6642, _alpha2));
                return fixed4(finalColor,(_alpha1_var.r+_alpha2_var.r));
            }
            ENDCG
        }
    }
    FallBack "Diffuse"
    CustomEditor "ShaderForgeMaterialInspector"
}
