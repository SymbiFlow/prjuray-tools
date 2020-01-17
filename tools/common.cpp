/*
 * Copyright (C) 2017-2020  The Project X-Ray Authors.
 * Copyright (C) 2020       The Project U-Ray Authors.
 *
 * Use of this source code is governed by a ISC-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/ISC
 *
 * SPDX-License-Identifier: ISC
 */

#include "common.h"


void ChipData::open(const std::string &root) {
	this->root = root;
	load_frames();
	load_tiles();
}

void ChipData::load_frames() {
	LineReader rd(root + "/frames.txt");
	for (const std::string &line : rd) {
		all_frames.insert(std::strtoul(line.c_str(), nullptr, 16));
	}
}

void ChipData::load_tiletype_database(int type) {
	auto &tt = tiletypes.at(type);
	if (tt.loaded_db)
		return;
	LineReader rd(root + "/" + tt.type + ".bits");
	std::vector<std::string> split;
	for (const std::string &line : rd) {
		split_str(line, split);
		if (split.size() < 1)
			continue;
		auto &feat = tt.features[split.at(0)];
		for (size_t i = 1; i < split.size(); i++)
			feat.push_back(std::stoi(split.at(i)));
	}
	tt.loaded_db = true;
}

void ChipData::load_tiles() {
	LineReader rd(root + "/tiles.txt");
	std::vector<std::string> split;
	TileInstance *curr = nullptr;

	for (const std::string &line : rd) {
		split_str(line, split);
		if (split.size() < 1)
			continue;
		if (split.at(0) == ".tile") {
			curr = &(tiles[split.at(1)]);
			curr->name = split.at(1);
			if (!tiletype_by_name.count(split.at(2))) {
				tiletype_by_name[split.at(2)] = int(tiletypes.size());
				curr->type = int(tiletypes.size());
				tiletypes.emplace_back();
				tiletypes.back().type = split.at(2);
			} else {
				curr->type = tiletype_by_name.at(split.at(2));
			}
			curr->x = std::stoi(split.at(3));
			curr->y = std::stoi(split.at(4));
		} else if (split.at(0) == "frame") {
			TileInstance::TileBitMapping tbm;
			tbm.frame_offset = std::strtoul(split.at(1).c_str(), nullptr, 16);
			tbm.bit_offset = std::stoi(split.at(3));
			tbm.size = std::stoi(split.at(5));
			curr->bits.push_back(tbm);
		}
	}
}


