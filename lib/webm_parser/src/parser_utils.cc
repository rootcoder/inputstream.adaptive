// Copyright (c) 2016 The WebM project authors. All Rights Reserved.
//
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE file in the root of the source
// tree. An additional intellectual property rights grant can be found
// in the file PATENTS.  All contributing project authors may
// be found in the AUTHORS file in the root of the source tree.
#include "parser_utils.h"

#include <cassert>
#include <cstdint>

#include "webm/reader.h"
#include "webm/status.h"

namespace webm {

Status ReadByte(Reader* reader, std::uint8_t* byte) {
  assert(reader != nullptr);
  assert(byte != nullptr);

  std::uint64_t num_bytes_read;
  const Status status = reader->Read(1, byte, &num_bytes_read);

  if (!status.completed_ok()) {
    assert(num_bytes_read == 0);
  } else {
    assert(num_bytes_read == 1);
  }

  return status;
}

}  // namespace webm
