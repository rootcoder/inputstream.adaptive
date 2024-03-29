// Copyright (c) 2016 The WebM project authors. All Rights Reserved.
//
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE file in the root of the source
// tree. An additional intellectual property rights grant can be found
// in the file PATENTS.  All contributing project authors may
// be found in the AUTHORS file in the root of the source tree.
#ifndef SRC_CUES_PARSER_H_
#define SRC_CUES_PARSER_H_

#include "cue_point_parser.h"
#include "master_parser.h"
#include "webm/id.h"

namespace webm {

// Spec reference:
// http://matroska.org/technical/specs/index.html#Cues
// http://www.webmproject.org/docs/container/#Cues
class CuesParser : public MasterParser {
 public:
  CuesParser() : MasterParser(MakeChild<CuePointParser>(Id::kCuePoint)) {}
};

}  // namespace webm

#endif  // SRC_CUES_PARSER_H_
