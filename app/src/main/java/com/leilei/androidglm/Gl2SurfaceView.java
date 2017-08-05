package com.leilei.androidglm;

import android.content.Context;
import android.graphics.PixelFormat;
import android.opengl.GLSurfaceView;
import android.util.AttributeSet;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

/**
 * Created by leilei on 2017/8/4.
 */

public class Gl2SurfaceView extends GLSurfaceView {
    private Render mRender;

    public Gl2SurfaceView(Context context) {
        this(context, null);
    }

    public Gl2SurfaceView(Context context, AttributeSet attrs) {
        super(context, attrs);
        initView();
    }

    private void initView() {
        //Set Opengl Version

        setEGLContextClientVersion(2);
        setEGLConfigChooser(true);
        setEGLConfigChooser(8, 8, 8, 8, 24, 8);
        getHolder().setFormat(PixelFormat.TRANSPARENT);
        mRender = new Render();
        setRenderer(mRender);
        setRenderMode(GLSurfaceView.RENDERMODE_CONTINUOUSLY);

    }

    class Render implements Renderer {

        @Override
        public void onSurfaceCreated(GL10 gl, EGLConfig config) {

        }

        @Override
        public void onSurfaceChanged(GL10 gl, int width, int height) {
            GLNative.init(width, height);
        }

        @Override
        public void onDrawFrame(GL10 gl) {
            GLNative.render();
        }
    }
}
