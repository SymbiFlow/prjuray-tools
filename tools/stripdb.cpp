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

#include <vector>
#include <iostream>
#include <map>
#include <string>
#include <fstream>
#include <stdexcept>
#include <iterator>
#include <stdarg.h>
#include <iomanip>
#include <map>
#include <set>
#include <dirent.h>


struct TileType {
	std::map<std::string, std::vector<int>> features;
};

std::map<std::string, TileType> tiletypes;


void parse_database(const std::string &name, std::istream &in) {
	std::string line;
	while (std::getline(in, line)) {
		auto cpos = line.find('#');
		if (cpos != std::string::npos)
			line = line.substr(0, cpos);
		if (line.empty())
			continue;
		std::istringstream iss(line);
		std::string featname;
		iss >> featname;
		if (featname.empty())
			continue;
		tiletypes[name].features[featname];
		int bit = -1;
		iss >> bit;
		while (bit != -1) {
			tiletypes[name].features[featname].push_back(bit);
			bit = -1;
			iss >> bit;
		}
	}
}


int main(int argc, char *argv[]) {
	if (argc < 3) {
		std::cerr << "usage: stripdb in/ out/" << std::endl;
		return 2;
	}

	// Currently have poor quality DBs for these tiles,
	// skip outputting them
	std::set<std::string> skip_tiles = {
		"CLEL_L", "CLEM_R", "RCLK_INT_R",
	};

	DIR *dir = opendir(argv[1]);
	std::string p;
	for(struct dirent *dirent = readdir(dir); dirent != nullptr; dirent = readdir(dir)) {
		if ((dirent->d_type & DT_REG) == 0) {
			continue;
		}

		p.assign(dirent->d_name);
		auto idx = p.find_last_of('.');
		if (idx == std::string::npos) {
			continue;
		}

		if (p.substr(idx) != ".bits") {
			continue;
		}

		std::ifstream tiledata(dirent->d_name);
		auto stem = p.substr(0, idx);
		if (skip_tiles.count(stem))
			continue;
		parse_database(stem, tiledata);
	}

	// Misc cleanups
	for (auto &f : tiletypes["INT"].features)
		if (f.first.find(".VCC_WIRE") != std::string::npos)
			f.second.clear();

	for (const auto &tt : tiletypes) {
		if(tt.second.features.empty())
			continue;
		std::string dbname = std::string(argv[2]) + "/" + tt.first + ".bits";
		std::ofstream out(dbname);
		if (!out)
			std::cerr << "failed to open " <<  dbname << " for writing." << std::endl;
		for (auto &f : tt.second.features) {
			out << f.first;
			for (int bit : f.second)
				out << " " << bit;
			out << std::endl;
		}
	}
}
