package com.leilei.androidglm;

/**
 * Created by leilei on 2017/8/4.
 * GLNative Interface
 */

public class GLNative {

    public static native boolean init(int width, int height);

    public static native void render();
}
