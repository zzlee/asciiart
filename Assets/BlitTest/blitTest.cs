using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class blitTest : MonoBehaviour {

    public RenderTexture myRT;
    public Material myMat;
    public Texture2D tex;
    void Start () {
        tex = new Texture2D(512, 512);
    }
	
	// Update is called once per frame
	void Update () {
        Graphics.Blit(null, myRT, myMat);

        tex.ReadPixels(new Rect(0, 0, 512, 512), 0, 0, false);
        tex.Apply();

    }
}
