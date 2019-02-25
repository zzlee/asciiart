using System.Collections;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using UnityEngine;
using UnityEngine.Rendering;
using System;

public class AsciiArt0 : MonoBehaviour
{
    public double noiseScale = 4;
    public double noiseSpeed = 0.8;
    public int perlinNoiseWidth = 64;
    public int perlinNoiseHeight = 64;
    public string charsResName = "16chars";
    public int charWidth = 8;

    Texture2D _perlinNoiseTexture;
    CommandBuffer _perlinNoiseCommand;
    Texture2D _digitTexture;
    CommandBuffer _digitCommand;
    int _gid;

    Texture2D _chars;
    Unity.Collections.NativeArray<Color32> _pixelsArray;
    Color32[] _charsPixels;
    GCHandle _charsHandle;

    [DllImport("AsciiArt")]
    static extern void AsciiArt_Init();

    [DllImport("AsciiArt")]
    static extern void AsciiArt_Uninit();

    [DllImport("AsciiArt")]
    static extern System.IntPtr AsciiArt_PerlinNoiseV2();

    [DllImport("AsciiArt")]
    static extern void AsciiArt_PerlinNoise_SetNoiseSeed(int gid, uint seed);

    [DllImport("AsciiArt")]
    static extern void AsciiArt_PerlinNoise_SetZ(int gid, double z);

    [DllImport("AsciiArt")]
    static extern void AsciiArt_PerlinNoise_SetScale(int gid, double scale);

    [DllImport("AsciiArt")]
    static extern System.IntPtr AsciiArt_DigitV2();

    [DllImport("AsciiArt")]
    static extern void AsciiArt_Digit_SetCharsImage(int gid, IntPtr ptr, int width, int height, int charWidth);

    void Start()
    {
        AsciiArt_Init();
        _gid = 0;

        AsciiArt_PerlinNoise_SetNoiseSeed(_gid, 123);

        _perlinNoiseCommand = new CommandBuffer();
        _perlinNoiseTexture = new Texture2D(perlinNoiseWidth, perlinNoiseHeight, TextureFormat.RGBA32, false);
        _perlinNoiseTexture.wrapMode = TextureWrapMode.Clamp;

        // initialize chars image
        Texture2D _chars = Resources.Load<Texture2D>(charsResName);
        _pixelsArray = _chars.GetRawTextureData<Color32>();
        _charsPixels = _pixelsArray.ToArray();
        Debug.Log(String.Format("_pixelsArray = {0} bytes", _pixelsArray.Length));

        _charsHandle = GCHandle.Alloc(_charsPixels, GCHandleType.Pinned);
        IntPtr ptr = _charsHandle.AddrOfPinnedObject();
        Debug.Log(String.Format("CharsImage: {0}x{1}, charWidth={2}", _chars.width, _chars.height, charWidth));
        AsciiArt_Digit_SetCharsImage(_gid, ptr, _chars.width, _chars.height, charWidth);

        _digitCommand = new CommandBuffer();
        _digitTexture = new Texture2D(perlinNoiseWidth * charWidth, perlinNoiseHeight * _chars.height, TextureFormat.RGBA32, false);
        _digitTexture.wrapMode = TextureWrapMode.Clamp;
        
        var prop = new MaterialPropertyBlock();

        prop.SetTexture("_MainTex", _perlinNoiseTexture);
        prop.SetTexture("_DigitTex", _digitTexture);

        GetComponent<Renderer>().SetPropertyBlock(prop);
    }

    void OnDestroy()
    {
        _perlinNoiseCommand.Dispose();
        Destroy(_perlinNoiseTexture);

        _digitCommand.Dispose();
        Destroy(_digitTexture);

        _charsHandle.Free();

        AsciiArt_Uninit();
    }

    // Update is called once per frame
    void Update()
    {
        AsciiArt_PerlinNoise_SetZ(_gid, Time.time * noiseSpeed);
        AsciiArt_PerlinNoise_SetScale(_gid, noiseScale);

        _perlinNoiseCommand.IssuePluginCustomTextureUpdateV2(AsciiArt_PerlinNoiseV2(), _perlinNoiseTexture, (uint)_gid);
        _digitCommand.IssuePluginCustomTextureUpdateV2(AsciiArt_DigitV2(), _digitTexture, (uint)_gid);

        Graphics.ExecuteCommandBuffer(_perlinNoiseCommand);
        Graphics.ExecuteCommandBuffer(_digitCommand);

        _perlinNoiseCommand.Clear();
        _digitCommand.Clear();

        //Rotation
        //transform.eulerAngles = new Vector3(10, 20, 30) * Time.time;
    }
}
