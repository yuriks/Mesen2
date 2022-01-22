#include "stdafx.h"
#include <algorithm>
#include "Shared/Video/DebugHud.h"
#include "Shared/Video/DrawCommand.h"
#include "Shared/Video/DrawLineCommand.h"
#include "Shared/Video/DrawPixelCommand.h"
#include "Shared/Video/DrawRectangleCommand.h"
#include "Shared/Video/DrawStringCommand.h"
#include "Shared/Video/DrawScreenBufferCommand.h"

DebugHud::DebugHud()
{
}

DebugHud::~DebugHud()
{
	_commandLock.Acquire();
	_commandLock.Release();
}

void DebugHud::ClearScreen()
{
	auto lock = _commandLock.AcquireSafe();
	_commands.clear();
}

void DebugHud::Draw(uint32_t* argbBuffer, FrameInfo frameInfo, OverscanDimensions overscan, uint32_t frameNumber, bool autoScale)
{
	auto lock = _commandLock.AcquireSafe();
	for(unique_ptr<DrawCommand> &command : _commands) {
		command->Draw(argbBuffer, frameInfo, overscan, frameNumber, autoScale);
	}
	_commands.erase(std::remove_if(_commands.begin(), _commands.end(), [](const unique_ptr<DrawCommand>& c) { return c->Expired(); }), _commands.end());
}

void DebugHud::DrawPixel(int x, int y, int color, int frameCount, int startFrame)
{
	auto lock = _commandLock.AcquireSafe();
	if(_commands.size() < DebugHud::MaxCommandCount) {
		_commands.push_back(unique_ptr<DrawPixelCommand>(new DrawPixelCommand(x, y, color, frameCount, startFrame)));
	}
}

void DebugHud::DrawLine(int x, int y, int x2, int y2, int color, int frameCount, int startFrame)
{
	auto lock = _commandLock.AcquireSafe();
	if(_commands.size() < DebugHud::MaxCommandCount) {
		_commands.push_back(unique_ptr<DrawLineCommand>(new DrawLineCommand(x, y, x2, y2, color, frameCount, startFrame)));
	}
}

void DebugHud::DrawRectangle(int x, int y, int width, int height, int color, bool fill, int frameCount, int startFrame)
{
	auto lock = _commandLock.AcquireSafe();
	if(_commands.size() < DebugHud::MaxCommandCount) {
		_commands.push_back(unique_ptr<DrawRectangleCommand>(new DrawRectangleCommand(x, y, width, height, color, fill, frameCount, startFrame)));
	}
}

void DebugHud::DrawScreenBuffer(uint32_t* screenBuffer, int startFrame)
{
	auto lock = _commandLock.AcquireSafe();
	if(_commands.size() < DebugHud::MaxCommandCount) {
		_commands.push_back(unique_ptr<DrawScreenBufferCommand>(new DrawScreenBufferCommand(screenBuffer, startFrame)));
	}
}

void DebugHud::DrawString(int x, int y, string text, int color, int backColor, int frameCount, int startFrame, int maxWidth)
{
	auto lock = _commandLock.AcquireSafe();
	if(_commands.size() < DebugHud::MaxCommandCount) {
		_commands.push_back(unique_ptr<DrawStringCommand>(new DrawStringCommand(x, y, text, color, backColor, frameCount, startFrame, maxWidth)));
	}
}
