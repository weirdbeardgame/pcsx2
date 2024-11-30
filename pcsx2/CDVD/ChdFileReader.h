// SPDX-FileCopyrightText: 2002-2024 PCSX2 Dev Team
// SPDX-License-Identifier: GPL-3.0+

#pragma once
#include "ThreadedFileReader.h"
#include <vector>
#include <map>

typedef struct _chd_file chd_file;

static std::map<std::string, u8> trackModes{
	{"MODE1_RAW", CDVD_MODE_2352},
	{"MODE1", CDVD_MODE1_TRACK},
	{"MODE2", CDVD_MODE2_TRACK},
	{"AUDIO", CDVD_AUDIO_TRACK}};

class ChdFileReader final : public ThreadedFileReader
{
	DeclareNoncopyableObject(ChdFileReader);

public:
	ChdFileReader();
	~ChdFileReader() override;

	bool Open2(std::string filename, Error* error) override;

	bool Precache2(ProgressCallback* progress, Error* error) override;

	std::vector<toc_entry> ReadTOC() override;

	Chunk ChunkForOffset(u64 offset) override;
	int ReadChunk(void* dst, s64 blockID) override;

	void Close2(void) override;
	uint GetBlockCount(void) const override;

private:
	bool ParseTOC(u64* out_frame_count, std::vector<toc_entry>& entries);

	chd_file* ChdFile = nullptr;
	u64 file_size = 0;
	u32 hunk_size = 0;
};
