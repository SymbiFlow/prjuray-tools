/*
 * Copyright 2017-2022 F4PGA Authors
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// Always include assert's, never disable.
#undef NDEBUG
#include <cassert>

#include <vector>
#include <iostream>
#include <map>
#include <string>
#include <fstream>
#include <stdexcept>
#include <iterator>
#include <stdarg.h>
#include <unordered_set>
#include <unordered_map>
#include <set>
#include <sstream>
#include <algorithm>
#include <dirent.h>

#include <gflags/gflags.h>

#include "absl/strings/str_split.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_format.h"
#include "absl/hash/hash.h"

DEFINE_string(root, ".", "set output directory");
DEFINE_bool(verbose, false, "Verbose output in segbits");

struct Bit : public std::pair<int, int> {
    Bit() : std::pair<int, int>(0, 0) {}
};

std::string output_bit(Bit bit) {
    return absl::StrFormat("%02d_%02d", bit.first, bit.second);
}

Bit parse_bit(const std::string &name) {
    std::vector<std::string> parts = absl::StrSplit(name, "_");
    assert(parts.size() == 2);
    Bit bit;
    bool result = absl::SimpleAtoi(parts[0], &bit.first);
    assert(result);
    result = absl::SimpleAtoi(parts[1], &bit.second);
    assert(result);

    return bit;
}

typedef std::unordered_set<Bit, absl::Hash<Bit>> BitSet;

struct FeatureData {
    BitSet always_set_with_feature;
    BitSet never_set_with_feature;
    BitSet always_set_without_feature;

    std::vector<std::pair<Bit, bool>> featbits;
    std::set<std::string> deps;
    int count;
};

struct TileTypeData {
    std::unordered_map<std::string, std::unordered_set<std::string>> inst_features;
    std::unordered_map<std::string, BitSet> inst_bits;
    BitSet settable_bits;
    std::set<std::string> extant_features;
    std::unordered_map<std::string, FeatureData> features;
};


class Correlator {
public:
    void parse_bits_and_features(const std::string &tile_type, std::istream &in);
    void correlate_features();
    void output_features(const std::string &root, bool verbose) const;
private:
    std::map<std::string, TileTypeData> tiletypes_;
    std::set<std::string> include_tts_;
};

std::pair<std::string, std::string> split_tilename(const std::string &name) {
    size_t idx = name.find(':');
    return std::make_pair(name.substr(0, idx), name.substr(idx+1));
}

void Correlator::parse_bits_and_features(const std::string &tile_type_str, std::istream &in) {
    std::string line;
    std::string type;
    std::string data;
    auto &tile_type = tiletypes_[tile_type_str];

    BitSet *settable_bits = &tile_type.settable_bits;
    std::set<std::string> *extant_features = &tile_type.extant_features;

    BitSet *inst_bits = nullptr;
    std::unordered_set<std::string> *inst_features = nullptr;

    std::string seg;
    while (std::getline(in, line)) {
        if (line.empty())
            continue;

        std::istringstream iss(line);
        iss >> type;
        iss >> data;

        if(type == "seg") {
            seg = data;
            inst_bits = &tile_type.inst_bits[data];
            inst_features = &tile_type.inst_features[data];
        } else if(type == "bit") {
            Bit bit = parse_bit(data);
            settable_bits->insert(bit);
            inst_bits->insert(bit);
        } else if(type == "tag") {
            extant_features->insert(data);
            int is_set;
            iss >> is_set;
            if(is_set) {
                inst_features->insert(data);
            }
        }
    }
}

template <typename Tc, typename Tv, typename Tf> void set_erase_if(Tc &target, std::vector<Tv> &temp, Tf pred) {
    temp.clear();
    for (auto &entry : target) {
        if (pred(entry)) {
            temp.push_back(entry);
        }
    }
    for (auto &toerase : temp) {
        target.erase(toerase);
    }
}

void find_feature_deps(TileTypeData &tt) {
    for (auto &f : tt.extant_features) {
        auto &fd = tt.features[f];
        fd.deps = tt.extant_features;
        fd.deps.erase(f);
        std::vector<std::string> temp;
        for (auto &inst : tt.inst_features) {
            if (!inst.second.count(f)) {
                continue;
            }
            set_erase_if(fd.deps, temp, [&](const std::string &ef){ return !inst.second.count(ef); });
        }
    }
}

void process_feature(TileTypeData &tt, const std::string &feature) {

    FeatureData &fd = tt.features[feature];

    fd.always_set_with_feature = tt.settable_bits;
    //fd.never_set_with_feature = tt.settable_bits;
    fd.always_set_without_feature = tt.settable_bits;

    std::vector<Bit> temp;
    fd.count = 0;
    bool always_have_feature = true;
    for (auto &inst : tt.inst_bits) {
        if (!tt.inst_features.count(inst.first)) {
            continue;
        }
        auto &ib = inst.second;
        bool has_feature = tt.inst_features.at(inst.first).count(feature);
        if (!has_feature) {
            always_have_feature = false;
        }

        if (has_feature) {
            ++fd.count;
        }

        if (has_feature) {
            set_erase_if(fd.always_set_with_feature, temp, [&](Bit bit){ return !ib.count(bit); });
        }
        //if (has_feature)
        //  set_erase_if(fd.never_set_with_feature, temp, [&](int bit){ return ib.count(bit); });
        if (!has_feature) {
            set_erase_if(fd.always_set_without_feature, temp, [&](Bit bit){ return !ib.count(bit); });
        }
    }

    for (Bit as : fd.always_set_with_feature) {
        if (always_have_feature || !fd.always_set_without_feature.count(as)) {
            if (std::all_of(fd.deps.begin(), fd.deps.end(), [&](const std::string &dep) {
                        auto &dd = tt.features[dep];
                        return std::find(dd.featbits.begin(), dd.featbits.end(), std::make_pair(as, false)) == dd.featbits.end();
                    })) {
                fd.featbits.emplace_back(as, false);
            }
        }
    }

    std::sort(fd.featbits.begin(), fd.featbits.end());
}

void Correlator::correlate_features() {
    for (auto &tiletype : tiletypes_) {
        auto &t = tiletype.second;
        find_feature_deps(tiletype.second);
        std::vector<std::string> ord_feats(t.extant_features.begin(),
            t.extant_features.end());
        std::stable_sort(ord_feats.begin(), ord_feats.end(), [&](const std::string &a, const std::string &b) {
            return t.features[a].deps.size() < t.features[b].deps.size();
        });

        for (auto &feat : ord_feats) {
            std::cerr << "Processing " << tiletype.first << "." << feat << std::endl;
            FeatureData fd;
            process_feature(t, feat);
        }
    }
}

void Correlator::output_features(const std::string &root, bool verbose) const {
    for (auto &tiletype : tiletypes_) {
        auto &t = tiletype.second;

        std::ofstream td(absl::StrCat(FLAGS_root, "/segbits_", tiletype.first + ".db"));
        for (auto &feat : t.extant_features) {
            auto &fd = t.features.at(feat);
            if (fd.count < 2) {
                continue;
            }
            td << feat;

            for (auto &fb : fd.featbits) {
                td << " " << (fb.second ? "!" : "") << output_bit(fb.first);
            }

            if(verbose) {
                td << " # count: " << fd.count << ", nfeats " << fd.featbits.size();

                if  (fd.deps.size() > 0) {
                    td << ", deps: ";
                    for (auto &d : fd.deps)
                        td << " " << d;
                }
            }

            td << std::endl;
        }
    }
}

int main(int argc, char *argv[]) {
    gflags::SetUsageMessage(
        absl::StrCat("Usage: ", argv[0], " [options] file.."));
    gflags::ParseCommandLineFlags(&argc, &argv, true);
    if (argc <= 1) {
        std::cerr << "usage: correlate_segdata segdata+" << std::endl;
        return -1;
    }

    Correlator correlator;

    for (int optind = 1; optind < argc; optind++) {
        printf("Reading %s.\n", argv[optind]);

        std::ifstream f;
        f.open(argv[optind]);

        // Check if input file exists.
        if (!f.good()) {
            std::cerr << "ERROR: Input file does not exist!" << std::endl;
            return -1;
        }

        assert(!f.fail());

        std::string file(argv[optind]);
        auto index = file.find_last_of('/');
        if(index != std::string::npos) {
            file = file.substr(index+1);
        }
        auto start_index = file.find_first_of('_');
        assert(start_index != std::string::npos);

        auto end_index = file.find_last_of('.');
        assert(end_index != std::string::npos);

        assert(end_index > start_index);

        if(file.substr(0, start_index+1) != "segdata_") {
            std::cerr << "Filename: " << file << " prefix: " << file.substr(0, start_index+1) << std::endl;
            assert(file.substr(0, start_index+1) == "segdata_");
        }

        auto tile_type = file.substr(start_index+1, end_index - start_index - 1);

        correlator.parse_bits_and_features(tile_type, f);
    }

    correlator.correlate_features();
    correlator.output_features(FLAGS_root, FLAGS_verbose);

    return 0;
}
