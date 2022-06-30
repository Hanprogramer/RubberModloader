#pragma once
#ifndef RML_H
#define RML_H

#include "pch.h"
#include "mem.h"
#include "hook.h"
#include "RMLHooks.h"

namespace RML {
	DWORD WINAPI RubberModLoaderThread(HMODULE hModule);
	class RubberModLoader
	{
	};


	/// <summary>
	/// Create and start the RML thread
	/// </summary>
	/// <param name="hModule">The Dll caller's module</param>
	void StartRMLThreadWin32(HMODULE hModule);

	/// <summary>
	/// Initialize the RML engine
	/// </summary>
	bool Initialize(HMODULE hModule);

	/// <summary>
	/// Deinitialize the RML engine
	/// </summary>
	bool Dispose();
}

#endif