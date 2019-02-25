using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class webcamera_texture : MonoBehaviour {

	// Use this for initialization
	void Start () {
		WebCamTexture webcamTexture = new WebCamTexture();

        Renderer renderer = GetComponent<Renderer>();
        renderer.material.mainTexture = webcamTexture;
        webcamTexture.Play();
	}
	
	// Update is called once per frame
	void Update () {
		
	}
}
