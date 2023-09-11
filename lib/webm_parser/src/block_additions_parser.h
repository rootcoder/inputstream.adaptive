// Copyright (c) 2016 The WebM project authors. All Rights Reserved.
//
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE file in the root of the source
// tree. An additional intellectual property rights grant can be found
// in the file PATENTS.  All contributing project authors may
// be found in the AUTHORS file in the root of the source tree.
#ifndef SRC_BLOCK_ADDITIONS_PARSER_H_
#define SRC_BLOCK_ADDITIONS_PARSER_H_

#include "block_more_parser.h"
#include "master_value_parser.h"
#include "webm/dom_types.h"
#include "webm/id.h"

namespace webm {

// Spec reference:
// http://matroska.org/technical/specs/index.html#BlockAdditions
// http://www.webmproject.org/docs/container/#BlockAdditions
class BlockAdditionsParser : public MasterValueParser<BlockAdditions> {
 public:
  BlockAdditionsParser()
      : MasterValueParser<BlockAdditions>(MakeChild<BlockMoreParser>(
            Id::kBlockMore, &BlockAdditions::block_mores)) {}
};

}  // namespace webm

#endif  // SRC_BLOCK_ADDITIONS_PARSER_H_
