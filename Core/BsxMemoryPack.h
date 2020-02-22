#pragma once
#include "stdafx.h"
#include "RamHandler.h"
#include "IMemoryHandler.h"
#include "../Utilities/ISerializable.h"

class BsxMemoryPackHandler;

class BsxMemoryPack : public ISerializable
{
	vector<uint8_t> _orgData;
	uint8_t* _data = nullptr;
	uint32_t _dataSize = 0;
	vector<unique_ptr<IMemoryHandler>> _handlers;

	uint8_t _calculatedSize = 0x0C;

	bool _enableCsr = false;
	bool _enableEsr = false;
	bool _enableVendorInfo = false;
	bool _writeByte = false;
	uint16_t _command = 0;

public:
	BsxMemoryPack(vector<uint8_t>& data);
	virtual ~BsxMemoryPack();

	void Serialize(Serializer& s) override;

	void ProcessCommand(uint8_t value, uint32_t page);
	void Reset();
	
	vector<unique_ptr<IMemoryHandler>>& GetMemoryHandlers();
	uint8_t* DebugGetMemoryPack();
	uint32_t DebugGetMemoryPackSize();
	
	class BsxMemoryPackHandler : public RamHandler
	{
		BsxMemoryPack* _memPack;
		uint32_t _page;

	public:
		BsxMemoryPackHandler(BsxMemoryPack* memPack, uint32_t offset);

		uint8_t Read(uint32_t addr) override;
		void Write(uint32_t addr, uint8_t value) override;
	};
};
