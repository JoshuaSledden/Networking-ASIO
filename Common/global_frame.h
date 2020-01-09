#pragma once
#include <string>
#include "wx/wx.h"

#include "frame.h"

#define SET_GLOBAL_FRAME(a) global_frame::getInstance().SetFrame(a)
#define GLOBAL_FRAME_LOG(a) global_frame::getInstance().GetFrame()->log(a)

class global_frame
{
public:
	static global_frame& getInstance()
	{
		static global_frame    instance;
		return instance;
	}

	void SetFrame(frame* f) { frame_ = f; }
	frame* GetFrame() { return frame_; }

private:
	global_frame()
		: frame_(nullptr) {}

	frame* frame_;

public: 
	global_frame(global_frame const&) = delete;
	void operator=(global_frame const&) = delete;
};