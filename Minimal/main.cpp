/************************************************************************************

Authors     :   Bradley Austin Davis <bdavis@saintandreas.org>
Copyright   :   Copyright Brad Davis. All Rights reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

************************************************************************************/


#include <iostream>
#include <memory>
#include <exception>
#include <algorithm>

#include <Windows.h>

#define __STDC_FORMAT_MACROS 1

#define FAIL(X) throw std::runtime_error(X)

///////////////////////////////////////////////////////////////////////////////
//
// GLM is a C++ math library meant to mirror the syntax of GLSL 
//

#include <glm/glm.hpp>
#include <glm/gtc/noise.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

// Import the most commonly used types into the default namespace
using glm::ivec3;
using glm::ivec2;
using glm::uvec2;
using glm::mat3;
using glm::mat4;
using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::quat;

///////////////////////////////////////////////////////////////////////////////
//
// GLEW gives cross platform access to OpenGL 3.x+ functionality.  
//

#include <GL/glew.h>

bool checkFramebufferStatus(GLenum target = GL_FRAMEBUFFER) {
	GLuint status = glCheckFramebufferStatus(target);
	switch (status) {
	case GL_FRAMEBUFFER_COMPLETE:
		return true;
		break;

	case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
		std::cerr << "framebuffer incomplete attachment" << std::endl;
		break;

	case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
		std::cerr << "framebuffer missing attachment" << std::endl;
		break;

	case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
		std::cerr << "framebuffer incomplete draw buffer" << std::endl;
		break;

	case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
		std::cerr << "framebuffer incomplete read buffer" << std::endl;
		break;

	case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
		std::cerr << "framebuffer incomplete multisample" << std::endl;
		break;

	case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
		std::cerr << "framebuffer incomplete layer targets" << std::endl;
		break;

	case GL_FRAMEBUFFER_UNSUPPORTED:
		std::cerr << "framebuffer unsupported internal format or image" << std::endl;
		break;

	default:
		std::cerr << "other framebuffer error" << std::endl;
		break;
	}

	return false;
}

bool checkGlError() {
	GLenum error = glGetError();
	if (!error) {
		return false;
	}
	else {
		switch (error) {
		case GL_INVALID_ENUM:
			std::cerr << ": An unacceptable value is specified for an enumerated argument.The offending command is ignored and has no other side effect than to set the error flag.";
			break;
		case GL_INVALID_VALUE:
			std::cerr << ": A numeric argument is out of range.The offending command is ignored and has no other side effect than to set the error flag";
			break;
		case GL_INVALID_OPERATION:
			std::cerr << ": The specified operation is not allowed in the current state.The offending command is ignored and has no other side effect than to set the error flag..";
			break;
		case GL_INVALID_FRAMEBUFFER_OPERATION:
			std::cerr << ": The framebuffer object is not complete.The offending command is ignored and has no other side effect than to set the error flag.";
			break;
		case GL_OUT_OF_MEMORY:
			std::cerr << ": There is not enough memory left to execute the command.The state of the GL is undefined, except for the state of the error flags, after this error is recorded.";
			break;
		case GL_STACK_UNDERFLOW:
			std::cerr << ": An attempt has been made to perform an operation that would cause an internal stack to underflow.";
			break;
		case GL_STACK_OVERFLOW:
			std::cerr << ": An attempt has been made to perform an operation that would cause an internal stack to overflow.";
			break;
		}
		return true;
	}
}

void glDebugCallbackHandler(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *msg, GLvoid* data) {
	OutputDebugStringA(msg);
	std::cout << "debug call: " << msg << std::endl;
}

//////////////////////////////////////////////////////////////////////
//
// GLFW provides cross platform window creation
//

#include <GLFW/glfw3.h>

namespace glfw {
	inline GLFWwindow * createWindow(const uvec2 & size, const ivec2 & position = ivec2(INT_MIN)) {
		GLFWwindow * window = glfwCreateWindow(size.x, size.y, "glfw", nullptr, nullptr);
		if (!window) {
			FAIL("Unable to create rendering window");
		}
		if ((position.x > INT_MIN) && (position.y > INT_MIN)) {
			glfwSetWindowPos(window, position.x, position.y);
		}
		return window;
	}
}

// A class to encapsulate using GLFW to handle input and render a scene
class GlfwApp {

protected:
	uvec2 windowSize;
	ivec2 windowPosition;
	GLFWwindow * window{ nullptr };
	unsigned int frame{ 0 };

public:
	GlfwApp() {
		// Initialize the GLFW system for creating and positioning windows
		if (!glfwInit()) {
			FAIL("Failed to initialize GLFW");
		}
		glfwSetErrorCallback(ErrorCallback);
	}

	virtual ~GlfwApp() {
		if (nullptr != window) {
			glfwDestroyWindow(window);
		}
		glfwTerminate();
	}

	virtual int run() {
		preCreate();

		window = createRenderingTarget(windowSize, windowPosition);

		if (!window) {
			std::cout << "Unable to create OpenGL window" << std::endl;
			return -1;
		}

		postCreate();

		initGl();

		while (!glfwWindowShouldClose(window)) {
			++frame;
			glfwPollEvents();
			update();
			draw();
			finishFrame();
		}

		shutdownGl();

		return 0;
	}


protected:
	virtual GLFWwindow * createRenderingTarget(uvec2 & size, ivec2 & pos) = 0;

	virtual void draw() = 0;

	void preCreate() {
		glfwWindowHint(GLFW_DEPTH_BITS, 16);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
	}


	void postCreate() {
		glfwSetWindowUserPointer(window, this);
		glfwSetKeyCallback(window, KeyCallback);
		glfwSetMouseButtonCallback(window, MouseButtonCallback);
		glfwMakeContextCurrent(window);

		// Initialize the OpenGL bindings
		// For some reason we have to set this experminetal flag to properly
		// init GLEW if we use a core context.
		glewExperimental = GL_TRUE;
		if (0 != glewInit()) {
			FAIL("Failed to initialize GLEW");
		}
		glGetError();

		if (GLEW_KHR_debug) {
			GLint v;
			glGetIntegerv(GL_CONTEXT_FLAGS, &v);
			if (v & GL_CONTEXT_FLAG_DEBUG_BIT) {
				//glDebugMessageCallback(glDebugCallbackHandler, this);
			}
		}
	}

	virtual void initGl() {
	}

	virtual void shutdownGl() {
	}

	virtual void finishFrame() {
		glfwSwapBuffers(window);
	}

	virtual void destroyWindow() {
		glfwSetKeyCallback(window, nullptr);
		glfwSetMouseButtonCallback(window, nullptr);
		glfwDestroyWindow(window);
	}

	virtual void onKey(int key, int scancode, int action, int mods) {
		if (GLFW_PRESS != action) {
			return;
		}

		switch (key) {
		case GLFW_KEY_ESCAPE:
			glfwSetWindowShouldClose(window, 1);
			return;
		}
	}

	virtual void update() {}

	virtual void onMouseButton(int button, int action, int mods) {}

protected:
	virtual void viewport(const ivec2 & pos, const uvec2 & size) {
		glViewport(pos.x, pos.y, size.x, size.y);
	}

private:

	static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
		GlfwApp * instance = (GlfwApp *)glfwGetWindowUserPointer(window);
		instance->onKey(key, scancode, action, mods);
	}

	static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
		GlfwApp * instance = (GlfwApp *)glfwGetWindowUserPointer(window);
		instance->onMouseButton(button, action, mods);
	}

	static void ErrorCallback(int error, const char* description) {
		FAIL(description);
	}
};

//////////////////////////////////////////////////////////////////////
//
// The Oculus VR C API provides access to information about the HMD
//

#include <OVR_CAPI.h>
#include <OVR_CAPI_GL.h>

namespace ovr {

	// Convenience method for looping over each eye with a lambda
	template <typename Function>
	inline void for_each_eye(Function function) {
		for (ovrEyeType eye = ovrEyeType::ovrEye_Left;
			eye < ovrEyeType::ovrEye_Count;
			eye = static_cast<ovrEyeType>(eye + 1)) {
			function(eye);
		}
	}

	inline mat4 toGlm(const ovrMatrix4f & om) {
		return glm::transpose(glm::make_mat4(&om.M[0][0]));
	}

	inline mat4 toGlm(const ovrFovPort & fovport, float nearPlane = 0.01f, float farPlane = 10000.0f) {
		return toGlm(ovrMatrix4f_Projection(fovport, nearPlane, farPlane, true));
	}

	inline vec3 toGlm(const ovrVector3f & ov) {
		return glm::make_vec3(&ov.x);
	}

	inline vec2 toGlm(const ovrVector2f & ov) {
		return glm::make_vec2(&ov.x);
	}

	inline uvec2 toGlm(const ovrSizei & ov) {
		return uvec2(ov.w, ov.h);
	}

	inline quat toGlm(const ovrQuatf & oq) {
		return glm::make_quat(&oq.x);
	}

	inline mat4 toGlm(const ovrPosef & op) {
		mat4 orientation = glm::mat4_cast(toGlm(op.Orientation));
		mat4 translation = glm::translate(mat4(), ovr::toGlm(op.Position));
		return translation * orientation;
	}

	inline ovrMatrix4f fromGlm(const mat4 & m) {
		ovrMatrix4f result;
		mat4 transposed(glm::transpose(m));
		memcpy(result.M, &(transposed[0][0]), sizeof(float) * 16);
		return result;
	}

	inline ovrVector3f fromGlm(const vec3 & v) {
		ovrVector3f result;
		result.x = v.x;
		result.y = v.y;
		result.z = v.z;
		return result;
	}

	inline ovrVector2f fromGlm(const vec2 & v) {
		ovrVector2f result;
		result.x = v.x;
		result.y = v.y;
		return result;
	}

	inline ovrSizei fromGlm(const uvec2 & v) {
		ovrSizei result;
		result.w = v.x;
		result.h = v.y;
		return result;
	}

	inline ovrQuatf fromGlm(const quat & q) {
		ovrQuatf result;
		result.x = q.x;
		result.y = q.y;
		result.z = q.z;
		result.w = q.w;
		return result;
	}
}

class RiftManagerApp {
protected:
	ovrSession _session;
	ovrHmdDesc _hmdDesc;
	ovrGraphicsLuid _luid;

public:
	RiftManagerApp() {
		if (!OVR_SUCCESS(ovr_Create(&_session, &_luid))) {
			FAIL("Unable to create HMD session");
		}

		_hmdDesc = ovr_GetHmdDesc(_session);
	}

	~RiftManagerApp() {
		ovr_Destroy(_session);
		_session = nullptr;
	}
};

class RiftApp : public GlfwApp, public RiftManagerApp {
public:

private:
	GLuint _fbo{ 0 };
	GLuint _depthBuffer{ 0 };
	ovrTextureSwapChain _eyeTexture;

	GLuint _mirrorFbo{ 0 };
	ovrMirrorTexture _mirrorTexture;

	ovrEyeRenderDesc _eyeRenderDescs[2];

	mat4 _eyeProjections[2];

	ovrLayerEyeFov _sceneLayer;
	ovrViewScaleDesc _viewScaleDesc;

	uvec2 _renderTargetSize;
	uvec2 _mirrorSize;

public:

	RiftApp() {
		using namespace ovr;
		_viewScaleDesc.HmdSpaceToWorldScaleInMeters = 1.0f;

		memset(&_sceneLayer, 0, sizeof(ovrLayerEyeFov));
		_sceneLayer.Header.Type = ovrLayerType_EyeFov;
		_sceneLayer.Header.Flags = ovrLayerFlag_TextureOriginAtBottomLeft;

		ovr::for_each_eye([&](ovrEyeType eye) {
			ovrEyeRenderDesc& erd = _eyeRenderDescs[eye] = ovr_GetRenderDesc(_session, eye, _hmdDesc.DefaultEyeFov[eye]);
			ovrMatrix4f ovrPerspectiveProjection =
				ovrMatrix4f_Projection(erd.Fov, 0.01f, 1000.0f, ovrProjection_ClipRangeOpenGL);
			_eyeProjections[eye] = ovr::toGlm(ovrPerspectiveProjection);
			_viewScaleDesc.HmdToEyeOffset[eye] = erd.HmdToEyeOffset;

			ovrFovPort & fov = _sceneLayer.Fov[eye] = _eyeRenderDescs[eye].Fov;
			auto eyeSize = ovr_GetFovTextureSize(_session, eye, fov, 1.0f);
			_sceneLayer.Viewport[eye].Size = eyeSize;
			_sceneLayer.Viewport[eye].Pos = { (int)_renderTargetSize.x, 0 };

			_renderTargetSize.y = std::max(_renderTargetSize.y, (uint32_t)eyeSize.h);
			_renderTargetSize.x += eyeSize.w;
		});
		// Make the on screen window 1/4 the resolution of the render target
		_mirrorSize = _renderTargetSize;
		_mirrorSize /= 4;
	}

protected:
	GLFWwindow * createRenderingTarget(uvec2 & outSize, ivec2 & outPosition) override {
		return glfw::createWindow(_mirrorSize);
	}

	void initGl() override {
		GlfwApp::initGl();

		// Disable the v-sync for buffer swap
		glfwSwapInterval(0);

		ovrTextureSwapChainDesc desc = {};
		desc.Type = ovrTexture_2D;
		desc.ArraySize = 1;
		desc.Width = _renderTargetSize.x;
		desc.Height = _renderTargetSize.y;
		desc.MipLevels = 1;
		desc.Format = OVR_FORMAT_R8G8B8A8_UNORM_SRGB;
		desc.SampleCount = 1;
		desc.StaticImage = ovrFalse;
		ovrResult result = ovr_CreateTextureSwapChainGL(_session, &desc, &_eyeTexture);
		_sceneLayer.ColorTexture[0] = _eyeTexture;
		if (!OVR_SUCCESS(result)) {
			FAIL("Failed to create swap textures");
		}

		int length = 0;
		result = ovr_GetTextureSwapChainLength(_session, _eyeTexture, &length);
		if (!OVR_SUCCESS(result) || !length) {
			FAIL("Unable to count swap chain textures");
		}
		for (int i = 0; i < length; ++i) {
			GLuint chainTexId;
			ovr_GetTextureSwapChainBufferGL(_session, _eyeTexture, i, &chainTexId);
			glBindTexture(GL_TEXTURE_2D, chainTexId);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		}
		glBindTexture(GL_TEXTURE_2D, 0);

		// Set up the framebuffer object
		glGenFramebuffers(1, &_fbo);
		glGenRenderbuffers(1, &_depthBuffer);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _fbo);
		glBindRenderbuffer(GL_RENDERBUFFER, _depthBuffer);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, _renderTargetSize.x, _renderTargetSize.y);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
		glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _depthBuffer);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

		ovrMirrorTextureDesc mirrorDesc;
		memset(&mirrorDesc, 0, sizeof(mirrorDesc));
		mirrorDesc.Format = OVR_FORMAT_R8G8B8A8_UNORM_SRGB;
		mirrorDesc.Width = _mirrorSize.x;
		mirrorDesc.Height = _mirrorSize.y;
		if (!OVR_SUCCESS(ovr_CreateMirrorTextureGL(_session, &mirrorDesc, &_mirrorTexture))) {
			FAIL("Could not create mirror texture");
		}
		glGenFramebuffers(1, &_mirrorFbo);
	}

	void onKey(int key, int scancode, int action, int mods) override {
		if (GLFW_PRESS == action) switch (key) {
		case GLFW_KEY_R:
			ovr_RecenterTrackingOrigin(_session);
			return;
		}

		GlfwApp::onKey(key, scancode, action, mods);
	}

	void draw() final override {
		ovrPosef eyePoses[2];
		ovr_GetEyePoses(_session, frame, true, _viewScaleDesc.HmdToEyeOffset, eyePoses, &_sceneLayer.SensorSampleTime);

		int curIndex;
		ovr_GetTextureSwapChainCurrentIndex(_session, _eyeTexture, &curIndex);
		GLuint curTexId;
		ovr_GetTextureSwapChainBufferGL(_session, _eyeTexture, curIndex, &curTexId);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _fbo);
		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, curTexId, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		ovr::for_each_eye([&](ovrEyeType eye) {
			const auto& vp = _sceneLayer.Viewport[eye];
			glViewport(vp.Pos.x, vp.Pos.y, vp.Size.w, vp.Size.h);
			_sceneLayer.RenderPose[eye] = eyePoses[eye];
			renderScene(_eyeProjections[eye], ovr::toGlm(eyePoses[eye]));
		});
		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 0, 0);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		ovr_CommitTextureSwapChain(_session, _eyeTexture);
		ovrLayerHeader* headerList = &_sceneLayer.Header;
		ovr_SubmitFrame(_session, frame, &_viewScaleDesc, &headerList, 1);

		GLuint mirrorTextureId;
		ovr_GetMirrorTextureBufferGL(_session, _mirrorTexture, &mirrorTextureId);
		glBindFramebuffer(GL_READ_FRAMEBUFFER, _mirrorFbo);
		glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mirrorTextureId, 0);
		glBlitFramebuffer(0, 0, _mirrorSize.x, _mirrorSize.y, 0, _mirrorSize.y, _mirrorSize.x, 0, GL_COLOR_BUFFER_BIT, GL_NEAREST);
		glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
	}

	virtual void renderScene(const glm::mat4 & projection, const glm::mat4 & headPose) = 0;
};

//////////////////////////////////////////////////////////////////////
//
// The remainder of this code is specific to the scene we want to 
// render.  I use oglplus to render an array of cubes, but your 
// application would perform whatever rendering you want
//

#include <time.h>
#include <random>
#include "Model.h"
#include "Group.h"
#include "MatrixTransform.h"
#include "Line.h"
struct SimScene {
	bool isPlaying = true;
	bool l_pressed;
	bool r_pressed;
	Line * l_line;
	Line * r_line;
	MatrixTransform * l_line_mt;
	MatrixTransform * r_line_mt;
	Model * factory;
	MatrixTransform * factory_mt;
	Group * co2Group;
	Group * o2Group;
	Model * co2;
	Model * o2;
	GLint shaderProgram;
	time_t last_co2_time;
	std::default_random_engine generator;

#define VERTEX_SHADER2_PATH "C:/Users/tiyang/Desktop/CSE190Proj1VR/Minimal/shader2.vert"
#define FRAGMENT_SHADER2_PATH "C:/Users/tiyang/Desktop/CSE190Proj1VR/Minimal/shader2.frag"

public:
	bool leftHandTriggerPressed;
	bool rightHandTriggerPressed;
	glm::mat4 left_transf;
	glm::mat4 right_transf;

	static glm::mat4 P; // P for projection
	static glm::mat4 V; // V for view

	SimScene() {
		shaderProgram = LoadShaders(VERTEX_SHADER2_PATH, FRAGMENT_SHADER2_PATH);

		factory = new Model("C:/Users/tiyang/Desktop/CSE190Proj1VR/Minimal/assets/factory1/factory1.obj");
		co2 = new Model("C:/Users/tiyang/Desktop/CSE190Proj1VR/Minimal/assets/co2/co2.obj");
		o2 = new Model("C:/Users/tiyang/Desktop/CSE190Proj1VR/Minimal/assets/o2/o2.obj");
		l_line = new Line();
		r_line = new Line();
		l_line_mt = new MatrixTransform(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f)));
		r_line_mt = new MatrixTransform(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f)));
		l_line_mt->addChild(l_line);
		r_line_mt->addChild(r_line);

		factory_mt = new MatrixTransform(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -10.0f, -15.0f)));
		factory_mt->addChild(factory);

		co2Group = new Group();
		o2Group = new Group();
		for (int i = 0; i < 5; i++) {
			create_co2(true);
		}
		last_co2_time = time(0);
	}

	bool update() {
		bool hit = false;
		if (isPlaying) {
			auto it = (co2Group->children).begin();
			while (it != co2Group->children.end()) {
				bool left_collide = check(l_line, left_transf, it);
				bool right_collide = check(r_line, right_transf, it);
				if (leftHandTriggerPressed && rightHandTriggerPressed && left_collide && right_collide) {
					MatrixTransform *tmp = new MatrixTransform(glm::mat4(1.0));
					tmp->M = (dynamic_cast<MatrixTransform*> (*it))->M;
					tmp->deg = (dynamic_cast<MatrixTransform*> (*it))->deg;
					tmp->axis = (dynamic_cast<MatrixTransform*> (*it))->axis;
					tmp->move = (dynamic_cast<MatrixTransform*> (*it))->move;
					tmp->pos = (dynamic_cast<MatrixTransform*> (*it))->pos;
					tmp->addChild(o2);
					o2Group->addChild(tmp);
					co2Group->children.erase(it++);
					hit = true;
				}
				else ++it;
			}
			if (co2Group->children.empty()) {
				glClearColor(0.0f, 191.0f / 255.f, 1.0f, 1.0f);
				isPlaying = false;
			}
			else if (co2Group->children.size() > 10) {
				for (int i = 0; i < 100; i++) {
					create_co2(true);
				}
				isPlaying = false;
			}
		}
		co2Group->update();
		o2Group->update();

		return hit;
	}

	bool check(Line* line, glm::mat4 transf, list<Node*>::iterator it) {
		vec4 v1(0.0f, 0.0f, 0.0f, 1.0f);
		vec4 v2(0.0f, 0.0f, -100.0f, 1.0f);
		vec4 tmp1 = transf*v1;
		vec4 tmp2 = transf*v2;
		vec3 x1(tmp1.x, tmp1.y, tmp1.z);
		vec3 x2(tmp2.x, tmp2.y, tmp2.z);
		vec3 x0 = (dynamic_cast<MatrixTransform*> (*it)) -> pos;
		float d = glm::length(glm::cross((x2 - x1), (x1 - x0))) / glm::length(x2 - x1);
		if (d < 1) return true;
		else return false;
	}

	void reset() {
		if (isPlaying) return;
		co2Group->children.clear();
		o2Group->children.clear();
		glClearColor(0.0f, 0.0f, 128.0f / 255.0f, 1.0f);

		isPlaying = true;

		for (int i = 0; i < 5; i++) {
			create_co2(true);
		}
		last_co2_time = time(0);
	}

	void render(const mat4 & projection, const mat4 & modelview) {
		// Use the shader of programID
		glUseProgram(shaderProgram);

		time_t cur_time = time(0);
		double seconds = difftime(cur_time, last_co2_time);
		if (seconds >= 1.4 && isPlaying) {
			last_co2_time = cur_time;
			create_co2(false);
		}

		// light
		glm::vec3 pointLightPosition;
		pointLightPosition = glm::vec3(10.0f, 10.0f, 5.0f);
		glUniform3f(glGetUniformLocation(shaderProgram, "pointLight[0].position"), pointLightPosition.x, pointLightPosition.y, pointLightPosition.z);
		glUniform3f(glGetUniformLocation(shaderProgram, "pointLight[0].ambient"), 1.0f, 1.0f, 1.0f);
		glUniform3f(glGetUniformLocation(shaderProgram, "pointLight[0].diffuse"), 1.0f, 1.0f, 1.0f);
		glUniform3f(glGetUniformLocation(shaderProgram, "pointLight[0].specular"), 1.0f, 1.0f, 1.0f);
		glUniform1f(glGetUniformLocation(shaderProgram, "pointLight[0].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(shaderProgram, "pointLight[0].linear"), 0.09f); // 0.09
		glUniform1f(glGetUniformLocation(shaderProgram, "pointLight[0].quadratic"), 0.032f); // 0.032

		
		pointLightPosition = glm::vec3(10.0f, 10.0f, -20.0f);
		glUniform3f(glGetUniformLocation(shaderProgram, "pointLight[1].position"), pointLightPosition.x, pointLightPosition.y, pointLightPosition.z);
		glUniform3f(glGetUniformLocation(shaderProgram, "pointLight[1].ambient"), 1.0f, 1.0f, 1.0f);
		glUniform3f(glGetUniformLocation(shaderProgram, "pointLight[1].diffuse"), 1.0f, 1.0f, 1.0f);
		glUniform3f(glGetUniformLocation(shaderProgram, "pointLight[1].specular"), 1.0f, 1.0f, 1.0f);
		glUniform1f(glGetUniformLocation(shaderProgram, "pointLight[1].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(shaderProgram, "pointLight[1].linear"), 0.09f); // 0.09
		glUniform1f(glGetUniformLocation(shaderProgram, "pointLight[1].quadratic"), 0.032f); // 0.032

		pointLightPosition = glm::vec3(-10.0f, 10.0f, 5.0f);
		glUniform3f(glGetUniformLocation(shaderProgram, "pointLight[2].position"), pointLightPosition.x, pointLightPosition.y, pointLightPosition.z);
		glUniform3f(glGetUniformLocation(shaderProgram, "pointLight[2].ambient"), 1.0f, 1.0f, 1.0f);
		glUniform3f(glGetUniformLocation(shaderProgram, "pointLight[2].diffuse"), 1.0f, 1.0f, 1.0f);
		glUniform3f(glGetUniformLocation(shaderProgram, "pointLight[2].specular"), 1.0f, 1.0f, 1.0f);
		glUniform1f(glGetUniformLocation(shaderProgram, "pointLight[2].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(shaderProgram, "pointLight[2].linear"), 0.09f); // 0.09
		glUniform1f(glGetUniformLocation(shaderProgram, "pointLight[2].quadratic"), 0.032f); // 0.032

		pointLightPosition = glm::vec3(-10.0f, 10.0f, -20.0f);
		glUniform3f(glGetUniformLocation(shaderProgram, "pointLight[3].position"), pointLightPosition.x, pointLightPosition.y, pointLightPosition.z);
		glUniform3f(glGetUniformLocation(shaderProgram, "pointLight[3].ambient"), 1.0f, 1.0f, 1.0f);
		glUniform3f(glGetUniformLocation(shaderProgram, "pointLight[3].diffuse"), 1.0f, 1.0f, 1.0f);
		glUniform3f(glGetUniformLocation(shaderProgram, "pointLight[3].specular"), 1.0f, 1.0f, 1.0f);
		glUniform1f(glGetUniformLocation(shaderProgram, "pointLight[3].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(shaderProgram, "pointLight[3].linear"), 0.09f); // 0.09
		glUniform1f(glGetUniformLocation(shaderProgram, "pointLight[3].quadratic"), 0.032f); // 0.032
		

		factory_mt->draw(glm::mat4(1.0f), shaderProgram, projection, modelview);
		co2Group->draw(glm::mat4(1.0f), shaderProgram, projection, modelview);
		o2Group->draw(glm::mat4(1.0f), shaderProgram, projection, modelview);
		l_line->pressed = leftHandTriggerPressed;
		r_line->pressed = rightHandTriggerPressed;
		l_line_mt->draw(left_transf, shaderProgram, projection, modelview);
		r_line_mt->draw(right_transf, shaderProgram, projection, modelview);
	}

private:
	void create_co2(bool first_create) {
		std::uniform_real_distribution<float> plus_minus_one_dist(-1.0, 1.0);
		std::uniform_real_distribution<float> plus_one_dist(0.0, 1.0);
		MatrixTransform* mt;
		if( first_create ) mt = new MatrixTransform(glm::translate(glm::mat4(1.0f), glm::vec3(plus_minus_one_dist(generator) * 9.0f, plus_minus_one_dist(generator) * 9.0f, plus_one_dist(generator) * -19.0f)));
		else mt = new MatrixTransform(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -9.0f, -15.0f) ) );
		mt->deg = plus_minus_one_dist(generator);
		mt->axis = glm::vec3(plus_minus_one_dist(generator), plus_minus_one_dist(generator), plus_minus_one_dist(generator));
		mt->move = glm::vec3(plus_minus_one_dist(generator) / 50.0f, plus_one_dist(generator) / 50.0f, plus_minus_one_dist(generator) / 50.0f); // upwards
		mt->scale(0.4f);
		mt->addChild(co2);
		co2Group->addChild(mt);
	}
};

class SimApp : public RiftApp {
	std::shared_ptr<SimScene> simScene;

public:
	SimApp() {}
protected:
	bool leftHandTriggerPressed = false;
	bool rightHandTriggerPressed = false;
	glm::mat4 left_transf;
	glm::mat4 right_transf;

	void initGl() override {
		RiftApp::initGl();
		// Enable depth buffering
		glEnable(GL_DEPTH_TEST);
		// Related to shaders and z value comparisons for the depth buffer
		glDepthFunc(GL_LEQUAL);
		// Set polygon drawing mode to fill front and back of each polygon
		// You can also use the paramter of GL_LINE instead of GL_FILL to see wireframes
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		// Disable backface culling to render both sides of polygons
		glDisable(GL_CULL_FACE);
		// Set clear color
		glClearColor(0.0f, 0.0f, 128.0f / 255.0f, 1.0f);
		ovr_RecenterTrackingOrigin(_session);
		simScene = std::shared_ptr<SimScene>(new SimScene());
	}

	void shutdownGl() override {
		simScene.reset();
	}

	void update() override {
		ovr_SetControllerVibration(_session, ovrControllerType_LTouch, 1.0f, 0.0f);
		ovr_SetControllerVibration(_session, ovrControllerType_RTouch, 1.0f, 0.0f);
		bool hit = simScene->update();
		if (hit) {
			ovr_SetControllerVibration(_session, ovrControllerType_LTouch, 1.0f, 1.0f);
			ovr_SetControllerVibration(_session, ovrControllerType_RTouch, 1.0f, 1.0f);
		}

		double displayMidpointSeconds = ovr_GetPredictedDisplayTime(_session, frame);
		ovrTrackingState trackState = ovr_GetTrackingState(_session, displayMidpointSeconds, ovrTrue);

		// determine whether left hand trigger pressed
		ovrInputState inputState;

		if (OVR_SUCCESS(ovr_GetInputState(_session, ovrControllerType_Touch, &inputState))) {
			if (inputState.Buttons) {
				simScene->reset();
			}

			if (inputState.IndexTrigger[ovrHand_Left] > 0.5f) simScene->leftHandTriggerPressed = true;
			else simScene->leftHandTriggerPressed = false;

			if (inputState.IndexTrigger[ovrHand_Right] > 0.5f) simScene->rightHandTriggerPressed = true;
			else simScene->rightHandTriggerPressed = false;
		}
		ovrPosef leftPose = trackState.HandPoses[ovrHand_Left].ThePose;
		simScene->left_transf = ovr::toGlm(leftPose);

		ovrPosef rightPose = trackState.HandPoses[ovrHand_Right].ThePose;
		simScene->right_transf = ovr::toGlm(rightPose);
	}

	void renderScene(const glm::mat4 & projection, const glm::mat4 & headPose) override {
		simScene->render(projection, glm::inverse(headPose));
	}
};

// Execute our example class
int __stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	int result = -1;
	try {
		if (!OVR_SUCCESS(ovr_Initialize(nullptr))) {
			FAIL("Failed to initialize the Oculus SDK");
		}
		result = SimApp().run();
	}
	catch (std::exception & error) {
		OutputDebugStringA(error.what());
		std::cerr << error.what() << std::endl;
	}
	ovr_Shutdown();
	return result;
}