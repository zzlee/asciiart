using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System;
using System.Runtime.InteropServices;

public class ReadPixels32 : MonoBehaviour
{
    // Start is called before the first frame update
    void Start()
    {
        Texture2D chars = Resources.Load<Texture2D>("16chars");
        Debug.Log(chars.width + "," + chars.height);
        var pixels_array = chars.GetRawTextureData<Color32>();
        Color32[] pixels = pixels_array.ToArray();

        GCHandle handle = GCHandle.Alloc(pixels, GCHandleType.Pinned);
        IntPtr ptr = handle.AddrOfPinnedObject();
        Debug.Log("ptr = " + ptr);
        handle.Free();
    }

    // Update is called once per frame
    void Update()
    {
        
    }
}
